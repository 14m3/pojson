#include <cctype> 
//int isdigit(int ch), the argument should first be converted to unsigned char
#include <new> //new(std::nothrow)
#include "parse.h"

using namespace polojson;

polojson::Parser::~Parser()
{

}

ParseErrorCode polojson::Parser::GetErrorCode()
{
    return error_code_;
}

void polojson::Parser::SetContent(const std::string& content)
{
	content_ = content;
	parse_pos_ = 0;
}

void polojson::Parser::ParseWhitespace()
{
	for (size_t i = parse_pos_; i != content_.size(); ++i)
	{
		char &ch = content_[i];
		if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
		{
			parse_pos_++;
		}
		else
		{
			break;
		}
	}
}

JsonElem polojson::Parser::ParseLiteral(const std::string &literal,
	JsonType type)
{
	assert(content_[parse_pos_] == literal[0]);
    if (content_.substr(parse_pos_).find(literal) != 0)
    {
        error_code_ = ParseErrorCode::kInvalidValue;
        return JsonElem{ nullptr };
    }
	
    JsonElem result;
    switch (type)
    {
    case polojson::JsonType::kNull:
        result = JsonElem{ nullptr };
        break;
    case polojson::JsonType::kTrue:
        result = JsonElem{ true };
        break;
    case polojson::JsonType::kFalse:
        result = JsonElem{ false };
        break;
    default:
        result = JsonElem{ nullptr };
        break;
    }
	parse_pos_ += literal.size();
	error_code_  = ParseErrorCode::kOK;
    return result;
}

JsonElem polojson::Parser::ParseNumber()
{
	int start_pos = parse_pos_;
    JsonElem result;
	if (content_[parse_pos_] == '-')
		parse_pos_++;
	if (content_[parse_pos_] == '0')
		parse_pos_++;
	else
	{
        if (!isdigit(static_cast<unsigned char>(content_[parse_pos_])))
        {
            error_code_ =  ParseErrorCode::kInvalidValue;
            return JsonElem{ nullptr };
        }
			
		for (++parse_pos_;
			isdigit(static_cast<unsigned char>(content_[parse_pos_]));
			++parse_pos_);
	}

	if (content_[parse_pos_] == '.')
	{
		parse_pos_++;
        if (!isdigit(static_cast<unsigned char>(content_[parse_pos_])))
        {
            error_code_ = ParseErrorCode::kInvalidValue;
            return JsonElem{ nullptr };
        }
		for (++parse_pos_;
			isdigit(static_cast<unsigned char>(content_[parse_pos_]));
			++parse_pos_);
	}

	if (content_[parse_pos_] == 'e' || content_[parse_pos_] == 'E')
	{
		parse_pos_++;
		if(content_[parse_pos_] == '-' || content_[parse_pos_] == '+')
			parse_pos_++;
		for (++parse_pos_;
			isdigit(static_cast<unsigned char>(content_[parse_pos_]));
			++parse_pos_);
	}

	int count = parse_pos_ - start_pos;
	try
	{
        result = JsonElem{ std::stod(content_.substr(start_pos, count)) };
	}
	catch (const std::invalid_argument&)
	{
        error_code_ = ParseErrorCode::kInvalidValue;
        return JsonElem{ nullptr };
	}
	catch (const std::out_of_range&)
	{
        error_code_ = ParseErrorCode::kNumberTooBig;
        return JsonElem{ nullptr };
	}
	catch (const std::exception&)
	{
        error_code_ = ParseErrorCode::kUnknown;
        return JsonElem{ nullptr };
	}

    error_code_ = ParseErrorCode::kOK;
    return result;
}

int polojson::Parser::ParseHex4()
{
	int hex_number = 0;
	size_t start_pos = parse_pos_;
	for (size_t i = 0; i < 4; ++i)
	{
		if (!isxdigit(static_cast<unsigned char>(content_[parse_pos_++])))
			return -1; //invalid_unicode_hex;
	}

	try
	{
		hex_number = std::stoi(content_.substr(start_pos, 4), 0, 16);
		//hex_number cannot be negative, so return -1 when an error occurs
	}
	catch (const std::exception&)
	{
		return -1;
	}
	return hex_number;
}

std::string polojson::Parser::EncodeUtf8(int n)
{
	unsigned u = static_cast<unsigned int>(n);
	std::string utf8_str;
	if (u <= 0x7F) // 0xxxxxxx
		utf8_str.push_back(static_cast<char>(u & 0xFF));
	else if (u <= 0x7FF) // 110xxxxx, 10xxxxxx
	{
		utf8_str.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0xFF)));
		utf8_str.push_back(static_cast<char>(0x80 | (u & 0x3F)));
	}
	else if (u <= 0xFFFF)
	{
		utf8_str.push_back(static_cast<char>(0xe0 | ((u >> 12) & 0xFF)));
		utf8_str.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
		utf8_str.push_back(static_cast<char>(0x80 | (u & 0x3F)));
	}
	else
	{
		assert(u <= 0x10FFFF);
		utf8_str.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0xFF)));
		utf8_str.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
		utf8_str.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
		utf8_str.push_back(static_cast<char>(0x80 | (u & 0x3F)));
	}
	return utf8_str;
}

