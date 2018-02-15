#include "polojson.h"

using namespace polojson;

ParseStatus polojson::Json::parse(const std::string & content)
{
	parser_ = new Parser(content);
	return parser_->parse();
}

JsonType polojson::Json::get_type()
{
	return parser_->get_type();
}

double polojson::Json::get_number()
{
	return parser_->get_number();
}
