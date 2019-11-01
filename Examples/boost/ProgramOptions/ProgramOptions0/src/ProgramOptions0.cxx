#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
	return os;
}

int main(int argc, char* argv[]) 
{
	try {

		int opt;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "Show help message")
			("optimization", po::value<int>(&opt)->default_value(10), "optimization level")
			("include-path,I", po::value< std::vector<std::string> >(),	"include path")
			("input-file", po::value< std::vector<std::string> >()->multitoken()->composing(), "input file")
			("config", po::value<std::string>(), "Config file")
		;
		
		po::variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("config"))
		{
			std::ifstream ifs{ vm["config"].as<std::string>().c_str() };
			if (ifs)
				store(parse_config_file(ifs, desc), vm);
		}
		notify(vm);
		
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 0;
		}

		if (vm.count("input-file")) {

			auto inputFiles = vm["input-file"].as< std::vector<std::string> >();

			std::cout << "Input files: " <<  inputFiles << "\n";

			//std::copy(inputFiles.begin(), inputFiles.end(), std::ostream_iterator<std::string>(std::cout, ", "));
		}

	}
	catch (std::exception & e) {
		std::cerr << e.what() << "\n";
	}

}