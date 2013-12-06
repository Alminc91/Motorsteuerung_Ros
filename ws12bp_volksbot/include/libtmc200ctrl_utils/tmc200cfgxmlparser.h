#ifndef _TMC200CONFIGURATOR_H_
#define _TMC200CONFIGURATOR_H_

/**
 * @file 	tmc200cfgxmlparser.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains a class which loads a configuration from an
 *			Xml-file and sends it to the controller.
 */
#include <tmc200cfgxmlparserexception.h>
#include <unknowntmc200commandexception.h>
#include <tmc200_commands.h>

class TiXmlNode;

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {

/**
 * @brief 	Loads an Xml source, interprets its content as TMC200 commands and
 * 			sends them to the to the controller.
 *
 * The xml file must be of the following form:
 * @code{.xml}
 * <tmc200ctrl>
 * <cmd>arg1 arg2 argn</cmd>
 * ...
 * <tmc200ctrl>
 * @endcode
 */
class TMC200CfgXmlParser {
	public:
	/**
	 * @brief 					Constructor of this class.
	 *
	 * @param tmc200commfunc	Pointer to function which sends the configuration
	 *							commands to the controller.
	 *
	 * @param throwonunknowntmc200command Whether an \link UnknownTMC200CommandException \endlink should be thrown.
	 */
	TMC200CfgXmlParser(	void (*tmc200commfunc)(TMC200Commands,const char*),
						bool throwonunknowntmc200command = false)
		: pfTMC200Comm(tmc200commfunc),
		  bThrowUnknownTMC200CommandException(throwonunknowntmc200command) {
	}
		
	/**
	 * @brief			Loads an Xml file and sends its content
	 					to the controller via the provided function.
	 *
	 * @param xmlfile	Path to an xmlfile
	 *
	 * @exception TMC200CfgXmlParserException
	 * @exception UnknownTMC200CommandException
	 *
	 * @returns Whether all commands have been sent successfully.
	 */
	 void sendConfigurationFromFile(const char* xmlfile) const;
	 
	 private:
	 /**
	  * @brief Parse an xml node.
	  *
	  * @param node Node to parse.
	  */
	 void parsXmlNode(TiXmlNode* node) const;
	 
	 /**
	  * @brief Pointer to the function that actually sends the commands to the TMC200.
	  */
	 void (*pfTMC200Comm)(TMC200Commands,const char*);
	 
	/**
	  * @brief Whether an \link UnknownTMC200CommandException \endlink should be thrown.
	  */
	 const bool bThrowUnknownTMC200CommandException;
};
}

#endif
