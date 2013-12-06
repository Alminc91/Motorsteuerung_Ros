#include <ros/ros.h>

#include <std_msgs/String.h>
#include <std_msgs/Int8.h>

#include <tmc200ctrl/SV.h>

#include <tmc200ctrl_error.h>

#include <tmc200svfeedbackparser.h>

#include <iostream>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

using namespace TMC200CTRL;
using namespace tmc200ctrl;

static int kfd = 0;
static struct termios cooked, raw;


static TMC200SVFeedbackParser tFBParser;


static void SVFBSubsClbk(const std_msgs::String::ConstPtr& msg) {
	tFBParser.parseSVFeedbackString(msg->data.c_str());
	std::pair<string,vector<int> > entry;

	while(tFBParser.hasNext()) {
		entry = tFBParser.getNext();
		
		cout << entry.first << ": { ";
		
		for(int i=0;i<entry.second.size();i++) {
			std::cout << entry.second[i] << " ";
		}
		
		std::cout << "} ";
	}
	
	std::cout << std::endl;
}

static void errSubsClbk(const std_msgs::Int8::ConstPtr& msg) {
	switch((TMC200CtrlError)msg->data) {
		case TMC200CTRL_ERROR_RTO:
			std::cerr << "ERROR: Read timeout." << std::endl;
			break;
		case TMC200CTRL_ERROR_WTO:
			std::cerr << "ERROR: Write timeout." << std::endl;
			break;
		case TMC200CTRL_ERROR_CMD_UNKNOWN:
			std::cerr << "ERROR: Unknown command." << std::endl;
			break;
		case TMC200CTRL_ERROR_NO_SENDCMDTHREAD:
			std::cerr << "ERROR: SendTMC200CmdsThread isn't running." << std::endl;
			break;			
		default:
			break;
	}
}


int main(int argc, char **argv) {	
	char	c;
	bool	bRun=true;
	int		m=0;
	int		iRight=0;
	int		iLeft=0;


	// get the console in raw mode
	tcgetattr(kfd, &cooked);
	memcpy(&raw, &cooked, sizeof(struct termios));
	raw.c_lflag &=~ (ICANON | ECHO);
	// Setting a new line, then end of file
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	tcsetattr(kfd, TCSANOW, &raw);

	ros::init(argc,argv,"remoteTestNode");
	
	ros::NodeHandle	tNode;
	
	ros::Publisher	tSVPubl = tNode.advertise<SV>("Velocity",20);
	
		ros::Subscriber tErrSubs = tNode.subscribe<std_msgs::Int8>("TMC200CtrlErrors",10,errSubsClbk);

	
	ros::Subscriber tSVFBSubs = tNode.subscribe<std_msgs::String>("TMC200SVFeedback",10,SVFBSubsClbk);

	SV	svMsg;

	while(bRun) {
		// get the next event from the keyboard
		if(read(kfd, &c, 1) < 0) {
			perror("read():");
			break;
		}
				
		switch(c) {
			case 'q':
				bRun=false;
				break;
			case 'w':
				m+=10;
				svMsg.m1 = m+iLeft;
				svMsg.m2 = m+iRight;
				svMsg.m3 = 0;
				
				tSVPubl.publish(svMsg);
				break;
			case 's':
				m-=10;
				
				svMsg.m1 = m+iLeft;
				svMsg.m2 = m+iRight;
				svMsg.m3 = 0;
				
				tSVPubl.publish(svMsg);
				break;
			case 'a':
				iLeft+=10;
				iRight-=10;
				
				svMsg.m1 = m+iLeft;
				svMsg.m2 = m+iRight;
				svMsg.m3 = 0;
				
				tSVPubl.publish(svMsg);
				break;
			case 'd':
				iRight+=10;
				iLeft-=10;
				
				svMsg.m1 = m+iLeft;
				svMsg.m2 = m+iRight;
				svMsg.m3 = 0;
				
				tSVPubl.publish(svMsg);
				break;
			
			case ' ':
				break;
				
			default:
				break;
		}
	}
	
	ros::shutdown();
	tcsetattr(kfd, TCSANOW, &cooked);

	return 0;
}