JsonElem polojson::Parser::ParseString()
{
    JsonElem result;
	std::string str_temp;
	assert(content_[parse_pos_] == '\"');
	parse_pos_++;
	ParseWhitespace();
	for (;;)
	{
		char ch = content_[parse_pos_++];
		switch (ch)
		{
		case '\"':
            error_code_ = ParseErrorCode::kOK;
            result = JsonElem{ str_temp };
            return result;
		case '\\':
			switch (content_[parse_pos_++])
			{
			case '\"': str_temp.push_back('\"'); break;
			case '\\': str_temp.push_back('\\'); break;
			case '/': str_temp.push_back('/'); break;
			case 'b': str_temp.push_back('\b'); break;
			case 'f': str_temp.push_back('\f'); break;
			case 'n': str_temp.push_back('\n'); break;
			case 'r': str_temp.push_back('\r'); break;
			case 't': str_temp.push_back('\t'); break;
			case 'u':
			{
				int u = ParseHex4();
                if (u < 0)
                {
                    error_code_ = ParseErrorCode::kInvalidUnicodeHex;
                    return JsonElem{ nullptr };
                }
				if (u >= 0xD800 && u <= 0xDBFF)
				{
                    if (content_[parse_pos_++] != '\\')
                    {
                        error_code_ = ParseErrorCode::kInvalidUnicodeSurrogate;
                        return JsonElem{ nullptr };
                    }
                    if (content_[parse_pos_++] != 'u')
                    {
                        error_code_ = ParseErrorCode::kInvalidUnicodeSurrogate;
                        return JsonElem{ nullptr };
                    }
					int u2 = ParseHex4();
                    if (u2 < 0)
                    {
                        error_code_ = ParseErrorCode::kInvalidUnicodeHex;
                        return JsonElem{ nullptr };
                    }
                    if (u2 < 0xDC00 || u2 >0xDFFF)
                    {
                        error_code_ = ParseErrorCode::kInvalidUnicodeSurrogate;
                        return JsonElem{ nullptr };
                    }

					u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
					
				}
				str_temp += EncodeUtf8(u);
				break;
			}
			default:
                error_code_ = ParseErrorCode::kInvalidStringEscape;
                return JsonElem{ nullptr };
			}
			break;
		case '\0':
            error_code_ = ParseErrorCode::kMissQuotationMark;
            return JsonElem{ nullptr };
		default:
            if (static_cast<unsigned char>(ch) < 0x20)
            {
                error_code_ = ParseErrorCode::kInvalidStringChar;
                return JsonElem{ nullptr };
            }
			str_temp.push_back(ch);
		}
	}
}

JsonElem polojson::Parser::ParseArray()
{
    array_t temp_array;
	assert(content_[parse_pos_] == '[');
	parse_pos_++;
	ParseWhitespace();
	if (content_[parse_pos_] == ']')
	{
		parse_pos_++;
        error_code_ = ParseErrorCode::kOK;
        return JsonElem{ temp_array };
	}

	for (;;)
	{
        JsonElem temp_result = ParseValue();
        ParseErrorCode temp_error_code = GetErrorCode();
        if (temp_error_code != ParseErrorCode::kOK)
            return temp_result;
        
        temp_array.emplace_back(temp_result);
		ParseWhitespace();
		if (content_[parse_pos_] == ',')
		{
			parse_pos_++;
			ParseWhitespace();
		}
		else if (content_[parse_pos_] == ']')
		{
			parse_pos_++;
			error_code_ =  ParseErrorCode::kOK;
            return JsonElem{ temp_array };
		}
        else
        {
            error_code_ = ParseErrorCode::kMissCommaOrSquareBracket;
            return JsonElem{ nullptr };
        }
	}
}

JsonElem polojson::Parser::ParseValue()
{
	switch (content_[parse_pos_])
	{
	case 'n':
		return ParseLiteral("null", JsonType::kNull);
	case 't':
		return ParseLiteral("true", JsonType::kTrue);
	case 'f':
		return ParseLiteral("false", JsonType::kFalse);
	case '"':
		return ParseString();
	case '[':
		return ParseArray();
	case '\0':
        error_code_ = ParseErrorCode::kExpectValue;
        return JsonElem{ nullptr };
	default:
		return ParseNumber();
	}
}


JsonElem polojson::Parser::Parse(const std::string& content)
{
    SetContent(content);
	ParseWhitespace();
    JsonElem temp_result = ParseValue();
	if (GetErrorCode() == ParseErrorCode::kOK)
	{
		ParseWhitespace();
		if (parse_pos_ < content_.size())
		{
			error_code_ = ParseErrorCode::kRootNotSingular;
            return JsonElem{ nullptr };
		}
	}
	return temp_result;
}

