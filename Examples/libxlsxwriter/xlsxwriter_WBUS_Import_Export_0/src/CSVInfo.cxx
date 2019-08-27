#include "CSVInfo.h"
#include "stringHelpers.h"
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////

std::map<std::string, CSVInfo::CSVType> csvTypeMap{
	{ "BothRadiologistRecallDetail",CSVInfo::CSV_WBUS_BOTH_RADIOLIGIST_RECALL_DETAIL },
};

/////////////////////////////////////////////////////////////////////////////////////////

CSVInfo::CSVInfo() : csvType{CSV_UNKNOWN}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool CSVInfo::setupCSVInfo(const std::string & strFileName)
{
	strCSVFile = strFileName;

	auto vec = splitString(strFileName, ".");

	bool retVal = vec.size() > 3;
	if (retVal) {
		retVal = !vec[0].empty();
		if (retVal) {
			std::string strPart = trim(vec[0]);
			retVal = (!strPart.empty());
			if (retVal) {
				// Valid years are 1XXX to 2XXX
				retVal = ((strPart[0] == '1') || (strPart[0] == '2'));
				if (retVal) {
					strExportDate = strPart;
					strMachineName = trim(vec[1]);
					strQueryName = trim(vec[2]);
				}
			}
			
			// Lookup the type of CSV file.
			auto it = csvTypeMap.find(removeAllWhitespace(strQueryName));

			if ( it != csvTypeMap.end()) {
				csvType = it->second;
			}
		
		}

	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
