#ifndef TMC200CTRL_H_
#define TMC200CTRL_H_

/**
 * @defgroup libtmc200ctrl_direct libtmc200ctrl_direct
 * @file libtmc200ctrl_direct.h
 * @ingroup libtmc200ctrl_direct
 * @brief Contains the libTMC200Ctrl-API.
 */

#include <tmc200ctrl_error.h>
#include <tmc200_commands.h>
#include <tmc200_motors.h>
#include <tmc200_operation_modes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @copydoc tmc200ctrl_io_init
 */
int tmc200ctrl_direct_init(const char* port);

/**
 * @copydoc tmc200ctrl_io_setCmdWithArgsStr
 */
void tmc200ctrl_direct_setCmdWithArgsStr(TMC200Commands cmd,const char* cmdargsstr);

/**
 * @brief Requests the version of the controller software.
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_GVERS();

/**
 * @brief Sets the operation mode of the controller.
 * @param mode Operation mode.
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SMODE(TMC200OperationModes mode);

/**
 * @brief Requests the operation mode the controller is running.
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink..
 */
int tmc200ctrl_direct_GMODE();

/**
 * @brief Sets the desired speed of all three motors.
 * @param m1 Motor 1
 * @param m2 Motor 2
 * @param m3 Motor 3
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SV(int m1, int m2, int m3);


/**
 * @brief Sets the verbosity of the TMC200 to a SV comand.
 * @param mode Verbosity level.
 *
 * @return 		Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SSEND(int mode);

/**
 * @brief Set timeout to cutoff all motors.
 *
 * @param to Timeout = to*10ms.
 *
 * @return 	Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SMZNM(int to);

/**
 * @brief Requests the timeout to cutoff all motors.
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_GMZNM();

/**
 * @brief Sets encoder constands.
 *
 * @param motor The motor.
 * @param encres Double encoder resolution.
 * @param drive Maximum engine rotational speed (rpm).
 * @param delta Change in distance per encoder step (Micrometer/0.5) / step.
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SENCO(TMC200Motors motor,int encres, int drive, int delta);

/**
 * @brief Requests the encoder constands
 *
 * @param motor The motor.
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_GENCO(TMC200Motors motor);

/**
 * @brief Sets microdegree per encoder tick.
 *
 * @param odomRotAngle Microdegree per encoder tick.
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SMDPT(int odomRotAngle);

/**
 * @brief Requests the encoder constands
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink..
 */
int tmc200ctrl_direct_SRODO();

/**
 * @brief Sets velocity regulator constands.
 *
 * @param motor The motor.
 * @param p Proportional component
 * @param i Integral component
 * @param d Differential component
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_SVREG(TMC200Motors motor,int p,int i,int d);

/**
 * @brief Requests velocity regulator constands.
 *
 * @param motor The motor.
 *
 * @return Zero if successful. Else see \link TMC200CtrlError \endlink.
 */
int tmc200ctrl_direct_GVREG(TMC200Motors motor);


/**
 * @copydoc tmc200ctrl_io_sendSVRepetitive
 */
void tmc200ctrl_direct_sendSVRepetitive(int val);

/**
 * @copydoc tmc200ctrl_io_setErrorProcessingFunction
 */
void tmc200ctrl_direct_setErrorProcessingFunction(void (*func)(TMC200CtrlError));

/**
 * @copydoc tmc200ctrl_io_setTMC200FeedbackProcessingFunction
 */
void tmc200ctrl_direct_setTMC200FeedbackProcessingFunction(void (*func)(TMC200Commands,const char*));

/**
 * @copydoc tmc200ctrl_io_getTMC200CommandKeyword
 */
const char* tmc200ctrl_direct_getTMC200CommandKeyword(TMC200Commands cmd);

/**
 * @copydoc tmc200ctrl_io_shutdown
 */
void tmc200ctrl_direct_shutdown();

#ifdef __cplusplus
}
#endif

#endif /* TMC200CTRL_H_ */
