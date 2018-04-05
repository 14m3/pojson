#pragma once
#include "parse.h"

namespace polojson
{

class Json
{
public:
	Json();
	~Json() { delete parser_; }
	JsonElem Parse(const std::string& content);
    ParseErrorCode GetErrorCode();
private:
	Parser* parser_;
};
}