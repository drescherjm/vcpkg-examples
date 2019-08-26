#include "exportCSV.h"
#include <map>
#include <regex>
#include <xlsxwriter.h>
#include "stringHelpers.h"
#include "CSVInfo.h"
#include "ColumnFormat.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

/////////////////////////////////////////////////////////////////////////////////////////

namespace fs = std::filesystem;

/////////////////////////////////////////////////////////////////////////////////////////

std::map<std::string, ColumnFormat> columnSizeMap {
	{ "SubjectID",{ ColumnFormat::DT_STRING,12.0f } },
	{ "ProjectID",{ ColumnFormat::DT_INT, 6.5f } },
	{ "PatientID",{ ColumnFormat::DT_INT, 6.5f } },
	{ "ExamID",{ ColumnFormat::DT_INT,5.0f } },
	{ "ExamDate",{ ColumnFormat::DT_DATE,11.0f } },
	{ "ExamTypeID",{ ColumnFormat::DT_INT,5.0f } },
	{ "ExamYear",{ ColumnFormat::DT_INT,5.0f } },
	{ "R1ProjectModeID",{ ColumnFormat::DT_INT,6.5f } },
	{ "R2ProjectModeID",{ ColumnFormat::DT_INT,6.5f } },
	{ "R1ReadingTypeID",{ ColumnFormat::DT_INT,7.5f } },
	{ "R2ReadingTypeID",{ ColumnFormat::DT_INT,7.5f } },
	{ "R1ReaderID",{ ColumnFormat::DT_INT,6.5f } },
	{ "R2ReaderID",{ ColumnFormat::DT_INT,6.5f } },
	{ "R1ReaderName",{ ColumnFormat::DT_STRING,16.0f } },
	{ "R2ReaderName",{ ColumnFormat::DT_STRING,16.0f } },
	{ "R1ReaderType",{ ColumnFormat::DT_STRING,10.0f } },
	{ "R2ReaderType",{ ColumnFormat::DT_STRING,10.0f } },
	{ "R1RightWBUSBIRADSRating",{ ColumnFormat::DT_STRING,12.0f } },
	{ "R2RightWBUSBIRADSRating",{ ColumnFormat::DT_STRING,12.0f } },
	{ "R1LeftWBUSBIRADSRating",{ ColumnFormat::DT_STRING,12.0f } },
	{ "R2LeftWBUSBIRADSRating",{ ColumnFormat::DT_STRING,12.0f } },
	{ "R1RightWBUSMaximumFindingBIRADSRating",{ ColumnFormat::DT_STRING,9.0f } },
	{ "R2RightWBUSMaximumFindingBIRADSRating",{ ColumnFormat::DT_STRING,9.0f } },
	{ "R1LeftWBUSMaximumFindingBIRADSRating",{ ColumnFormat::DT_STRING,9.0f } },
	{ "R2LeftWBUSMaximumFindingBIRADSRating",{ ColumnFormat::DT_STRING,9.0f } },
	{ "R1FinalAssessmentRightRecommendedProcedure",{ ColumnFormat::DT_STRING,14.0f } },
	{ "R2FinalAssessmentRightRecommendedProcedure",{ ColumnFormat::DT_STRING,14.0f } },
	{ "R1FinalAssessmentLeftRecommendedProcedure",{ ColumnFormat::DT_STRING,14.0f } },
	{ "R2FinalAssessmentLeftRecommendedProcedure",{ ColumnFormat::DT_STRING,14.0f } },
	{ "R1FinalAssessmentRightMammoViewsNeeded",{ ColumnFormat::DT_INT,14.0f } },
	{ "R2FinalAssessmentRightMammoViewsNeeded",{ ColumnFormat::DT_INT,14.0f } },
	{ "R1FinalAssessmentLeftMammoViewsNeeded",{ ColumnFormat::DT_INT,14.0f } },
	{ "R2FinalAssessmentLeftMammoViewsNeeded",{ ColumnFormat::DT_INT,14.0f } },

	{ "R1Patient2DTomoRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2Patient2DTomoRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R1PatientWBUSRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2PatientWBUSRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },

	{ "R1Patient2DTomoTechnicalRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2Patient2DTomoTechnicalRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R1PatientWBUSTechnicalRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2PatientWBUSTechnicalRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },

	{ "R1PatientBothModalityRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2PatientBothModalityRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },

	{ "R1PatientEitherModalityRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2PatientEitherModalityRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },


	{ "R1Patient2DTomoExclusiveRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2Patient2DTomoExclusiveRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R1PatientWBUSExclusiveRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
	{ "R2PatientWBUSExclusiveRecallAfterIntegration",{ ColumnFormat::DT_INT,10.0f } },
};

/////////////////////////////////////////////////////////////////////////////////////////

struct exportCSV::exPrivate
{
public:
	exPrivate(const std::vector<std::string> & vecFiles);

public:
	int				processFile(std::string strCSVFile);

	float			getColumnWidth(std::string strColumnName);
	std::string		getCleanColumnName(const std::string & strColumnName);
	ColumnFormat::DataType	getColumnDataType(std::string strColumnName);
	ColumnFormat::DataType	getColumnDataType(const std::vector<std::string> & colNames, int nCol);
	bool			exportHeader(std::ifstream & file, lxw_worksheet* worksheet, std::vector<std::string> & vecColNames);
	bool			generateNamedRanges(lxw_worksheet* worksheet, const std::vector<std::string> & vecColNames, int row, const std::string & strSheetName, 
		const CSVInfo & info, std::vector<std::string> & vecNamedRanges );
	bool			generateCalculationSheet(lxw_worksheet* worksheet, const std::vector<std::string> & vecColNames, int row, 
		std::string strSheetName, const CSVInfo & info, 
		std::vector<std::string> & vecNamedRanges);
	lxw_error		exportCSV(std::ifstream & file, const CSVInfo & info);

	std::string		getOutputFileName(const CSVInfo & info);
	std::string		getRawDataSheetName(const CSVInfo & info);
	std::string		getKeyWorksheetName(const CSVInfo & info);
	std::string		getInfoWorksheetName(const CSVInfo & info);
	std::string		getCalcWorksheetName(const CSVInfo & info);

	std::string		getColunmNameFromColIndex(uint16_t nCol, bool bAbsolute);

	bool			addSubjectExamKeyColumn(lxw_worksheet* pWorkSheet, std::string strWorksheetName, uint16_t nCol, 
		const std::pair<uint16_t, uint16_t> rowRange, const std::vector<std::string> & vecColNames, 
		std::string colName, uint16_t nColOffset /*=0*/);

	int				lookupColumnIndex(std::string strCol, const std::vector<std::string> & vecColNames, uint16_t nColOffset = 0);
	bool			addKeyColumn(lxw_worksheet* pWorkSheet, uint16_t nCol, const std::vector<std::string> & vecColNames, 
		std::string colName, 
		const std::vector<std::string> & vecKeyCols, 
		uint16_t nColOffset /*=0*/);
	bool			addKeyColumn(lxw_worksheet* pWorkSheet, std::string strWorksheetName, uint16_t nCol, 
		const std::pair<uint16_t, uint16_t> rowRange, const std::vector<std::string> & vecColNames, 
		std::string colName, const std::vector<std::string> & vecKeyCols, uint16_t nColOffset /*=0*/);
public:
	lxw_workbook*				m_pWorkBook;
	lxw_format*					m_pWrapTextFormat;
	std::vector<std::string>	m_vecFiles;
};

/////////////////////////////////////////////////////////////////////////////////////////

exportCSV::exPrivate::exPrivate(const std::vector<std::string> & vecFiles) : m_pWorkBook{}, 
m_vecFiles{vecFiles},m_pWrapTextFormat{}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getCleanColumnName(const std::string & strColumnName)
{
	// Remove all whitespace.
	return std::regex_replace(strColumnName, std::regex("\\s+"), "");
}

/////////////////////////////////////////////////////////////////////////////////////////

float exportCSV::exPrivate::getColumnWidth(std::string strColumnName)
{
	float retVal{ -1.0f };

	auto it = columnSizeMap.find(getCleanColumnName(strColumnName));

	if (it != columnSizeMap.end()) {
		retVal = it->second.m_nWidth;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

ColumnFormat::DataType exportCSV::exPrivate::getColumnDataType(std::string strColumnName)
{
	ColumnFormat::DataType retVal{ ColumnFormat::DT_NONE };

	auto it = columnSizeMap.find(getCleanColumnName(strColumnName));

	if (it != columnSizeMap.end()) {
		retVal = it->second.m_dt;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

ColumnFormat::DataType exportCSV::exPrivate::getColumnDataType(const std::vector<std::string> & colNames, int nCol)
{
	ColumnFormat::DataType retVal{ ColumnFormat::DT_NONE };

	if ((nCol >= 0) && (nCol < colNames.size())) {
		retVal = getColumnDataType(colNames[nCol]);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////////////

bool exportCSV::exPrivate::exportHeader(std::ifstream & file, lxw_worksheet* worksheet, std::vector<std::string> & vecColNames)
{
	int row = 0;
	std::vector<std::string> vecLine = getNextLineAndSplitIntoTokens(file);

	bool retVal = !vecLine.empty();

	if (retVal) {
		int col = 0;
		for (auto strColumn : vecLine) {
			worksheet_write_string(worksheet, row, col, strColumn.c_str(), NULL);

			vecColNames.push_back(strColumn);

			auto colWidth = getColumnWidth(strColumn);

			if (colWidth > 0.0) {
				worksheet_set_column(worksheet, col, col, colWidth, NULL);
			}
			else {
				// if it does not have a width it is not in the map.
				if ((strColumn.find("Recall") != std::string::npos) ||
					(strColumn.find("Disagree") != std::string::npos)) {
					columnSizeMap[getCleanColumnName(strColumn)] = ColumnFormat(ColumnFormat::DT_INT);
				}
			}

			col++;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool exportCSV::exPrivate::generateNamedRanges(lxw_worksheet* worksheet, const std::vector<std::string> & vecColNames, int row, const std::string & strSheetName, const CSVInfo & info,
	std::vector<std::string> & vecNamedRanges )
{
	bool retVal = (worksheet != nullptr);
	if (retVal) {

		uint16_t nCol{};

		std::string infoWSName = getInfoWorksheetName(info);

		lxw_worksheet *worksheetInfo = workbook_add_worksheet(m_pWorkBook, infoWSName.c_str());

		retVal = (worksheetInfo != nullptr);

		if (retVal) {

			lxw_error error = worksheet_set_row(worksheetInfo, 0, LXW_DEF_ROW_HEIGHT, m_pWrapTextFormat);

			retVal = (error == LXW_NO_ERROR);

			if (retVal) {

			for (const auto& strColName : vecColNames) {
				std::string strCleanName = getCleanColumnName(strColName);

				worksheet_write_string(worksheetInfo, 0, nCol, strColName.c_str(), NULL);

				auto colWidth = getColumnWidth(strColName);

				if (colWidth > 0.0) {
					worksheet_set_column(worksheetInfo, nCol, nCol, colWidth, NULL);
				}

				if (!strCleanName.empty()) {

					strCleanName.insert(0, "\'!rf");
					strCleanName.insert(0, strSheetName);
					strCleanName.insert(0, "\'");

					std::string strCol = getColunmNameFromColIndex(nCol, true);

					std::stringstream strRange;
					strRange << "=\'" << strSheetName << "\'!" << strCol << "$2:" << strCol << "$" << row;

					workbook_define_name(m_pWorkBook, strCleanName.c_str(), strRange.str().c_str());

					worksheet_write_string(worksheetInfo, 1, nCol, strCleanName.c_str(), NULL);
					worksheet_write_string(worksheetInfo, 2, nCol, strRange.str().c_str(), NULL);

					strRange.str(std::string());
					
					strRange << "\'" << strSheetName << "\'!" << strCol << "$2:" << strCol << "$" << row;

					worksheet_write_string(worksheetInfo, 3, nCol, strRange.str().c_str(), NULL);

					nCol++;
				}
			}
			}
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

lxw_error exportCSV::exPrivate::exportCSV(std::ifstream & file,const CSVInfo & info)
{
	lxw_error error{ LXW_NO_ERROR };
	
	if (!m_pWorkBook) {
		m_pWorkBook = workbook_new(getOutputFileName(info).c_str());
	}

	if (m_pWorkBook) {

		std::string strRawDataSheetName = getRawDataSheetName(info);

		lxw_worksheet *worksheet = workbook_add_worksheet(m_pWorkBook, strRawDataSheetName.c_str());

		if (worksheet) {
			std::vector<std::string> vecColNames;
			if (exportHeader(file, worksheet, vecColNames)) {
				if (m_pWrapTextFormat == nullptr) {
					m_pWrapTextFormat = workbook_add_format(m_pWorkBook);
					format_set_text_wrap(m_pWrapTextFormat);
				}
	
				error = worksheet_set_row(worksheet, 0, LXW_DEF_ROW_HEIGHT, m_pWrapTextFormat);

				int row = 1;
				std::vector<std::string> vecLine = getNextLineAndSplitIntoTokens(file);
				while (!vecLine.empty()) {
					int col = 0;

					for (auto strValue : vecLine) {

						if (!strValue.empty()) {

							ColumnFormat::DataType dt = getColumnDataType(vecColNames, col);

							switch (dt) {
							case ColumnFormat::DT_DATE:
							case ColumnFormat::DT_STRING:
							case ColumnFormat::DT_NONE:
								worksheet_write_string(worksheet, row, col, strValue.c_str(), NULL);
								break;
							case ColumnFormat::DT_INT:
								try {
									int nVal = std::stoi(strValue);
									worksheet_write_number(worksheet, row, col, nVal, NULL);
								}
								catch (const std::exception &exc) {
									worksheet_write_string(worksheet, row, col, strValue.c_str(), NULL);
									std::cerr << "ERROR parsing line " << row << " column " << col << exc.what();
								}
								break;
							case ColumnFormat::DT_DOUBLE:
								try {
									double nVal = std::stod(strValue);
									worksheet_write_number(worksheet, row, col, nVal, NULL);
								}
								catch (const std::exception &exc) {
									worksheet_write_string(worksheet, row, col, strValue.c_str(), NULL);
									std::cerr << "ERROR parsing line " << row << " column " << col << exc.what();
								}
								break;
							}
						}
						col++;
					}

					row++;
					vecLine = getNextLineAndSplitIntoTokens(file);

				}

				std::vector<std::string> vecNamedRanges;
				generateNamedRanges(worksheet, vecColNames, row, strRawDataSheetName,info,vecNamedRanges);

				generateCalculationSheet(worksheet, vecColNames, row, strRawDataSheetName, info, vecNamedRanges);

			}

			
		}
		else {
			std::cerr << "ERROR: Could not create the worksheet" << std::endl;
		}
	}

	return error;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getOutputFileName(const CSVInfo & info)
{
	fs::path fileName{ info.strCSVFile };

	fileName.replace_extension(".xlsx");

	return fileName.string();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getRawDataSheetName(const CSVInfo & info)
{
	std::string retVal{ "RawData" };
	
	std::string strRaw = getCleanColumnName(info.strQueryName);

	if (strRaw.size() > 24) {
		strRaw.resize(24);
	}

	if (!strRaw.empty()) {
		strRaw.append("-raw");

		retVal = strRaw;
	}
	
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getKeyWorksheetName(const CSVInfo & info)
{
	std::string retVal{ "Verification" };

	std::string strRaw = getCleanColumnName(info.strQueryName);

	if (strRaw.size() > 24) {
		strRaw.resize(24);
	}

	if (!strRaw.empty()) {
		strRaw.append("-test");

		retVal = strRaw;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getInfoWorksheetName(const CSVInfo & info)
{
	std::string retVal{ "Info" };

	std::string strRaw = getCleanColumnName(info.strQueryName);

	if (strRaw.size() > 24) {
		strRaw.resize(24);
	}

	if (!strRaw.empty()) {
		strRaw.append("-info");

		retVal = strRaw;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getCalcWorksheetName(const CSVInfo & info)
{
	std::string retVal{ "Calculation" };

	std::string strRaw = getCleanColumnName(info.strQueryName);

	if (strRaw.size() > 24) {
		strRaw.resize(24);
	}

	if (!strRaw.empty()) {
		strRaw.append("-calc");

		retVal = strRaw;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

int exportCSV::exPrivate::processFile(std::string strCSVFile)
{
	int retVal{};
	
	std::ifstream	csvFile(strCSVFile);

	if (csvFile) {
		
		CSVInfo info;
		if ( info.setupCSVInfo(strCSVFile)) {
			retVal = exportCSV(csvFile,info);
		}
		else {
			retVal = -1;
		}
	}
	else {
		std::cerr << "ERROR: Could not open " << strCSVFile << std::endl;
		retVal = -1;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool exportCSV::exPrivate::generateCalculationSheet(lxw_worksheet* worksheet, const std::vector<std::string> & vecColNames, 
	int row, std::string strSheetName, 
	const CSVInfo & info, std::vector<std::string> & vecNamedRanges)
{
	bool retVal = (worksheet != nullptr);
	if (retVal) {

		uint16_t nCol{};

		uint16_t nColOffset = 8;

		std::string infoWSName = getCalcWorksheetName(info);

		lxw_worksheet *worksheetCalc = workbook_add_worksheet(m_pWorkBook, infoWSName.c_str());

		retVal = (worksheetCalc != nullptr);

		if (retVal) {

			lxw_error error = worksheet_set_row(worksheetCalc, 0, LXW_DEF_ROW_HEIGHT, m_pWrapTextFormat);

			retVal = (error == LXW_NO_ERROR);

			if (retVal) {

				strSheetName.insert(0, "\'").append("\'!");

				nCol = nColOffset;
				uint16_t nInputCol{};			

				for (const auto& strColName : vecColNames) {
					std::string strCleanName = getCleanColumnName(strColName);

					worksheet_write_string(worksheetCalc, 0, nCol, strColName.c_str(), NULL);
					
					auto colWidth = getColumnWidth(strColName);

					if (colWidth > 0.0) {
						worksheet_set_column(worksheetCalc, nCol, nCol, colWidth, NULL);
					}
					
					std::string strColReference = getColunmNameFromColIndex(nInputCol, true);
					strColReference.insert(0, strSheetName);
				
					for (int nRow = 1; nRow < row; ++nRow) {

						std::stringstream streamFormula;

						streamFormula << "=" << "IF( " << strColReference << "$" << nRow+1 << " <> \"\" , " << strColReference << "$" << nRow + 1 << ", \"\" )";

						error = worksheet_write_formula(worksheetCalc, nRow, nCol, streamFormula.str().c_str(), NULL);
					}
					nCol++;
					nInputCol++;
				}


				addSubjectExamKeyColumn(worksheetCalc, infoWSName, 0, std::make_pair<>(1, row), vecColNames, "ExamKey", nColOffset);
			}
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string exportCSV::exPrivate::getColunmNameFromColIndex(uint16_t nCol, bool bAbsolute)
{
	char colName[20]{};
	lxw_col_to_name(colName, nCol, bAbsolute);

	return std::string(colName);

}


/////////////////////////////////////////////////////////////////////////////////////////

bool exportCSV::exPrivate::addKeyColumn(lxw_worksheet* pWorkSheet, std::string strWorksheetName, uint16_t nCol,
	const std::pair<uint16_t, uint16_t> rowRange,
	const std::vector<std::string> & vecColNames,
	std::string colName, const std::vector<std::string> & vecKeyCols, uint16_t nColOffset /*=0*/)
{
	bool retVal{ !vecKeyCols.empty() };

	if (retVal) {

		worksheet_write_string(pWorkSheet, 0, nCol, colName.c_str(), NULL);

		for (uint16_t nRow = rowRange.first; nRow < rowRange.second; ++nRow) {

			int nIndex = lookupColumnIndex(vecKeyCols[0], vecColNames, nColOffset);
			retVal = (nIndex >= 0);

			if (retVal) {

				//
				std::stringstream streamFormula;
				streamFormula << "=CONCATENATE( " << getColunmNameFromColIndex(nIndex, true) << nRow+1;

				for (size_t nPos = 1; nPos < vecKeyCols.size(); ++nPos) {
					streamFormula << ",\"_\",";

					nIndex = lookupColumnIndex(vecKeyCols[nPos], vecColNames, nColOffset);
					retVal = (nIndex >= 0);
					if (retVal) {
						streamFormula << getColunmNameFromColIndex(nIndex, true) << nRow+1;
					}
					else {
						break;
					}
				}

				streamFormula << " )";

				std::cout << streamFormula.str() << std::endl;

				worksheet_write_formula(pWorkSheet, nRow, nCol, streamFormula.str().c_str(), NULL);
			}

		}



	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool exportCSV::exPrivate::addSubjectExamKeyColumn(lxw_worksheet* pWorkSheet, std::string strWorksheetName, uint16_t nCol, const std::pair<uint16_t,uint16_t> rowRange, 
	const std::vector<std::string> & vecColNames, 
	std::string colName, uint16_t nColOffset /*=0*/)
{
	std::vector<std::string> vecKey{ "SubjectID", "ExamID" };

	return addKeyColumn(pWorkSheet,strWorksheetName, nCol, rowRange, vecColNames, colName, vecKey, nColOffset);
}

/////////////////////////////////////////////////////////////////////////////////////////

int exportCSV::exPrivate::lookupColumnIndex(std::string strCol, const std::vector<std::string> & vecColNames, uint16_t nColOffset /*= 0*/)
{
	int retVal{ -1 };

	uint16_t pos = nColOffset;
	for (const auto& strName : vecColNames) {
		if (removeAllWhitespace(strName) == strCol) {
			retVal = pos;
			break;
		}
		pos++;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

exportCSV::exportCSV(const std::vector<std::string> & vecFiles) : m_pPrivate{std::make_unique<exPrivate>(vecFiles)}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

exportCSV::~exportCSV()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

int exportCSV::process()
{
	int retVal{};
	for (auto strCSV : m_pPrivate->m_vecFiles) {
		retVal = m_pPrivate->processFile(strCSV);

		if (retVal != 0) {
			break;
		}
	}

	if (m_pPrivate->m_pWorkBook) {

		lxw_error error = workbook_close(m_pPrivate->m_pWorkBook);

		if (error) {

			printf("Error in workbook_close().\n"
				"Error %d = %s\n", retVal, lxw_strerror(error));

			retVal = -1;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

