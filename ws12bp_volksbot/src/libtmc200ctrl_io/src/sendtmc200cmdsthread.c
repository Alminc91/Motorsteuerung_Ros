#include <tmc200ctrl_error.h>
#include <queues.h>
#include <threads_common.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//TMC 200 error indication string.
#define TMC200ERRORSTR ("ERR ")

//Timeout when sending or receiving data.
#define MILLSECS_RW_TIMEOUT	(500)

//Whether the current command shall be sent repeatedly.
static int		bResendCurCmd=FALSE;

//Whether the current command is an SV command.
static int		bResendSV=FALSE;

//Function pointer to the error callback function.
static void		(*pfErrorStateClbk)(TMC200CtrlError) = NULL;

// 1 Millsec time out.
static const struct timespec	tOneMilliSec = {0,1000};


/*
 * Call the error processing function if it has been set.
 */
static void setErrorState(TMC200CtrlError error) {
	if(pfErrorStateClbk!=NULL)
		pfErrorStateClbk(error);	
}

/*
 * Sends data to the controller.
 */
static int sendData(int fd, const char* str) {
	//How many bytes.
	const int	iLen=strlen(str);
	
	//The current position (How many bytes has been sent already):
	int			iCurPos=0;
	//How many bytes were sent in the last iteration.
	int			iBytesProcessed=0;
	//Attempts to send data.
	int			iTrys=0;

	while(iCurPos<iLen) {
		iBytesProcessed=write(fd,str+iCurPos,iLen-iCurPos);

		if(iBytesProcessed>0) {
			//If some data was sent, increase iCurPos and set iTrys to 0.
			iCurPos+=iBytesProcessed;
			iTrys=0;
		} else if(iTrys>MILLSECS_RW_TIMEOUT)
			//Cancel if timeout is over.
			break;
		else {
			//Wait, then increment iTrys.
			nanosleep(&tOneMilliSec,NULL);
			iTrys++;
		}
	}
	
	return iCurPos;
}

static int receiveData(int fd,TMC200Commands cmd) {
	//The current position (How many bytes has been received already):
	int			iCurPos=0;
	//How many bytes were received in the last iteration.
	int			iBytesProcessed=0;
	//Attempts to receive data.
	int			iTrys=0;

	//Obtain a new item from the incoming queue.
	TMC200CmdQueueItem*	ptItem = getFreshItemFromQueue(FEEDBACK);

	while(1) {
		iBytesProcessed=read(fd,ptItem->ptData+iCurPos,FEEDBACKQUEUE_DATA_SIZE-iCurPos);

		if(iBytesProcessed>0) {
			//If some data was received, increase iCurPos.
			iCurPos+=iBytesProcessed;

			//Leave if a line break occurred.
			if(	((iCurPos-2)>0) && 
				(ptItem->ptData[iCurPos-2]=='\n'))
				break;
			
			iTrys=0;
		} else if(iTrys>MILLSECS_RW_TIMEOUT)
			//Cancel if timeout is over.
			break;
		else {
			//Wait, then increment iTrys.
			nanosleep(&tOneMilliSec,NULL);
			iTrys++;
		}
	}

	if(iCurPos>0) {
		//Set the null byte properly.
		ptItem->ptData[iCurPos-2]='\x0';
	
		/*
		 * Rise an error if the TMC200 reports an error.
		 */
		if(strcmp(ptItem->ptData,TMC200ERRORSTR)==0) {
			setErrorState(TMC200CTRL_ERROR_CMD_UNKNOWN);
		
			recycleItemAtQueue(FEEDBACK,ptItem);
		} else {			
			//Set the command.
			ptItem->eCmd=cmd;

			//Add the item to the queue.
			addItemToQueue(FEEDBACK,ptItem);
		
			//Wake up the thread if if necessary.
			if(isThreadPaused(TMC200FEEDBACK)==TRUE)
				resumeThread(TMC200FEEDBACK);
		}
	} else {
		//Otherwise recycle the item.
	
		#if DEBUG
		printf("[LIBTMC200CTRL receiveData] ERROR RECIVING DATA.\n");
		#endif
		recycleItemAtQueue(FEEDBACK,ptItem);
	}
		
	return iCurPos;
}

