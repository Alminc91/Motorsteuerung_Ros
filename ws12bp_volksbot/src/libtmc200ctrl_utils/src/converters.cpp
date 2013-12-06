#include <converters.h>

#include <stdlib.h>
#include <string.h>

namespace TMC200CTRL {
namespace Converters {

TMC200OperationModes convertToTMC200OperationMode(const char* data) {
	return (TMC200OperationModes)atoi(data+strlen(data)-2);
}

std::vector<int> convertToIntArray(const char* data, int n) {
	int					j=0,i=0;	
	char* 				cStrTok = strtok((char*)data," ");
	std::vector<int>	tRetVec;
	
	tRetVec.reserve(n);
	
	/*
	 * Splits an string by spaces and trys to
	 * parse these substrings as integers.
	 */	
	while(i<n) {
		if(cStrTok==NULL)
			break;
	
		j=(int)strtol(cStrTok,NULL,10);

		if(j!=0L) {
			tRetVec.push_back(j);
			i++;
		}

		cStrTok = strtok(NULL," ");
	}
	
	return tRetVec;
}

}
}
