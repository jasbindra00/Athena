//helper class for reading files.
#ifndef FILEREADER_H
#define FILEREADER_H
#include <fstream>
#include <limits>
#include "StreamAttributes.h"
#include "KeyProcessing.h"
class FileReader {
private:
	std::ifstream file;
	Attributes linestream;
	std::string line;
	std::string word;
	int linenumber;
	int previouspos;
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
	FileReader& NextLine() {
		std::getline(file, line);
		linestream.PopulateStream(line);
		++linenumber;
		return *this;
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
	bool CheckStandardSyntax(const std::vector<std::string>& syntaxes) {
		bool correct = true;
		for (auto& syntax : syntaxes) {
			NextLine();
			if (line != syntax) correct = false;
		}
		return correct;
	}
	std::string GetWord() {
		linestream >> word;
		return word;
	}

	std::string PeekWord() {
		return linestream.PeekWord();
	}
	std::stringstream& GetLineStream() { return linestream; }
	bool EndOfFile() {
		return file.eof();
	}
	void CloseFile() {
		file.close();
	}
	void SeekToLineTemplate(const std::string& str) {
		previouspos = file.tellg();
		while (!file.eof()) {
			NextLine();
			if (line == str) break;
		}
	}

	std::streampos TellG() { return file.tellg(); }
	std::string ReturnLine() { return line; }
	std::ifstream& GetFileStream() { return file; }
};

#endif