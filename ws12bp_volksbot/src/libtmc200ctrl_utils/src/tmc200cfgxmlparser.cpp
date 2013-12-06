#include <tmc200cfgxmlparser.h>

#include <tinyxml.h>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#define TMC200CFG_XML_ROOT	("tmc200cfg")

namespace TMC200CTRL {
	
void TMC200CfgXmlParser::sendConfigurationFromFile(const char* xmlfile) const {
	TiXmlDocument	tXmlDoc(xmlfile);

	if(!tXmlDoc.LoadFile())
		throw TMC200CfgXmlParserException("TMC200CfgXmlParser: Error loading file: " + std::string(xmlfile));

	/*
	 * Throw an exception if the root element is not the expected.
	 */
	if(tXmlDoc.RootElement()->ValueStr()!=TMC200CFG_XML_ROOT)
		throw TMC200CfgXmlParserException("TMC200CfgXmlParser: No valid configuration file: " + std::string(xmlfile));	

	try {
		parsXmlNode(&tXmlDoc);
	} catch(UnknownTMC200CommandException& ex) {
		throw ex;
	}
}

void TMC200CfgXmlParser::parsXmlNode(TiXmlNode* node) const {
	std::string	valueStr;
	
	/*
	 * Only proceed if the current node is an
	 * element and is not the root element.
	 */
	if((node->Type() == TiXmlNode::TINYXML_ELEMENT) &&
	   (node->ValueStr() != TMC200CFG_XML_ROOT)) {
		
		#ifdef DEBUG
		std::cout << "[LIBTMC200CTRL_UTILS TMC200Configurator::parsXmlNode] Element: " << node->ValueStr() << std::endl;
		#endif
		//If the node is an Element, convert its value to upper case.
		std::transform(	node->ValueStr().begin(), node->ValueStr().end(),
						std::back_inserter(valueStr), toupper);

		int iTMC200Cmd = -1;
				
		/*
		 * Compares the extracted command with
		 * the elements of the commands keyword array.
		 *
		 * When a match occurs, set iTMC200Cmd equal i and exit the loop.
		 */
		for(int i=0;i<TMC200CTRL_CMD_COUNT;i++) {
			if(valueStr==TMC200CommandsKeywords[i]) {
				iTMC200Cmd = i;
				break;
			}
		}
		
		
		if(iTMC200Cmd>-1) {	
			const TiXmlElement* xmlElmt = node->ToElement();
		
			//Send the command.
			if(xmlElmt->GetText()!=NULL)
				pfTMC200Comm((TMC200Commands)iTMC200Cmd,xmlElmt->GetText());
			else
				pfTMC200Comm((TMC200Commands)iTMC200Cmd,"");
		} else {
			//Throw exception if the command was not found and its expected.
			if(bThrowUnknownTMC200CommandException)
				throw UnknownTMC200CommandException(node->ValueStr());
		}
	}
	
	//Traverse the tree further.
	for (TiXmlNode* child = node->FirstChild();
		 child != NULL; child = child->NextSibling()) {
	
		try {
			parsXmlNode(child);	
		} catch(UnknownTMC200CommandException& ex) {
			throw ex;
		}
	}
}

}
