#ifndef STREAMATTRIBUTES_H
#define STREAMATTRIBUTES_H
#include <sstream>
#include <string>
class Attributes : public std::stringstream {
private:
/*	std::string word;*/
	int previouswordpos{ 0 };
public:
	explicit Attributes() {
	}
	Attributes(const std::string& str) {
		static_cast<std::stringstream&>(*this) << str;
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
		previouswordpos = tellg();
		std::string w;
		*this >> w;
		return w;
	}
	void PutBackPreviousWord() {
		seekg(previouswordpos);
	}
};

#endif