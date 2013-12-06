//ROS
#include <ros/ros.h>

/*
 * Costume ROS messages.
 */
#include <tmc200ctrl/GVERS.h>
#include <tmc200ctrl/GMODE.h>
#include <tmc200ctrl/SENCO.h>
#include <tmc200ctrl/GENCO.h>
#include <tmc200ctrl/GMDPT.h>
#include <tmc200ctrl/SV.h>
#include <tmc200ctrl/SVREG.h>
#include <tmc200ctrl/GVREG.h>

/*
 * Standard ROS messages.
 */
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Duration.h>
#include <std_msgs/Empty.h>

//libtmc200ctrl_io
#include <libtmc200ctrl_io.h>

/*
 * Standard includes
 */
#include <cstdlib>
#include <ostream>
#include <stdint.h>

using namespace std;
using namespace tmc200ctrl;

#define PARSE_TMC200FEEDBACK_ERROR	(-127)

//Forward decalration of the error handling function.
static void publishTMC200CtrlError(int error);

//Publishes the answer to an velocity command.
static ros::Publisher*	tSVFBPublisher;

//Publish errors that might happen in libtmc200ctrl_io and tmc200controller.
static ros::Publisher*	tTMC200CtrlErrorPublisher;

//When this timer elapse all motors gets cut off.
static ros::Timer*		tCutOffTimer;

//Whether the current SV command should be resend localy.
static bool				isAutoresendSVSet=false;

/*
 * Function to parse an string into a tupel of integers.
 *
 * n: Number of expected values.
 * str: The string to parse
 *
 * returns: returns: A pointer to place where the tupel has been allocated.
 */
static int16_t* extractNShortssFromString(int n,char* str) {
	//Allocate n*2 bytes.
	int16_t*	iRet=(int16_t*)malloc(sizeof(int16_t)*n);
	int16_t		j=0;
	int			i=0;
	
	//Split by spaces.
	char* 		cStrTok = strtok(str," ");
	
	//As long as n isn't reached.
	while(i<n) {
		/*
		 * If there is no further string,
		 * free the memory and return with NULL.
		 */
		if(cStrTok==NULL) {
			if(i<(n-1)) {
				free(iRet);
				iRet=NULL;
			}
			break;
		}
	
		j=(int16_t)strtol(cStrTok,NULL,10);

		if(j!=0L) {
			/*
			 * Vaild number. Add it and
			 * increment the positon.
			 */
			iRet[i]=j;
			i++;
		} else {
			/*
			 * No valid number.
			 * free the memory and return with NULL.
			 */		
			free(iRet);
			iRet=NULL;
		}

		//Next token ...
		cStrTok = strtok(NULL," ");
	}
	
	return iRet;	
}

/*
 * Function that parses the controller feedback.
 * In this case the only the SV command feedback gets published.
 *
 * It is executed by SendTMC200CmdsThread thread.
 */
static void processFeedbackFunc(TMC200Commands cmd,const char* data) {
	std_msgs::String msg;

	switch(cmd) {
		case TMC200CMD_SV:	
			msg.data = data;
			tSVFBPublisher->publish(msg);
			break;
		default:
			break;
	};
}

/*
 * This function publishes errors of libtmc200ctrl_io and tmc200controller.
 * It is executed by SendTMC200CmdsThread thread.
 */
static void processTCM200Ctrl_IO_errors(TMC200CtrlError error) {
	publishTMC200CtrlError(error);
}

/*
 * Shortcut to publish errors.
 */
static void publishTMC200CtrlError(int error) {
	std_msgs::Int8	msg;
	
	msg.data = error;
	ROS_ERROR("An Error happend in libtmc200ctrl_io. code: '%d'\n",error);
	tTMC200CtrlErrorPublisher->publish(msg);
}

/*
 * Subscriber callback for the SMODE command.
 */
static void SMODESubsClbk(const std_msgs::UInt8::ConstPtr& msg) {
	const int ret = tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMODE,msg->data);

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * An additonal hidden callback to receive SV comands via android.
 */
#ifdef PA_DEMONSTRATION
static void androidSVSubsClbk(const std_msgs::String::ConstPtr& msg) {
#else
/*
 * Subscriber callback for the SV command.
 */
static void SVSubsClbk(const SV::ConstPtr& msg) {
#endif

#ifdef PA_DEMONSTRATION
	std::string svStr = msg->data + std::string(" 0");
	const int ret = tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SV,svStr.c_str());
#else
	const int ret = tmc200ctrl_io_setCmdWithThreeArgs(TMC200CMD_SV,
													  msg->m1,msg->m2,msg->m3);
#endif

	if(ret<0)
		publishTMC200CtrlError(ret);

	/*
	 * Only reset the tCutOffTimer when the
	 * automatic local resend commands is enabled.
	 */
	if(isAutoresendSVSet) {
		if(tmc200ctrl_io_isSendSVRepetitiveSet()==0)
			tmc200ctrl_io_sendSVRepetitive(isAutoresendSVSet);

		tCutOffTimer->stop();
		tCutOffTimer->start();
	}
}

