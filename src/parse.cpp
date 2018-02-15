#include <cctype> //int isdigit(int ch)
#include "parse.h"

using namespace polojson;

void polojson::Parser::parse_whitespace()
{
	for (auto i = content_.begin(); i != content_.end(); ++i)
	{
		if (*i == ' ' || *i == '\t' || *i == '\n' || *i == '\r')
		{
			parse_pos_++;
		}
		else
		{
			break;
		}
	}
}

ParseStatus polojson::Parser::parse_null()
{
	assert(content_[parse_pos_] == 'n');
	if (content_.substr(parse_pos_).find("null") != parse_pos_)
		return ParseStatus::invalid_value;
	parse_pos_ += 3;
	type_ = JsonType::Null;
	return ParseStatus::ok;
}

ParseStatus polojson::Parser::parse_true()
{
	assert(content_[parse_pos_] == 't');
	if (content_.substr(parse_pos_).find("true") != parse_pos_)
		return ParseStatus::invalid_value;
	parse_pos_ += 3;
	type_ = JsonType::True;
	return ParseStatus::ok;
}

ParseStatus polojson::Parser::parse_false()
{
	assert(content_[parse_pos_] == 'f');
	if (content_.substr(parse_pos_).find("false") != parse_pos_)
		return ParseStatus::invalid_value;
	parse_pos_ += 4;
	type_ = JsonType::False;
	return ParseStatus::ok;
}

ParseStatus polojson::Parser::parse_literal(const std::string &literal, JsonType type)
{
	assert(content_[parse_pos_] == literal[0]);
	if (content_.substr(parse_pos_).find(literal) != parse_pos_)
		return ParseStatus::invalid_value;
	parse_pos_ += literal.size() - 1;
	type_ = type;
	return ParseStatus::ok;
}

ParseStatus polojson::Parser::parse_number()
{
	int start_pos = parse_pos_;

	if (content_[parse_pos_] == '-')
		parse_pos_++;
	if (content_[parse_pos_] == '0')
		parse_pos_++;
	else
	{
		if (!isdigit(content_[parse_pos_]))
			return ParseStatus::invalid_value;
		for (++parse_pos_; isdigit(content_[parse_pos_]); ++parse_pos_);
	}

	if (content_[parse_pos_] == '.')
	{
		parse_pos_++;
		if (!isdigit(content_[parse_pos_]))
			return ParseStatus::invalid_value;
		for (++parse_pos_; isdigit(content_[parse_pos_]); ++parse_pos_);
	}

	if (content_[parse_pos_] == 'e' || content_[parse_pos_] == 'E')
	{
		parse_pos_++;
		if(content_[parse_pos_] == '-' || content_[parse_pos_] == '+')
			parse_pos_++;
		for (++parse_pos_; isdigit(content_[parse_pos_]); ++parse_pos_);
	}

	int count = parse_pos_ - start_pos;
	try
	{
		number_ = std::stod(content_.substr(start_pos, count));
	}
	catch (const std::invalid_argument&)
	{
		return ParseStatus::invalid_value;
	}
	catch (const std::out_of_range&)
	{
		return ParseStatus::number_too_big;
	}
	catch (const std::exception&)
	{
		return ParseStatus::unknown;
	}

	type_ = JsonType::Number;
	return ParseStatus::ok;
}

ParseStatus polojson::Parser::parse_value()
{
	switch (content_[parse_pos_])
	{
	case 'n':
		return parse_literal("null", JsonType::Null);
	case 't':
		return parse_literal("true", JsonType::True);
	case 'f':
		return parse_literal("false", JsonType::False);
	case '\0':
		return ParseStatus::expect_value;
	default:
		return parse_number();
	}
}

ParseStatus polojson::Parser::parse()
{
	ParseStatus ret;
	parse_whitespace();
	if ((ret = parse_value()) == ParseStatus::ok)
	{
		parse_whitespace();
		if (parse_pos_ != content_.size() - 1)
		{
			ret = ParseStatus::root_not_singular;
			type_ = JsonType::Null;
		}
	}
	return ret;
}
