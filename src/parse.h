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

	void set_content(const std::string & content) { content_ = content; }
	ParseStatus parse();
	JsonType get_type() { return type_; }
	bool get_boolean()
	{
		assert(type_ == JsonType::True || type_ == JsonType::False);
		return type_ == JsonType::True;
	}
	double get_number()
	{
		assert(type_ == JsonType::Number); return number_;
	}
	std::string get_string()
	{
		assert(type_ == JsonType::String); return str_;
	}
	size_t get_string_length()
	{
		assert(type_ == JsonType::String); return str_len_;
	}

	void set_null() { type_ = JsonType::Null; }
	void set_boolean(int b) { type_ = b ? JsonType::True : JsonType::False; }
	void set_number(double n) { type_ = JsonType::Number; number_ = n; }
	void set_string(const std::string& s, size_t len)
	{
		type_ = JsonType::String; str_ = s + '\0'; str_len_ = len;
	}
private:
	std::string content_;
	size_t parse_pos_;
	JsonType type_;
	double number_;
	std::string str_;
	size_t str_len_;

	void parse_whitespace();
	ParseStatus parse_null();
	ParseStatus parse_true();
	ParseStatus parse_false();
	ParseStatus parse_literal(const std::string&, JsonType);
	ParseStatus parse_number();
	int parse_hex4();
	std::string encode_utf8(int);
	ParseStatus parse_string();
	ParseStatus parse_value();
};
}