#include <iostream>
#include "CLI/CLI.hpp"

int main(int argc, char **argv) {
    CLI::App app;

    // Add new options/flags here
	bool my_flag = true;
	app.add_flag("-f", my_flag, "Optional description");
	
    CLI11_PARSE(app, argc, argv);

	std::boolalpha(std::cout);
	std::cout << my_flag;

    return 0;
}