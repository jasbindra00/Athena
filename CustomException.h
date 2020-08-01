#ifndef CUSTOMEXCEPTION_H
#define CUSTOMEXCEPTION_H
#include <exception>
#include <iostream>


class CustomException : public std::exception {
private:
	std::string errormsg;
	std::string str;
	int num;
public:
	CustomException(const std::string& msg, const std::string& fcnname = "") :std::exception(msg.c_str()) {
		
	}
	void SetErrorMsg(const std::string& x) { errormsg = x; }
	void SetNum(const int& x) { num = x; }
	void SetStr(const std::string& st) { str = st; }
	std::string GetStr() const { return str; }
	std::string GetErrorMsg() const { return errormsg; }
	int GetNum() const { return num; }
};




#endif