/*
 * Subscriber callback for the SSEND command.
 */
static void SSENDSubsClbk(const std_msgs::UInt8::ConstPtr& msg) {
	const int ret = tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SSEND,msg->data);

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * Subscriber callback for the SENCO command.
 */
static void SENCOSubsClbk(const SENCO::ConstPtr& msg) {
	std::stringstream tArgsStr;
	tArgsStr << "M" << (int)msg->motor << " " << msg->encres << " " << msg->rpm << " " << msg->delta;

	const int ret = tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,tArgsStr.str().c_str());

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * Subscriber callback for the SMDPT command.
 */
static void SMDPTSubsClbk(const std_msgs::UInt16::ConstPtr& msg) {
	const int ret = tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMDPT,msg->data);

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * Subscriber callback for AutorepeatVelociyCommand.
 */
static void SVREGSubsClbk(const SVREG::ConstPtr& msg) {
	std::stringstream tArgsStr;
	tArgsStr << "M" << (int)msg->motor << " " << msg->p << " " << msg->i << " " << msg->d;

	const int ret = tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SVREG,tArgsStr.str().c_str());

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * Subscriber callback to set the
 * automatic resend of the current SV command.
 */
static void autoSVSubsClbk(const std_msgs::Bool::ConstPtr& msg) {
	isAutoresendSVSet=msg->data;
	tmc200ctrl_io_sendSVRepetitive(isAutoresendSVSet);
}

/*
 * Callback for CutOffMotorsTimeout.
 */
static void cutofftoSubsClbk(const std_msgs::Duration::ConstPtr& msg) {
	tCutOffTimer->setPeriod(msg->data);
}

/*
 * Service callback for the SRODO command.
 */
static void SRODOSubsClbk(const std_msgs::Empty::ConstPtr& msg) {
	const int ret = tmc200ctrl_io_setCmd(TMC200CMD_SRODO);

	if(ret<0)
		publishTMC200CtrlError(ret);
}

/*
 * Service callback for the GVERS command.
 */
static bool GVERSSrvClbk(GVERS::Request& req, GVERS::Response& resp) {
	const int ret = tmc200ctrl_io_setCmd(TMC200CMD_GVERS);

	if(ret<0) {
		publishTMC200CtrlError(ret);
		
		return false;
	}
	
	char*	cRet=tmc200ctrl_io_waitForTMC200Feedback();
	
	if(cRet!=NULL) {
		resp.version = cRet;
		free(cRet);
	} else
		return false;
	
	return true;
}

/*
 * Service callback for the SMODE command.
 */
static bool GMODESrvClbk(GMODE::Request& req, GMODE::Response& resp) {
	const int ret = tmc200ctrl_io_setCmd(TMC200CMD_GMODE);

	if(ret<0) {	
		publishTMC200CtrlError(ret);
		
		return false;
	}
		
	char*	cRet=tmc200ctrl_io_waitForTMC200Feedback();
	
	if(cRet!=NULL) {
		resp.mode = atoi(cRet+strlen(cRet)-2);		
		free(cRet);
	} else
		return false;
		
	return true;
}

/*
 * Service callback for the GENCO command.
 */
static bool GENCOSrvClbk(GENCO::Request& req, GENCO::Response& resp) {
	std::stringstream tArgsStr;
	tArgsStr << "M" << (int)req.motor;

	const int ret = tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_GENCO,tArgsStr.str().c_str());

	if(ret<0) {
		publishTMC200CtrlError(ret);
		
		return false;
	}
	
	char*	cRet=tmc200ctrl_io_waitForTMC200Feedback();
	
	if(cRet!=NULL) {
		resp.motor = req.motor;

		int16_t*	iRetVals=extractNShortssFromString(3,cRet);
		
		if(iRetVals!=NULL) {
			resp.encres=iRetVals[0];
			resp.rpm=iRetVals[1];
			resp.delta=iRetVals[2];
			
			free(cRet);
		
		} else {
			publishTMC200CtrlError(PARSE_TMC200FEEDBACK_ERROR);
			free(cRet);
			return false;
		}
	} else
		return false;
	
	return true;
}

/*
 * Service callback for the GMDPT command.
 */
