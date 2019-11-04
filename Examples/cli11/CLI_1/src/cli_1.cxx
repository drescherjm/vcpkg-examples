#include <iostream>
#include "CLI/CLI.hpp"

int main(int argc, char **argv) {
    CLI::App app;

    // Add new options/flags here
	bool my_flag = true;
	app.add_flag("--flag,!--no-flag", my_flag, "Optional description");

	std::vector<std::string> imageFiles, smallTemplateFiles, largeTemplateFiles;
	std::vector<uint16_t> vecSmallTemplateSize{ 50 };
	std::string strExportConfig;

	app.set_config("--config");

	app.add_option("--image,i", imageFiles, "Image Files")->check(CLI::ExistingFile)->required(true)->ignore_case(true);
	app.add_option("--smallTemplate,--st", smallTemplateFiles, "Small Template Files")->check(CLI::ExistingFile)->ignore_case(true);
	app.add_option("--smallTemplateSize", vecSmallTemplateSize, "Sizes of the small templates");
	app.add_option("--largeTemplate,--lt", largeTemplateFiles, "Large Template Files")->check(CLI::ExistingFile)->ignore_case(true);

	auto pExport = app.add_option("--export_config", strExportConfig, "Export command line for later reuse")->ignore_case(true);
	
    CLI11_PARSE(app, argc, argv);

	std::boolalpha(std::cout);
	std::cout << my_flag;

	if (!strExportConfig.empty()) {

		app.remove_option(pExport);

		std::ofstream file(strExportConfig);
		file << app.config_to_str(true, true) << std::endl;
	}

    return 0;
}