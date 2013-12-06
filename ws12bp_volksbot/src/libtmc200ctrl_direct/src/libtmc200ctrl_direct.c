#include <libtmc200ctrl_io.h>
#include <libtmc200ctrl_direct.h>
#include <stdio.h>

#define ARGSSTR_LEN (31)

static char	cArgsStr[ARGSSTR_LEN+1]={'\x0'};

int tmc200ctrl_direct_init(const char* port) {
	return tmc200ctrl_io_init(port);
}

void tmc200ctrl_direct_setCmdWithArgsStr(TMC200Commands cmd,const char* cmdargsstr) {
	tmc200ctrl_io_setCmdWithArgsStr(cmd,cmdargsstr);
}

int tmc200ctrl_direct_GVERS() {
	return tmc200ctrl_io_setCmd(TMC200CMD_GVERS);
}

int tmc200ctrl_direct_GMODE() {
	return tmc200ctrl_io_setCmd(TMC200CMD_GMODE);
}

int tmc200ctrl_direct_SMODE(TMC200OperationModes mode) {
	return tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMODE,mode);
}

int tmc200ctrl_direct_SV(int m1, int m2, int m3) {
	return tmc200ctrl_io_setCmdWithThreeArgs(TMC200CMD_SV,m1,m2,m3);
}

int tmc200ctrl_direct_SSEND(int mode) {
	return tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SSEND,mode);
}

int tmc200ctrl_direct_SMZNM(int timeout) {
	return tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMZNM,timeout);
}

int tmc200ctrl_direct_GMZNM(){
	return tmc200ctrl_io_setCmd(TMC200CMD_GMZNM);
}

int tmc200ctrl_direct_SENCO(TMC200Motors motor,int encres, int drive, int delta){
	snprintf(cArgsStr,ARGSSTR_LEN,"M%d %d %d %d",motor,encres,drive,delta);
	return tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,cArgsStr);
}

int tmc200ctrl_direct_GENCO(TMC200Motors motor){
	snprintf(cArgsStr,ARGSSTR_LEN,"M%d",motor);
	return tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,cArgsStr);
}

int tmc200ctrl_direct_SMDPT(int odomRotAngle){
	return tmc200ctrl_io_setCmdWithOneArg(TMC200CMD_SMDPT,odomRotAngle);
}

int tmc200ctrl_direct_SRODO(){
	return tmc200ctrl_io_setCmd(TMC200CMD_SRODO);
}

int tmc200ctrl_direct_SVREG(TMC200Motors motor,int p,int i,int d){
	snprintf(cArgsStr,ARGSSTR_LEN,"M%d %d %d %d",motor,p,i,d);
	return tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,cArgsStr);
}

int tmc200ctrl_direct_GVREG(TMC200Motors motor){
	snprintf(cArgsStr,ARGSSTR_LEN,"M%d",motor);
	return tmc200ctrl_io_setCmdWithArgsStr(TMC200CMD_SENCO,cArgsStr);
}

void tmc200ctrl_direct_sendSVRepetitive(int val) {
	tmc200ctrl_io_sendSVRepetitive(val);
}

const char* tmc200ctrl_direct_getTMC200CommandKeyword(TMC200Commands cmd) {
	return tmc200ctrl_io_getTMC200CommandKeyword(cmd);
}


void tmc200ctrl_direct_setTMC200FeedbackProcessingFunction(void (*func)(TMC200Commands,const char*)) {
	tmc200ctrl_io_setTMC200FeedbackProcessingFunction(func);
}

void tmc200ctrl_direct_setErrorProcessingFunction(void (*func)(TMC200CtrlError)) {
	tmc200ctrl_io_setErrorProcessingFunction(func);
}

void tmc200ctrl_direct_shutdown() {
	tmc200ctrl_io_shutdown();
}