static bool GMDPTSrvClbk(GMDPT::Request& req, GMDPT::Response& resp) {
	const int ret = tmc200ctrl_io_setCmd(TMC200CMD_GMDPT);

	if(ret<0) {
		publishTMC200CtrlError(ret);
		
		return false;
	}
	
	char*	cRet=tmc200ctrl_io_waitForTMC200Feedback();
	
	if(cRet!=NULL) {

		int16_t*	iRetVals=extractNShortssFromString(1,cRet);
		
		if(iRetVals!=NULL) {
			resp.RotationAngleConstant=iRetVals[0];
			free(cRet);
		} else {
			publishTMC200CtrlError(PARSE_TMC200FEEDBACK_ERROR);
			free(cRet);
			return false;
		}
	} else
		return false;
	
	return true;
}

/*
 * Service callback for the GVREG command.
 */
static bool GVREGSrvClbk(GVREG::Request& req, GVREG::Response& resp) {
	std::stringstream tArgsStr;
	tArgsStr << "M" << (int)req.motor;

	const int ret = tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_GVREG,tArgsStr.str().c_str());

	if(ret<0) {
		publishTMC200CtrlError(ret);
		
		return false;
	}
	
	char*	cRet=tmc200ctrl_io_waitForTMC200Feedback();
	
	if(cRet!=NULL) {
		resp.motor = req.motor;

		int16_t*	iRetVals=extractNShortssFromString(3,cRet);
		
		if(iRetVals!=NULL) {
			resp.p=iRetVals[0];
			resp.i=iRetVals[1];
			resp.d=iRetVals[2];
			
			free(cRet);
		
		} else {
			publishTMC200CtrlError(PARSE_TMC200FEEDBACK_ERROR);
			free(cRet);
			return false;
		}
	} else
		return false;
	
	return true;
}

/*
 * Callback when 'tCutOffTimer' is elapsed.
 */
static void onCutOffTimerExpire(const ros::TimerEvent& e) {
	tmc200ctrl_io_sendSVRepetitive(0);
}

