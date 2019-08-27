#include "stringHelpers.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>


/////////////////////////////////////////////////////////////////////////////////////////

std::string getCurrentDate()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	return ss.str();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> splitString(std::string str, std::string token) {
	std::vector<std::string>result;
	while (str.size()) {
		auto index = str.find(token);
		if (index != std::string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string& ltrim(std::string& str, const std::string& chars /*= "\t\n\v\f\r "*/)
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string& rtrim(std::string& str, const std::string& chars /*= "\t\n\v\f\r "*/)
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string& trim(std::string& str, const std::string& chars /*= "\t\n\v\f\r "*/)
{
	return ltrim(rtrim(str, chars), chars);
}

/////////////////////////////////////////////////////////////////////////////////////////

void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
	std::vector<std::string>   result;
	std::string                line;

	if (std::getline(str, line)) {

		std::stringstream          lineStream(line);
		std::string                cell;

		while (std::getline(lineStream, cell, ','))
		{
			// Remove the quotes from the first and last position.
			if (cell.front() == '"') {
				cell.erase(0, 1); // erase the first character
				cell.erase(cell.size() - 1); // erase the last character
				trim(cell);
			}

			result.push_back(cell);
		}
		// This checks for a trailing comma with no data after it.
		if (!lineStream && cell.empty())
		{
			// If there was a trailing comma then add an empty element.
			result.push_back("");
		}
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string removeAllWhitespace(const std::string & str)
{
	return std::regex_replace(str, std::regex("\\s+"), "");
}

/////////////////////////////////////////////////////////////////////////////////////////