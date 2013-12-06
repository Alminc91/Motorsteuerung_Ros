#include <libtmc200ctrl_io.h>
#include <threads_common.h>
#include <queues.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


//Maximum length of the argument string.
#define ARGSBUFFER_LEN 			(31)

//The suffix which must be appended to every TMC200 command.
#define TMC200CTRL_CMD_SUFFIX	" \n"

//External forward declaration of the TMC200FeedBack thread function.
extern void* TMC200FeedBackThreadFunc(void*);

//External forward declaration of the sendTMC200Cmds thread function.
extern void* sendTMC200CmdsThreadFunc(void*);

//File descriptor of the serial connection.
static int				iFd;

//Backup for the serial port settings to restore them on program exit.
static struct termios 	oldtio;	

/*
 * Char buffer to store the current command arguments
 * which has been converted from integer to char.
 */
static char				cArgsBuffer[ARGSBUFFER_LEN+1] = {'\x0'};


/*
 * Close the connection to the TMC200 and
 * restores the settings of the serial port.
 */
static void closeFd() {
	if(iFd>0) {
		tcsetattr(iFd,TCSANOW,&oldtio);
		close(iFd);
	}
}

/*
 * Initialize the serial port connection.
 */
static int initSerialPort(const char* port) {		

	//New settings.
	struct termios newtio;
	
	//File descriptor.
	int fd = -1;
	
	fd = open (port,O_RDWR | O_NOCTTY | O_NONBLOCK);

	//Return if fd is invalid.
	if(fd<0)
		return TMC200CTRL_ERROR_INVFILEDESC;

	//If fd is not of type 'tty', we close it and return.
	if(!isatty(fd)) {
		close(fd);
		return TMC200CTRL_ERROR_NOTTY;
	}

	//Save current serial port settings
	tcgetattr(fd,&oldtio); 
	
	//Clear struct for new port settings
	memset(&newtio,0, sizeof(newtio)); 

	newtio.c_iflag &= ~( BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	newtio.c_oflag = 0;

	newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	newtio.c_cflag &= ~(CSIZE | PARENB);
	newtio.c_cflag |= CS8;
	newtio.c_cflag |= CS8|CREAD|CLOCAL;

	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME] = 0;

	//In- and output Baud rate.
	cfsetispeed(&newtio, B57600);
	cfsetospeed(&newtio, B57600);

	//Flush old settings.
	tcflush(fd, TCIFLUSH);

	//Apply the new settings.
	tcsetattr(fd,TCSANOW,&newtio);

	//Initialization succeeded.
	return fd;
}

int tmc200ctrl_io_setCmd(TMC200Commands cmd) {
	return tmc200ctrl_io_setCmdWithArgsStr(cmd,NULL);
}

int tmc200ctrl_io_setCmdWithOneArg(TMC200Commands cmd,int arg1) {
	snprintf(cArgsBuffer,ARGSBUFFER_LEN,"%d", arg1);
	return tmc200ctrl_io_setCmdWithArgsStr(cmd,cArgsBuffer);
}

int tmc200ctrl_io_setCmdWithTwoArgs(TMC200Commands cmd,int arg1,int arg2) {
	snprintf(cArgsBuffer,ARGSBUFFER_LEN,"%d %d", arg1,arg2);
	return tmc200ctrl_io_setCmdWithArgsStr(cmd,cArgsBuffer);
}

int tmc200ctrl_io_setCmdWithThreeArgs(TMC200Commands cmd,int arg1,int arg2,int arg3) {
	snprintf(cArgsBuffer,ARGSBUFFER_LEN,"%d %d %d", arg1,arg2,arg3);
	return tmc200ctrl_io_setCmdWithArgsStr(cmd,cArgsBuffer);
}

const char* tmc200ctrl_io_getTMC200CommandKeyword(TMC200Commands cmd) {
	return TMC200CommandsKeywords[cmd];
}

