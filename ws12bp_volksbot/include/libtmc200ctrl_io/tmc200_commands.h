#ifndef TMC200_COMMANDS_H_
#define TMC200_COMMANDS_H_
/**
 * @file tmc200_commands.h
 * @ingroup libtmc200ctrl_io
 * @brief Contains the TMC200Commands enumeration.
 */

#define TMC200CTRL_CMD_COUNT	14	///<Number of supported commands.

/**
 * @brief List of the tmc200 Commands.
 * The order must be the same as @see{TMC200Commands}.
 */
static const char	TMC200CommandsKeywords[][6]	= { {"GVERS"},{"SMODE"},
								 		   	 		{"GMODE"},{"SV"},{"SSEND"},
								 		     		{"SMZNM"},{"GMZNM"},
											 		{"SENCO"},{"GENCO"},
								 		 	 		{"SMDPT"},{"GMDPT"},{"SRODO"},
								 		 	 		{"SVREG"},{"GVREG"} };


/**
 * @brief The commands of the TCM200 controller.
 */
typedef enum {
	TMC200CMD_GVERS,	///<Get the TMC200 version string.
	TMC200CMD_SMODE,	///<Set the operation mode.
	TMC200CMD_GMODE,	///<Get the operation mode.
	TMC200CMD_SV,		///<Set the desired speed of all three motors.
	TMC200CMD_SSEND,	///<Set the verbosity of an SV feedback.
	TMC200CMD_SMZNM,	///<Set the timeout.
	TMC200CMD_GMZNM,	///<Get the timeout.
	TMC200CMD_SENCO,	///<Set encoder constants.
	TMC200CMD_GENCO,	///<Get encoder constants.
	TMC200CMD_SMDPT,	///<Set odometrie rotation angle constant.
	TMC200CMD_GMDPT,	///<Get odometrie rotation angle constant.
	TMC200CMD_SRODO,	///<Reset odometrie.
	TMC200CMD_SVREG,	///<Set speed regulation.
	TMC200CMD_GVREG		///<Get speed regulation.
} TMC200Commands;
#endif

