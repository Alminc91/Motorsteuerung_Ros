#ifndef _LIBTMC200CTRL_COMMON_H_
#define _LIBTMC200CTRL_COMMON_H_

/**
 * @defgroup libtmc200ctrl_io libtmc200ctrl_io 
 * @file libtmc200ctrl_io.h
 * @ingroup libtmc200ctrl_io 
 * @brief Contains the libtmc200ctrl_io API.
 */

#include <tmc200_commands.h>
#include <tmc200ctrl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		Initializes the library.
 *
 *				This must be called before other are function called.
 * 
 * @param port	Device file that represents the COM-port where a
 *				TMC200-Controller is connected.
 * 
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_init(const char* port);

/**
 * @brief		Sends a command to the controller.
 *
 * @param cmd 	The command to execute.
 *
 * @remark		There are no checks whether the number of arguments
 *				inside the string fits to the given command.
 * 
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_setCmd(TMC200Commands cmd);

/**
 * @brief		Sends a command to the controller.
 *
 * @param cmd 	The command to execute.
 * @param arg1 	Argument 1.
 *
 * @remark		There are no checks whether the number of arguments
 				inside the string fits to the given command.
 * 
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_setCmdWithOneArg(TMC200Commands cmd,int arg1);

/**
 * @brief		Sends a command to the controller.
 *
 * @param cmd 	The command to execute.
 * @param arg1 	Argument 1.
 * @param arg2 	Argument 2.
 *
 * @remark		There are no checks whether the number of arguments
 *				inside the string fits to the given command.
 * 
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_setCmdWithTwoArgs(TMC200Commands cmd,int arg1,int arg2);

/**
 * @brief		Sends a command to the controller.
 *
 * @param cmd 	The command to execute.
 * @param arg1 	Argument 1.
 * @param arg2 	Argument 2.
 * @param arg3 	Argument 3.
 *
 * @remark		There are no checks whether the number of arguments
 *				inside the string fits to the given command.
 * 
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_setCmdWithThreeArgs(TMC200Commands cmd,int arg1,int arg2,int arg3);

/**
 * @brief		Sends a command to the controller.
 *
 * @param cmd 			The command to execute.
 * @param cmdargsstr 	A string that contains the arguments. Separated by a blanks.
 *
 * @remark		There are no checks whether the number of arguments
 *				inside the string fits to the given command.
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_io_setCmdWithArgsStr(TMC200Commands cmd,const char* cmdargsstr);

/**
 * @brief Determines whether the current SV command should be sent repeatedly.
 *
 * Determines whether the current SV command should be
 * sent repeatedly, until a new one is given.
 *
 * @param val 1 - True, 0 - False,
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
void tmc200ctrl_io_sendSVRepetitive(int val);

/**
 * @brief Whether sendSVRepetitve is set.
 *
 * @return Whether sendSVRepetitve is set.
 */
int tmc200ctrl_io_isSendSVRepetitiveSet();

/**
 * @brief		This callback function is executed every time
 *				a feedback from the controller arrives.
 *
 * 				The function signatur must be: 'void func(TMC200Commands,const char*)'
 *
 * @param func 	The functions function pointer.
 *
 * @remark 		The function is executed in the library thread.
 * 				The string is only accessable inside the callback function scope.
 *				To access it outside, it must be copied into the local context.			
 */
void tmc200ctrl_io_setTMC200FeedbackProcessingFunction(void (*func)(TMC200Commands,const char*));

/**
 * @brief 		This callback function is executed after
 * 				an error in one of the threads occurred.
 *
 * 				Through this mechanism one can catch operational errors.
 * 				The function signatur must be: 'void func(OPERATIONAL_STATUS)'
 *
 * @param func	The functions function pointer.
 *
 * @remark 		The function is executed in the library thread.
 */
void tmc200ctrl_io_setErrorProcessingFunction(void (*func)(TMC200CtrlError));

/**
 * @brief 		Returns the TMC200 command keyword.
 *
 * @param cmd	The command as TMC200Commands enumeration.
 *
 * @return 		The TMC200 command keyword.
 */
const char* tmc200ctrl_io_getTMC200CommandKeyword(TMC200Commands cmd);

/**
 * @brief Stops  the calling thread until the TMC200 answer to the recent command is available.
 * 
 * @attention The string memory was allocated by 'malloc' on the heap and must be freed manually to avoid memory leaks.
 *
 * @return Pointer to the answer string. In case of an error an null pointer.
 */
char* tmc200ctrl_io_waitForTMC200Feedback();

/**
 * @brief	Shuts down the library and frees all resources. 
 */ 
void tmc200ctrl_io_shutdown();

#ifdef __cplusplus
}
#endif

#endif
