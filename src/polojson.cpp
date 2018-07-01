#include "polojson.h"

using namespace polojson;

JsonElem polojson::Json::Parse(const std::string& content)
{
	return parser_->Parse(content);
}

Json& polojson::Json::operator=(const Json& other)
{
    if (this == &other)
        return *this;
    
    delete parser_;
    parser_ = new Parser();
    *parser_ = *other.parser_;
    return *this;
}

ParseErrorCode polojson::Json::GetErrorCode() const
{
    return parser_->GetErrorCode();
}
