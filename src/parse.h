#pragma once

#include <string>
#include "util.h"
#include <cassert>

namespace polojson
{

class Parser
{
public:
	Parser() :content_(""), parse_pos_(0),
		type_(JsonType::Null), number_(0.0) {}
	explicit Parser(const std::string& content) :
		content_(content), parse_pos_(0),
		type_(JsonType::Null), number_(0.0) {}
	ParseStatus parse();
	JsonType get_type() { return type_; }
	double get_number() { assert(type_ == JsonType::Number); return number_; }
private:
	std::string content_;
	size_t parse_pos_;
	JsonType type_;
	double number_;
	void parse_whitespace();
	ParseStatus parse_null();
	ParseStatus parse_true();
	ParseStatus parse_false();
	ParseStatus parse_literal(const std::string &, JsonType);
	ParseStatus parse_number();
	ParseStatus parse_value();
};
}