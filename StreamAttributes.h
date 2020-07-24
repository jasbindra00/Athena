#ifndef STREAMATTRIBUTES_H
#define STREAMATTRIBUTES_H
#include <sstream>
#include <string>
class Attributes : public std::stringstream {
private:
	std::string word;
	int previouswordpos{ 0 };
public:
	explicit Attributes() {
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
		*this >> word;
		seekg(pos);
		return word;
	}
	std::string GetWord() {
		previouswordpos = tellg();
		*this >> word;
		return word;
	}
	void PutBackPreviousWord() {
		seekg(previouswordpos);
	}
};

#endif