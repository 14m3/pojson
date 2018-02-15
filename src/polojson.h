#pragma once
#include "parse.h"

namespace polojson
{

class Json
{
public:
	Json() :parser_(nullptr) {}
	~Json() { delete parser_; }
	ParseStatus parse(const std::string& content);
	JsonType get_type();
	double get_number();
private:
	Parser* parser_;
};
}