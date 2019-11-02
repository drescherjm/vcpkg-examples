#include <iostream>
#include "CLI/CLI.hpp"

int main(int argc, char **argv) {
    CLI::App app;

    // Add new options/flags here
	bool my_flag = true;
	app.add_flag("--flag,!--no-flag", my_flag, "Optional description");

	std::vector<std::string> imageFiles;

	app.add_option("--image", imageFiles, "Image Files")->check(CLI::ExistingFile);
	
    CLI11_PARSE(app, argc, argv);

	std::boolalpha(std::cout);
	std::cout << my_flag;

	std::cout << app.config_to_str(true, true) << std::endl;

    return 0;
}