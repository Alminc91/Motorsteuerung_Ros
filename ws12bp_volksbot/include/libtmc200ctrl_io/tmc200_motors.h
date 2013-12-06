#ifndef TMC200_MOTORS_H_
#define TMC200_MOTORS_H_
/**
 * @file tmc200_motors.h
 * @ingroup libtmc200ctrl_io
 * @brief Contains the TMC200Motors enumeration.
 */
 
 
/**
* Enumerates the available motors of the TMC200.
*/
typedef enum {
	TMC200_MOTOR_1 = 1,	///< First motor.
	TMC200_MOTOR_2 = 2,	///< Second motor.
	TMC200_MOTOR_3 = 3	///< Third motor.
} TMC200Motors;

#endif