int tmc200ctrl_io_setCmdWithArgsStr(TMC200Commands cmd,const char* cmdargsstr) {	
	//Return if SendTMC200Cmd thread isn't running.
	if(isThreadAlive(SENDTMC200CMDS)==FALSE) {
		#ifdef DEBUG
		printf("[LIBTMC200CTRL copyCmdStrToTMC200CmdThread] TMC200CmdThread death.\n");
		#endif
		return TMC200CTRL_ERROR_NO_SENDCMDTHREAD;
	}

	//Obtain a new item.
	TMC200CmdQueueItem* ptItem = getFreshItemFromQueue(SENDCMDS);
	
	//Set the command.
	ptItem->eCmd = cmd;

	//Copy over the command.
	strcpy(ptItem->ptData,TMC200CommandsKeywords[cmd]);

	//Append the arguments string if available.
	if(cmdargsstr!=NULL) {
		strcat(ptItem->ptData," ");
		strcat(ptItem->ptData,cmdargsstr);
	}
	
	//Append the command suffix.
	strcat(ptItem->ptData,TMC200CTRL_CMD_SUFFIX);
	
	#ifdef DEBUG
	printf("[LIBTMC200CTRL copyCmdStrToTMC200CmdThread] Finished comp.: '%s'\n",ptItem->ptData);
	#endif

	//Append the item to the outgoing queue.
	addItemToQueue(SENDCMDS,ptItem);

	//Resume the thread if it was paused.				
	if(isThreadPaused(SENDTMC200CMDS)==TRUE)
		resumeThread(SENDTMC200CMDS);
	
	return TMC200CTRL_ERROR_NONE;	
}

int tmc200ctrl_io_init(const char* port) {
	int ret;
	iFd = initSerialPort(port);
	
	//Return if the initialization of the serial connection has failed.
	if(iFd<0)
		return iFd;
	
	//Initialize the outgoing queue.
	#ifdef DEBUG
	initQueue(SENDCMDS,"SendCmds Queue",CMDQUEUE_DATA_SIZE);
	#else
	initQueue(SENDCMDS,CMDQUEUE_DATA_SIZE);
	#endif	
	
	//Initialize the SendTMC200Cmd thread.
	#ifdef DEBUG
	ret = initThread(SENDTMC200CMDS,"SendTMC200CmdsThread",sendTMC200CmdsThreadFunc,(void*)&iFd);
	#else
	ret = initThread(SENDTMC200CMDS,sendTMC200CmdsThreadFunc,(void*)&iFd);
	#endif

	//Cancel on failure.
	if(ret != 0) { 
		closeFd();
		return TMC200CTRL_ERROR_INITTHREADS;
	}

	//Initialize the incoming queue.	
	#ifdef DEBUG
	initQueue(FEEDBACK,"Feedback Queue",FEEDBACKQUEUE_DATA_SIZE);
	#else
	initQueue(FEEDBACK,FEEDBACKQUEUE_DATA_SIZE);
	#endif

	//Initialize the TMC200FeedBack thread.
	#ifdef DEBUG
	ret = initThread(TMC200FEEDBACK,"TMC200FeedBack",TMC200FeedBackThreadFunc,NULL);
	#else
	ret = initThread(TMC200FEEDBACK,TMC200FeedBackThreadFunc,NULL);
	#endif

	//Cancel on failure.
	if(ret != 0) { 
		closeFd();
		shutdownThread(SENDTMC200CMDS);
		return TMC200CTRL_ERROR_INITTHREADS;
	}
		
	return TMC200CTRL_ERROR_NONE;
}

void tmc200ctrl_io_shutdown() {		
	//Shutdown all threads.
	shutdownThread(SENDTMC200CMDS);	
	shutdownThread(TMC200FEEDBACK);
	
	//Cleanup the queues.
	cleanUpQueue(FEEDBACK);
	cleanUpQueue(SENDCMDS);

	//Close the serial connections.
	closeFd();
}

