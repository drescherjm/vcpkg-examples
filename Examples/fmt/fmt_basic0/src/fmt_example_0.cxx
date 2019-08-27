#include <iostream>
#include <ctime>
#include <fmt/format.h>
#include <fmt/printf.h>

int main(int argc, char *argv[])
{
	std::string s = fmt::format("I'd rather be {1} than {0}.", "right", "happy");

	std::cout << s << std::endl;

	std::time_t t = std::time(nullptr);

	// The following line is commented out because it currently causes several compiler errors in Visual 
	// Studio 2019
	//fmt::print("The date is {:%Y-%m-%d}.", *std::localtime(&t));

	fmt::print("Hello, {}!\n", "world");  // Python-like format string syntax
	fmt::printf("Hello, %s!\n", "world"); // printf format string syntax

	try {
		fmt::format("The answer is {:d}", "forty-two");
	}
	catch (const fmt::format_error& er)
	{
		std::cout << "Caught exception: " << er.what() << std::endl;
	}
	
    return 0;
}