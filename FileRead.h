#ifndef FILEREAD_H
#define FILEREAD_H
#include <string>
#include <sstream>
#include <fstream>



struct FileRead {
protected:
	static std::string line;
	static std::string word;
	static std::stringstream wordstream;
public:
	static void PopulateStream(const std::string& line) {
		wordstream.str(line);
		wordstream.seekg(0, std::ios_base::beg);
	}
	static void ExtractLineToStream(std::ifstream& file) {
		std::getline(file, line);
		wordstream << line;
	}
	static std::string ExtractWordFromStream() {
		wordstream >> word;
	}
};

std::string FileRead::line{};
std::string FileRead::word{};
std::stringstream FileRead::wordstream = std::stringstream{};
#endif