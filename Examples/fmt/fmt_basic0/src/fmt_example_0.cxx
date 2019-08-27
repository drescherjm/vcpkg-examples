#include <iostream>
#include <fmt/format.h>

int main(int argc, char *argv[])
{
	std::string s = fmt::format("I'd rather be {1} than {0}.", "right", "happy");

	std::cout << s << std::endl;
	
    return 0;
}