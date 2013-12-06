#ifndef _TMC200CFGXMLPARSEREXCEPTION_H_
#define _TMC200CFGXMLPARSEREXCEPTION_H_

/**
 * @file 	tmc200cfgxmlparserexception.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains an exception to indicate errors during parsing of an xml file.
 */

#include <string>
#include <exception>

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {

/**
 * @brief An exception to indicate errors during parsing of the xml file.
 */
class TMC200CfgXmlParserException : public std::exception {

	public:
	/**
 	 * @brief	Constructor
 	 *			
 	 * @param msg Reason why the exception has been thrown.
 	 */
	TMC200CfgXmlParserException(const std::string& msg)
		: tWhat(msg) {}
	
	/**
 	 * @brief	Deonstructor
 	 */
	virtual ~TMC200CfgXmlParserException() throw() {}

	/**
 	 * @brief	 Returns why the exception has been thrown.
 	 *			
 	 * @return	 Reason why the exception has been thrown.
 	 */
	const char* what() const throw() {
		return tWhat.c_str();
	}	
	
	private:
	const std::string tWhat;	///<Message string.
};
}

#endif

