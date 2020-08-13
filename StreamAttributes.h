#ifndef STREAMATTRIBUTES_H
#define STREAMATTRIBUTES_H
#include <sstream>
#include <string>
class Attributes : public std::stringstream {
private:
/*	std::string word;*/
	int previouswordpos{ 0 };
	std::string currentword;
public:
	explicit Attributes() {
	}
	Attributes(const std::string& str) {
		static_cast<std::stringstream&>(*this) << str;
	}
	Attributes& NextWord() {
		previouswordpos = tellg();
		*this >> currentword;
		return *this;
	}
	void ResetStream() {
		*static_cast<std::stringstream*>(this) = std::stringstream{};
		previouswordpos = 0;
	}
	void PopulateStream(const std::string& attributes) {
		*static_cast<std::stringstream*>(this) = std::stringstream{attributes};
		previouswordpos = 0;
	}
	std::string PeekWord() {
		int pos = tellg();
		std::string w; //word might be empty
		*this >> w;
		seekg(pos);
		return w;
	}
	std::string GetWord() {
		NextWord();
		return currentword;
	}
	std::string ReturnWord() {
		return currentword;
	}
	void PutBackPreviousWord() {
		seekg(previouswordpos);
	}
	void InsertWord() {

	}
	std::string GetRemainingLine() {
		int pos = tellg();
		std::string str;
		std::getline(*this, str);
		seekg(pos);
		return str;
	}
};

#endif