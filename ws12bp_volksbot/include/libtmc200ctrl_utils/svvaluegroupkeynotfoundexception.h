#ifndef _SVVALUEGROUPKEYNOTFOUNDEXCEPTION_H_
#define _SVVALUEGROUPKEYNOTFOUNDEXCEPTION_H_

/**
 * @file 	svvaluegroupkeynotfoundexception.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains an exception to indicate that the \link TMC200CTRL::TMC200SVFeedbackParser \endlink hasen't parsed the requested value.
 */

#include <string>
#include <exception>

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {

/**
 * @brief An exception to indicate that the \link TMC200SVFeedbackParser \endlink hasen't parsed the requested value.
 */
class SVValueGroupKeyNotFoundException : public std::exception {

	public:
	/**
 	 * @brief	Constructor
 	 *			
 	 * @param msg The key that wasn't found.
 	 */
	SVValueGroupKeyNotFoundException(const std::string& msg)
		: tWhat(msg) {}
	
	/**
 	 * @brief	Deonstructor
 	 */
	virtual ~SVValueGroupKeyNotFoundException() throw() {}

	/**
 	 * @brief	 Returns the key that wasn't found.
 	 *			
 	 * @return	 The key that wasn't found.
 	 */
	const char* what() const throw() {
		return tWhat.c_str();
	}	
	
	private:
	const std::string tWhat;	///<Message string.
};
}

#endif

