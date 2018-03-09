#pragma once
#include "parse.h"

namespace polojson
{

class Json
{
public:
	Json() :parser_(new Parser()) {}
	~Json() { delete parser_; }
	ParseStatus parse(const std::string& content);
	JsonType get_type();
	bool get_boolean();
	double get_number();
	std::string get_string();
	size_t get_string_length();

	void set_null();
	void set_boolean(int b);
	void set_number(double n);
	void set_string(const char* s, size_t len);

private:
	Parser* parser_;
};
}