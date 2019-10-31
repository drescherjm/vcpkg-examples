#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) 
{
	try {

		int opt;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "Show help message")
			("optimization", po::value<int>(&opt)->default_value(10),
				"optimization level")
				("include-path,I", po::value< std::vector<std::string> >(),
					"include path")
					("input-file", po::value< std::vector<std::string> >(), "input file")
			;


		po::variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 0;
		}
	}
	catch (std::exception & e) {
		std::cerr << e.what() << "\n";
	}

}