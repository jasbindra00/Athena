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
	std::string NextLine() {
		std::getline(file, line);
		linestream.PopulateStream(line);
		++linenumber;
		return line;
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
	bool SeekToLineKey(const std::string& key,const bool& reset, const bool& attr1, const bool& attr2) {
		previouspos = file.tellg();
		std::string attr1str;
		std::string attr2str; {
			Attributes keystream(KeyProcessing::ExtractAttributesToStream(key));
			attr1str = keystream.GetWord();
			attr2str = keystream.GetWord();
		}
		while (!file.eof()) {
			Attributes linestream = KeyProcessing::ExtractAttributesToStream(NextLine());
			std::string lineattr1 = linestream.PeekWord();
			std::string lineattr2 = linestream.PeekWord();
			if (attr1 && attr2) {
				if ((lineattr1 == attr1str) && (lineattr2 == attr2str)) return true;
			}
			else if (attr1) {
				if (attr1str == lineattr1) return true;
			}
			else if (attr2) {
				if (attr2str == lineattr2) return true;
			}
		}
		/*if(reset) file.seekg(previouspos);*/
		return false;
	}
	std::streampos TellG() { return file.tellg(); }
	std::string ReturnLine() { return line; }
	std::ifstream& GetFileStream() { return file; }
};

#endif