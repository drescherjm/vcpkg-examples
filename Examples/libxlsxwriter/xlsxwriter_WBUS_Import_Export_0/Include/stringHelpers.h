#pragma once

#ifndef STRINGHELPERS_H
#define STRINGHELPERS_H

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////

extern std::string	getCurrentDate();
extern std::vector<std::string> splitString(std::string str, std::string token);
extern std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

extern std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
extern std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
extern void replaceAll(std::string& source, const std::string& from, const std::string& to);

extern std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);


extern std::string removeAllWhitespace(const std::string & str);

/////////////////////////////////////////////////////////////////////////////////////////

#endif // STRINGHELPERS_H
