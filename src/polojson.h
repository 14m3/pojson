#pragma once
#include "parse.h"

namespace polojson
{

class Json
{
public:
    Json() :parser_(new Parser()) {};
	~Json() { delete parser_; }
	JsonElem Parse(const std::string& content);
    Json& operator=(const Json& other);
    ParseErrorCode GetErrorCode() const;
private:
	Parser* parser_;
};
}