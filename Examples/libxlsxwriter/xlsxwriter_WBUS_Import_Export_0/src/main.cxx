/*
* Example of how to create defined names using libxlsxwriter. This method is
* used to define a user friendly name to represent a value, a single cell or
* a range of cells in a workbook.
*
* Copyright 2014-2017, John McNamara, jmcnamara@cpan.org
*
*/

// #include <xlsxwriter.h>
// #include <iostream>
// #include <string>
// #include <vector>
// #include <sstream>
// 
// #include <map>
// 
// #include <chrono>
// #include <iomanip>
// #include <filesystem>
#include "exportCSV.h"
#include <iostream>


/////////////////////////////////////////////////////////////////////////////////////////











/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <csvfile.csv> " << std::endl;
		return -1;
	}

	std::vector<std::string> csvFiles;
	csvFiles.push_back(argv[1]);

	exportCSV exportData(csvFiles);

	return exportData.process();

}

/////////////////////////////////////////////////////////////////////////////////////////

