//helper class for reading files.
#ifndef FILEREADER_H
#define FILEREADER_H
#include <fstream>
#include <limits>
#include "StreamAttributes.h"
class FileReader {
private:
	std::ifstream file;
	Attributes linestream;
	std::string line;
	std::string word;
	int linenumber;
public:
	bool LoadFile(const std::string& name) {
		if (file.is_open()) file.close();
		file.open(name, std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		linestream.ResetStream();
		linenumber = 1;
		return true;
	}
	int GetLineNumber() { return linenumber; }
	std::string GetLineNumberString(){ return std::to_string(linenumber); }
	void NextLine() {
		std::getline(file, line);
		linestream.PopulateStream(line);
		++linenumber;
	}
	void GotoLine(const unsigned int& num) {
		file.seekg(std::ios::beg);
		for (int i = 0; i < num - 1; ++i) {
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		linenumber = num;
	}
	void PutBackLine() {
		GotoLine(linenumber - 1);
	}

	std::string GetWord() {
		linestream >> word;
		return word;
	}
	std::stringstream& GetLineStream() { return linestream; }
	bool EndOfFile() {
		return file.eof();
	}
	void CloseFile() {
		file.close();
	}
	std::string ReturnLine() { return line; }
};

#endif