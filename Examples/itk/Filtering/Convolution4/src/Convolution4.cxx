#include <iostream>
#include "CADe.h"

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{

	// Verify command line arguments
// 	if (argc < 2)
// 	{
// 		std::cerr << "Usage: ";
// 		std::cerr << argv[0] << "inputImageFile [small_template] [large_template]" << std::endl;
// 		return EXIT_FAILURE;
// 	}


	CADe cade;

	cade.addImageFile(R"(C:\Users\jdrescher\Box\Ingrid\BG\BG_120.tiff)");
	cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr50\sig50_template.tiff)",50);
	cade.addTemplate(R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr100\sig100_template.tiff)", 100);
	
	cade.enableDebugMode(true);
	cade.excute();
}

