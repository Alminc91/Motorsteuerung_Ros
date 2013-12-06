#include <tmc200svfeedbackparser.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <utility>
#include <cctype>

namespace TMC200CTRL {

using namespace std;

void TMC200SVFeedbackParser::parseSVFeedbackString(const string& str) {
    int 	pos = str.find("\t");
    int 	initialPos = 0;
    char*	strTok=NULL;
	string	subStr;
	string	key;
	vector<int> valVec;

	valVec.reserve(3);

	this->tValueMap.clear();

    // Decompose statement
    while( pos != string::npos ) {
    	subStr = str.substr( initialPos, pos - initialPos);
        
        /*
         * Extract the key.
         */
    	strTok = strtok(const_cast<char *>(subStr.c_str())," ");
    	key=strTok;

		/* 
	     * Extract the integer tupels.
	     */    	
    	while((strTok=strtok(NULL," "))!= NULL) {
			valVec.push_back((int)strtol(strTok,NULL,10));
		}

		/*
		 * Add both to the map.
		 */
		this->tValueMap.insert(	this->tValueMap.begin(),
								std::pair<string,vector<int> >(key,valVec));
		
		valVec.clear();		
	
		initialPos = pos + 1;
        pos = str.find("\t", initialPos );
    }
         
    this->tValueMapIter=this->tValueMap.begin();
}

bool TMC200SVFeedbackParser::valuesExists(SVFBValueGroups key) const {
	return (this->tValueMap.count(SVFbValueGroupsStrs[key])>0);
}

const vector<int>& TMC200SVFeedbackParser::getValue(SVFBValueGroups key) {
	//Throw an exception if there's no entry for the searched key.
	if(this->tValueMap.count(SVFbValueGroupsStrs[key])==0)
		throw SVValueGroupKeyNotFoundException(SVFbValueGroupsStrs[key]);

	return this->tValueMap[SVFbValueGroupsStrs[key]];
}

bool TMC200SVFeedbackParser::hasNext() {
	return (++this->tValueMapIter)!=this->tValueMap.end();
}

std::pair< string,vector<int> > TMC200SVFeedbackParser::getNext() const {
	return *(this->tValueMapIter);
}

}
