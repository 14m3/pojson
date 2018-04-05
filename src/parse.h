#pragma once

#include <string>
#include <cassert>
#include <vector>
#include "util.h"

namespace polojson
{

class Parser
{
public:

	Parser() :content_(), parse_pos_(0){}
	~Parser();

	JsonElem Parse(const std::string& content);

    ParseErrorCode GetErrorCode();
	void SetContent(const std::string& content);

private:
    void ParseWhitespace();

    //ParseLiteral include parse true, false, null
    JsonElem ParseLiteral(const std::string &, JsonType);
    JsonElem ParseNumber();
    int ParseHex4();
    std::string EncodeUtf8(int);
    JsonElem ParseString();
    JsonElem ParseArray();
    JsonElem ParseValue();

private:
	std::string content_;
	size_t parse_pos_;

    //JsonElem parse_result_;
    ParseErrorCode error_code_;
};
}