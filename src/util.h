#pragma once
namespace polojson
{
	enum class JsonType
	{
		Null,
		True,
		False,
		Number,
		String,
		Array,
		Object
	};

	enum class ParseStatus
	{
		ok = 0,
		expect_value,
		invalid_value,
		root_not_singular,
		number_too_big,
		miss_quotation_mark,
		invalid_string_escape,
		invalid_string_char,
		unknown
	};

}