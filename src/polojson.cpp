#include "polojson.h"

using namespace polojson;

polojson::Json::Json()
{
	parser_ = new Parser();
}

JsonElem polojson::Json::Parse(const std::string& content)
{
	return parser_->Parse(content);
}

ParseErrorCode polojson::Json::GetErrorCode()
{
    return parser_->GetErrorCode();
}
