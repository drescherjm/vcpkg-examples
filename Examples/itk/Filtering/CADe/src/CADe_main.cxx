#include <iostream>
#include "CADe.h"
#include "CLI\CLI.hpp"

/////////////////////////////////////////////////////////////////////////////////////////

const uint16_t SMALL_TEMPLATE_SIZE{ 50 };
const uint16_t LARGE_TEMPLATE_SIZE{ 100 };

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{

	CLI::App app;

	std::vector<std::string> imageFiles, smallTemplateFiles, largeTemplateFiles;
	std::string strExportConfig;

	std::vector<uint16_t> vecSmallTemplateSize{ SMALL_TEMPLATE_SIZE }, vecLargeTemplateSize{ LARGE_TEMPLATE_SIZE };

	bool	bDebug = false;

	app.set_config("--config");

	app.add_flag("--debug", bDebug, "Create image files for debugging the result.");
	app.add_option("--image,i", imageFiles, "Image Files")->check(CLI::ExistingFile)->required(true)->ignore_case(true);
	app.add_option("--smallTemplate,--st", smallTemplateFiles, "Small Template Files")->check(CLI::ExistingFile)->ignore_case(true);
	app.add_option("--smallTemplateSize", vecSmallTemplateSize, "Sizes of the small templates");
	app.add_option("--largeTemplate,--lt", largeTemplateFiles, "Large Template Files")->check(CLI::ExistingFile)->ignore_case(true);

	auto pExport = app.add_option("--export_config", strExportConfig, "Export command line for later reuse")->ignore_case(true);
	
	CLI11_PARSE(app, argc, argv);

	if (!strExportConfig.empty()) {

		// Don't export a file with the export command exported!
		app.remove_option(pExport);

		std::ofstream file(strExportConfig);
		file << app.config_to_str(true, true) << std::endl;
	}
	
	if (!imageFiles.empty() && (!smallTemplateFiles.empty() || largeTemplateFiles.empty())) {

		CADe cade;

		for (const auto& strImage : imageFiles) {
			cade.addImageFile(strImage);
		}
		
		if (!smallTemplateFiles.empty()) {
			int nPos = 0;
			uint16_t templateSize = SMALL_TEMPLATE_SIZE;
			for (const auto& strTemplate : smallTemplateFiles) {
				if (nPos < vecSmallTemplateSize.size()) {
					// If there are any command line size parameters use them.
					templateSize = vecSmallTemplateSize[nPos++];
				}
				cade.addTemplate(strTemplate, templateSize);
			}
		}

		if (!largeTemplateFiles.empty()) {
			int nPos = 0;
			uint16_t templateSize = LARGE_TEMPLATE_SIZE;
			for (const auto& strTemplate : largeTemplateFiles) {
				if (nPos < vecLargeTemplateSize.size()) {
					// If there are any command line size parameters use them.
					templateSize = vecLargeTemplateSize[nPos++];
				}
				cade.addTemplate(strTemplate, templateSize);
			}
		}

		cade.enableDebugMode(bDebug);
		if (!cade.excute()) {
			std::cerr << cade.getErrorMessages() << std::endl;
		}

	}

}

/////////////////////////////////////////////////////////////////////////////////////////

// 		cade.addImageFile(R"(C:\Users\jdrescher\Box\Ingrid\BG\BG_120.tiff)");
// 		cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr50\sig50_template.tiff)",50);
// 		cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr100\sig100_template.tiff)", 100);

// 		cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr25\sig25_template.tiff)", 100);
// 		cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr12\sig12_template.tiff)", 50);