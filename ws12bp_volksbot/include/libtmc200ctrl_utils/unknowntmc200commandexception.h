#ifndef _UNKNOWNTMC200COMMANDEREXCEPTION_H_
#define _UNKNOWNTMC200COMMANDEREXCEPTION_H_

/**
 * @file 	unknowntmc200commandexception.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains an exception to indicate that a given command is unknown.
 */

#include <string>
#include <exception>

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {

/**
 * @brief An exception to indicate that a given command is unknown.
 */
class UnknownTMC200CommandException : public std::exception {

	public:
	/**
 	 * @brief	Constructor
 	 *			
 	 * @param unknwncmd The unknown TMC200 command.
 	 */
	UnknownTMC200CommandException(const std::string& unknwncmd)
		: tWhat(unknwncmd) {}
	
	/**
 	 * @brief	Deonstructor
 	 */
	virtual ~UnknownTMC200CommandException() throw() {}

	/**
 	 * @brief	 Returns the unknown TMC200 command.
 	 *			
 	 * @return	 The unknown TMC200 command.
 	 */
	const char* what() const throw() {
		return tWhat.c_str();
	}	
	
	private:
	const std::string tWhat;	///<Message string.
};
}

#endif

