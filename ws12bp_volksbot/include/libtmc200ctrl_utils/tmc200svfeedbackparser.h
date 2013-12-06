#ifndef _TMC200SVFEEBACKPARSER_H_
#define _TMC200SVFEEBACKPARSER_H_

/**
 * @file 	tmc200svfeedbackparser.h
 * @ingroup libtmc200ctrl_utils 
 * @brief 	Contains a Component to parse the controller feedback to an SV command.
 */

#include <svvaluegroupkeynotfoundexception.h>

#include <stdint.h>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 * @brief	Contains the parts of the TMC200Ctrl that were written in C++.
 */
namespace TMC200CTRL {


/**
 * @brief Specefies the different values of an sv feedback.
 */
typedef enum {
	SVFBGROUP_VELOCITY,		///<Velocity
	SVFBGROUP_VOLTAGE,		///<Current voltage
	SVFBGROUP_DISTANCE,		///<Distance
	SVFBGROUP_PWMOUTPUT,	///<Output
	SVFBGROUP_MSGCOUNTER,	///<Message counter. (0-255)
	SVFBGROUP_DELTATIME,	///<Delta time.
	SVFBGROUP_ODOMETRY,		///<Odometry
	SVFBGROUP_TEMPERATURE,	///<Temperature of each Motor.
	SVFBGROUP_OVERTEMP,		///<Overtemperature status.
	SVFBGROUP_BATTERYVOLT,	///<Battery voltage.
	SVFBGROUP_AMPLERRBITS	///<Error Bits (First Byte).
} SVFBValueGroups;


/**
 * @brief List of SV command feedback value groups.
 * The order must be the same as \link SVFBValueGroups \endlink.
 */
static const char	SVFbValueGroupsStrs[][4] = { {"V"},{"C"},{"D"},{"O"},
												 {"x"},{"dT"},{"ODO"},{"TEM"},
											 	 {"SS"},{"UB"},{"BE"} };

/**
 * @brief	Class to parse the controller feedback to an SV command.
 *
 *			It allows to check whether a certain value exists and
 *			its conversion into its actual fixed point representation.
 */
class TMC200SVFeedbackParser {
	public:
	/**
	 * @brief	Parses an SV feedback string.
	 *
	 * @param str The SV feedback string.
	 */
	void parseSVFeedbackString(const string& str);

	/**
	 * @brief Whether a specific key exists.
	 *
	 * @param key The key.
	 *
	 * @return Whether a specific key exists.
	 */
	bool valuesExists(SVFBValueGroups key) const;
	
	/**
	 * @brief Returns the values to a key.
	 *
	 * @param key The key.
	 *
	 * @return Returns the values to a key.
	 *
	 * @exception SVValueGroupKeyNotFoundException
	 */
	const vector<int>& getValue(SVFBValueGroups key);
		
	/**
	 * @brief Returns true if there are values.
	 *
	 * @return True if there are values.
	 */	
	bool hasNext();
	
	/**
	 * @brief Returns the next values.
	 *
	 * @return The next values.
	 */	
	std::pair< string,vector<int> > getNext() const;
	
	
	private:
	/**
	 * @brief Map that holds all parsed value.
	 */
	map<string,vector<int> > tValueMap;
	
	/**
	 * @brief Iterator to traverse all values.
	 */
	map<string,vector<int> >::const_iterator tValueMapIter;
};
}
#endif
