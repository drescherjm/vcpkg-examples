#pragma once

#ifndef CSVINFO_H
#define CSVINFO_H

#include <string>

/////////////////////////////////////////////////////////////////////////////////////////

struct CSVInfo
{
public:
	enum CSVType
	{
		CSV_UNKNOWN,
		CSV_WBUS_BOTH_RADIOLIGIST_RECALL_DETAIL
	};

public:
	CSVInfo();
	bool setupCSVInfo(const std::string & strFileName);

public:
	std::string	strCSVFile;
	std::string strExportDate;
	std::string strMachineName;
	std::string strQueryName;
	CSVType		csvType;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CSVINFO_H