int main(int argc, char ** argv) {
	int ret;

	/*
	 * Cancel if no port was given.
	 */
	if(argv[1]==NULL) {
		printf("A port must be given: </dev/port");
		return 1;
	}

	ros::init(argc, argv, "tmc200controller");

	ros::NodeHandle tNode;
	
	/*
	 * Initlaize the error publisher.
	 */
	ros::Publisher errPubl=tNode.advertise<std_msgs::Int8>("TMC200CtrlErrors", 10);
	tTMC200CtrlErrorPublisher=&errPubl;
	
	/*
	 * Intialize the tmc200ctrl_io library.
	 */
	ret=tmc200ctrl_io_init(argv[1]);
	
	if(ret<0) {
		printf("Error initializing libtmc200ctrl_io. Error code: '%d'\n",ret); 
		ROS_FATAL("Error initializing libtmc200ctrl_io. Error code: '%d'\n",ret);
		publishTMC200CtrlError(ret);
		ros::shutdown();
		return ret;
	}	
	
	/*
	 * Set the libtmc200ctrl_io callbackfunctions to catch errors and
	 * the TMC200 feedback.
	 */
	tmc200ctrl_io_setTMC200FeedbackProcessingFunction(processFeedbackFunc);
	tmc200ctrl_io_setErrorProcessingFunction(processTCM200Ctrl_IO_errors);
	
	ros::Publisher svfbPubl = tNode.advertise<std_msgs::String>("TMC200SVFeedback", 50);
	tSVFBPublisher=&svfbPubl;

#ifdef PA_DEMONSTRATION
/*
 * An additonal subsciber receive SV comands via android.
 */
ros::Subscriber androidSVSubs = tNode.subscribe<std_msgs::String>("motor_speeds",10,androidSVSubsClbk);
#else
/*
 * Initalize the SV feedback publisher.
 */
 	ros::Subscriber svSubs = tNode.subscribe<SV>("Velocity",10,SVSubsClbk);
#endif	 

	
	std::string	tParamStr;
	ros::NodeHandle tParamNode("~");
	
	int	iVal;

	/*
	 * Reads the parameters on the parameter server if available.
	 */	
	if(tParamNode.hasParam("Mode")) {
		tParamNode.getParam("Mode",iVal);
		tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMODE,iVal);
	} if(tParamNode.hasParam("VelocityFeedbackVerbosity")) {
		tParamNode.getParam("VelocityFeedbackVerbosity",iVal);
		tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SSEND,iVal);
	} if(tParamNode.hasParam("EncoderConstantsM1")) {
		tParamNode.getParam("EncoderConstantsM1",tParamStr);
		std::string strVal = std::string("M1 ") + tParamStr;
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,strVal.c_str());
	} if(tParamNode.hasParam("EncoderConstantsM2")) {
		tParamNode.getParam("EncoderConstantsM2",tParamStr);
		std::string strVal = std::string("M2 ") + tParamStr;
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,strVal.c_str());
	} if(tParamNode.hasParam("EncoderConstantsM3")) {
		tParamNode.getParam("EncoderConstantsM3",tParamStr);
		std::string strVal = std::string("M3 ") + tParamStr;
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,strVal.c_str());
	} if(tParamNode.hasParam("RotationAngleConstant")) {
		tParamNode.getParam("RotationAngleConstant",iVal);
		tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMDPT,iVal);
	} if(tParamNode.hasParam("VelocityRegulatorsConstantsM1")) {
		tParamNode.getParam("VelocityRegulatorsConstantsM1",tParamStr);
		std::string strVal = std::string("M1 ") + tParamStr;		
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SVREG,strVal.c_str());
	} if(tParamNode.hasParam("VelocityRegulatorsConstantsM2")) {
		tParamNode.getParam("VelocityRegulatorsConstantsM2",tParamStr);
		std::string strVal = std::string("M2 ") + tParamStr;
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SVREG,strVal.c_str());
	} if(tParamNode.hasParam("VelocityRegulatorsConstantsM3")) {
		tParamNode.getParam("VelocityRegulatorsConstantsM3",tParamStr);
		std::string strVal = std::string("M3 ") + tParamStr;		
		tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SVREG,strVal.c_str());
	} if(tParamNode.hasParam("AutorepeatVelociyCommand")) {
		bool bVal;
		tParamNode.getParam("AutorepeatVelociyCommand",bVal);
		
		isAutoresendSVSet=bVal;
		tmc200ctrl_io_sendSVRepetitive(isAutoresendSVSet);
	}
	
	/*
	 * Initialize the CutoffTimer.
	 */
	ros::Timer cutoffTimer = tNode.createTimer(ros::Duration(1,0),onCutOffTimerExpire,false,false);
	tCutOffTimer = &cutoffTimer;
	
	/*
	 * Update its period with the one on the parameter server.
	 */
	if(tParamNode.hasParam("CutOffMotorsTimeout")) {	
		tParamNode.getParam("CutOffMotorsTimeout",tParamStr);
		
		int16_t* iRet = extractNShortssFromString(2,const_cast<char*>(tParamStr.c_str()));
		
		if(iRet!=NULL) {
			tCutOffTimer->setPeriod(ros::Duration(iRet[0],iRet[1]));	
			free(iRet);
		}
	}
	
	/*
	 * initlaize the subscribers
	 */
	ros::Subscriber smodeSubs = tNode.subscribe<std_msgs::UInt8>("Mode",2,SMODESubsClbk);
	ros::Subscriber ssendSubs = tNode.subscribe<std_msgs::UInt8>("VelocityFeedbackVerbosity",2,SSENDSubsClbk);
	ros::Subscriber sencoSubs = tNode.subscribe<SENCO>("EncoderConstants",2,SENCOSubsClbk);
	ros::Subscriber smdptSubs = tNode.subscribe<std_msgs::UInt16>("RotationAngleConstant",2,SMDPTSubsClbk);
	ros::Subscriber svregtSubs = tNode.subscribe<SVREG>("VelocityRegulatorsConstants",2,SVREGSubsClbk);
	ros::Subscriber autosvSubs = tNode.subscribe<std_msgs::Bool>("AutorepeatVelociyCommand",2,autoSVSubsClbk);
	ros::Subscriber cutofftoSubs = tNode.subscribe<std_msgs::Duration>("CutOffMotorsTimeout",2,cutofftoSubsClbk);
	ros::Subscriber srodoSubs = tNode.subscribe<std_msgs::Empty>("ResetOdometry",2,SRODOSubsClbk);
	
	/*
	 * initlaize the services.
	 */
	ros::ServiceServer gversSrv = tNode.advertiseService("GetTMC200SWVers",GVERSSrvClbk);
	ros::ServiceServer gmodeSrv = tNode.advertiseService("GetMode",GMODESrvClbk);
	ros::ServiceServer gencoSrv = tNode.advertiseService("GetEncoderConstants",GENCOSrvClbk);
	ros::ServiceServer gmdptSrv = tNode.advertiseService("GetRotationAngleConstant",GMDPTSrvClbk);
	ros::ServiceServer gvregSrv = tNode.advertiseService("GetVelocityRegulatorsConstants",GVREGSrvClbk);
	
	ROS_INFO("Controller started ...");
	
	/*
	 * Spin until ctrl+c was pressed.
	 */
	ros::spin();
	
	/*
	 * Shutdown everything.
	 */
	
	ROS_INFO("Shuting down ...");

	ros::shutdown();
	tmc200ctrl_io_shutdown();
	
	return 0;
}