void* sendTMC200CmdsThreadFunc(void* data) {
	//Extract the file desciptor.
	int	iFd=*(int*)data;
	
	//Some temporary variables.
	TMC200CmdQueueItem*	ptItem = NULL;
	int 	iRet,iLen;
	
	#ifdef DEBUG
	printf("[LIBTMC200CTRL] Thread '%s' started.\n",getThreadName(SENDTMC200CMDS));
	#endif
		
	//Loop forever.
	while(1) {
		if(bResendCurCmd==TRUE) {
			//If the current command is sent repeatedly.
			if(queueSize(SENDCMDS)>0) {
				//If the outgoing queue is not empty.
				if(ptItem!=NULL)
					//Recycle the current item if present.
					recycleItemAtQueue(SENDCMDS,ptItem);

				//Get the next item.
				ptItem = popItemFromQueue(SENDCMDS);				
			} else {
				//Exit the thread if it has been stopped.
				if(isThreadAlive(SENDTMC200CMDS)==FALSE) {
					if(ptItem!=NULL)
						recycleItemAtQueue(SENDCMDS,ptItem);

					break;
				}
			}
		} else {
			//If the current command is sent only once.
			if(ptItem!=NULL)
				//Recycle the current item if present.
				recycleItemAtQueue(SENDCMDS,ptItem);

			//Get the next item.
			ptItem = popItemFromQueue(SENDCMDS);
			
			/*
			 * Exit the thread if there are no new items
			 * and the thread has been stopped.
			 */
			if((ptItem==NULL) && (isThreadAlive(SENDTMC200CMDS)==FALSE))
				break;
		}
	
		if(ptItem!=NULL) {
			/*
			 * Make sure that only the SV command is sent repeatedly.
			 */
			if(ptItem->eCmd==TMC200CMD_SV)
				bResendCurCmd = bResendSV;
			else 
				bResendCurCmd=FALSE;

			//Send the data.
			iLen = strlen(ptItem->ptData);
			iRet=sendData(iFd,ptItem->ptData);

			#ifdef DEBUG
			printf(	"[LIBTMC200CTRL] Thread '%s': Sent %d bytes of '%s'.\n",
					getThreadName(SENDTMC200CMDS),iRet,ptItem->ptData);
			#endif

			if(iRet==iLen) {
				//If all data has been sent successfully, receive the feedback.
				iRet=receiveData(iFd,ptItem->eCmd);
				
				#ifdef DEBUG
				printf(	"[LIBTMC200CTRL] Thread '%s': Received %d bytes of feedback.\n",
						getThreadName(SENDTMC200CMDS),iRet);
				#endif
								
				//Raise an error if receiving failed.
				if(iRet<=0)
					setErrorState(TMC200CTRL_ERROR_RTO);	
			} else
				//Raise an error if sending failed.
				setErrorState(TMC200CTRL_ERROR_WTO);
		} else {
			//Suspend if there is no more work to do.
			pauseThread(SENDTMC200CMDS);
			checkThreadPaused(SENDTMC200CMDS);
		}
	}
	
	#ifdef DEBUG
	printf("[LIBTMC200CTRL] Thread '%s' finished.\n",getThreadName(SENDTMC200CMDS));
	#endif

	pthread_exit(0);
}

void tmc200ctrl_io_sendSVRepetitive(int val) {
	bResendSV=val;
}

int tmc200ctrl_io_isSendSVRepetitiveSet() {
	return bResendSV;
}


void tmc200ctrl_io_setErrorProcessingFunction(void (*func)(TMC200CtrlError)) {
	pfErrorStateClbk = func;
}

