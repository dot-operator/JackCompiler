#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	std::vector<std::string> files;

#ifdef _DEBUG
	files.push_back("test.jack");
#else
	if (argc != 2) {
		std::cout << "Usage: JackCompiler <file.jack / directory>\n";
		return 1;
}
	std::string firstFile = argv[1];

	fs::path path = argv[1];
	if (fs::is_directory(path)) {
		for (auto& p : fs::recursive_directory_iterator(argv[1])) {
			// Add if it's a .jack file
			if (fs::is_regular_file(p) && path.string().find(".jack") != firstFile.npos) {
				files.push_back(p.path().string());
			}
		}
	}
	else if (fs::is_regular_file(path) && path.string().find(".jack") != firstFile.npos) {
		files.push_back(path.string());
	}

#endif

	Parser parser(files);
	
	
	std::cout << "\nWrote " << files.size() << " VM files.\n\n";
	return 0;
}