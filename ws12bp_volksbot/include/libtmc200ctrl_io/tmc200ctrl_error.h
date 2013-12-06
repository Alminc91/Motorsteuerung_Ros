#ifndef TMC200CTRL_ERROR_H_
#define TMC200CTRL_ERROR_H_
/**
 * @file tmc200ctrl_error.h
 * @ingroup libtmc200ctrl_io
 * @brief Contains the TMC200CtrlError enumeration.
 */

/**
 * @brief Indicates errors that might occur during operation.
 */
typedef enum {
	TMC200CTRL_ERROR_NONE=0,				///<No Error, everything OK.
	TMC200CTRL_ERROR_RTO=-1,				///<Timeout during receiving data.
	TMC200CTRL_ERROR_WTO=-2, 				///<Timeout during sending data.
	TMC200CTRL_ERROR_NOTTY=-3,				///<File descriptor is not a TTY.
	TMC200CTRL_ERROR_INVFILEDESC=-4,		///<Invalid file descriptor.
	TMC200CTRL_ERROR_NO_SENDCMDTHREAD=-5,	//<The IO-Thread isn't working.
	TMC200CTRL_ERROR_INITTHREADS=-6,		///<Problems with starting threads.
	TMC200CTRL_ERROR_CMD_UNKNOWN=-7			///<Given command is unknown or not supported.
} TMC200CtrlError;

#endif
