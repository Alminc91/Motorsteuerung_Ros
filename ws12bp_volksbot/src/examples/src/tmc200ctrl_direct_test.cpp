#include <libtmc200ctrl_direct.h>
#include <tmc200cfgxmlparser.h>

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>


static int bOk=1;

static void nonblock(int state)
{
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (state==1)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==0)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

static int kbhit()
{
	static struct timeval tv = {0,0};
	static fd_set fds;
	
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

static void GMODEClbk(const char* data) {
	std::cout << "Mode: " << data << std::endl;
}


static void GVERSClbk(const char* data) {
	
}

static void SVClbk(const char* data) {
	std::cout << "SV: " << data << std::endl;
}



static void processTMC200Feedback(TMC200Commands cmd,const char* data) {
	std::cout << tmc200ctrl_direct_getTMC200CommandKeyword(cmd) << ": " << data << std::endl;
}

static void ErrorStateClbk(TMC200CtrlError error) {
	switch(error) {
		case TMC200CTRL_ERROR_RTO:
			std::cout << "Error: Reading timeout" << std::endl;
			break;
		case TMC200CTRL_ERROR_WTO:
			std::cout << "Error: Writing timeout" << std::endl;
			break;
		case TMC200CTRL_ERROR_CMD_UNKNOWN:
			std::cout << "Error: Unknown command." << std::endl;
			break;
		default:
			break;
	}
}

int main(int argc, char **argv) {	

	if(tmc200ctrl_direct_init(argv[1])<0) {
		std::cout << "Error initializing library!" << std::endl;
		return -1;
	}

	tmc200ctrl_direct_setErrorProcessingFunction(ErrorStateClbk);
	tmc200ctrl_direct_setTMC200FeedbackProcessingFunction(processTMC200Feedback);

	if(argv[2]!=NULL) {
		TMC200CTRL::TMC200CfgXmlParser	tConfigurator(tmc200ctrl_direct_setCmdWithArgsStr);

		try {
			tConfigurator.sendConfigurationFromFile(argv[2]);
		} catch(TMC200CTRL::TMC200CfgXmlParserException& exc) {
			std::cout << "Exception thrown: " << exc.what() << std::endl;
		}
	}

	tmc200ctrl_direct_sendSVRepetitive(1);

	nonblock(1);
	
	int i=0;
	char c;
	
	int v=0;
	
	while(bOk==1) {
		usleep(1);
		i=kbhit();
		
		if(i!=0) {
			c=fgetc(stdin);
		
			switch(c) {
				case 'q':
					bOk=0;
					break;
				case '+':
					v+=10;
					tmc200ctrl_direct_SV(v,v,v);
					break;
				case '-':
					v-=10;
					tmc200ctrl_direct_SV(v,v,v);
					break;
				default:				
					break;
			}
		}
	}

	tmc200ctrl_direct_shutdown();
}

