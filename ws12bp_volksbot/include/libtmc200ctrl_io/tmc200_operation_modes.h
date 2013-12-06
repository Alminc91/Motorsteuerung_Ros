#ifndef TMC200CTRL_OPERATION_MODES_H_
#define TMC200CTRL_OPERATION_MODES_H_

/**
 * @file tmc200_operation_modes.h
 * @ingroup libtmc200ctrl_io
 * @brief Contains the TMC200OperationModes enumeration.
 */

/**
 * @brief The controller operation modes.
 */
typedef enum {
	TMC200_OPERATION_MODE_DIRECT_OUTPUT, ///< Direct input
	TMC200_OPERATION_MODE_PID_VELOCITY_CONTROL, ///< PID-Regualtion
	TMC200_OPERATION_MODE_PID_VELOCITY_CONTROL_WITH_CUT_OUT ///< PID-Regualtion with cut out
} TMC200OperationModes;

#endif
