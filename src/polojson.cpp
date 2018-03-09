#include "polojson.h"

using namespace polojson;

ParseStatus polojson::Json::parse(const std::string & content)
{
	assert(parser_ != nullptr);
	parser_->set_content(content);
	return parser_->parse();
}

JsonType polojson::Json::get_type()
{
	assert(parser_ != nullptr);
	return parser_->get_type();
}

bool polojson::Json::get_boolean()
{
	assert(parser_ != nullptr);
	return parser_->get_boolean();
}

double polojson::Json::get_number()
{
	assert(parser_ != nullptr);
	return parser_->get_number();
}

std::string polojson::Json::get_string()
{
	assert(parser_ != nullptr);
	return parser_->get_string();
}

size_t polojson::Json::get_string_length()
{
	assert(parser_ != nullptr);
	return parser_->get_string_length();
}

void polojson::Json::set_null()
{
	return parser_->set_null();
}

void polojson::Json::set_boolean(int b)
{
	return parser_->set_boolean(b);
}

void polojson::Json::set_number(double n)
{
	return parser_->set_number(n);
}

void polojson::Json::set_string(const char * s, size_t len)
{
	return parser_->set_string(s, len);
}
