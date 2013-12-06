#ifndef TMC200CTRL_CONVERTERS_H_
#define TMC200CTRL_CONVERTERS_H_

/**
 * @defgroup libtmc200ctrl_utils libtmc200ctrl_utils
 *
 * @file 	converters.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains functions to convert the string received
 * 			from the TMC200 into their actual data types.
 *
 * @remark 	There are no checks made whether the memory block
 * 			contains compatible data for the specific conversion.
 */

#include <tmc200_operation_modes.h>
#include <vector>

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {

/**
 * @brief	Contains functions to convert the string received
 * 			from the TMC200 into their actual data types.
 */
namespace Converters {

/**
 * @brief		Parses the input string to a TMC200OperationModes enumeration.
 *
 * @param data	String to parse.
 *
 * @return		Operation Mode.
 */
TMC200OperationModes convertToTMC200OperationMode(const char* data);

/**
 * @brief Extract a number of integers out of an string.
 *
 * @param data The string to parse.
 * @param n Count of numbers to parse.
 *
 * @return An vector that holds the data.
 */
std::vector<int> convertToIntArray(const char* data, int n);

}
}
#endif
