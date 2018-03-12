#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "polojson.h"

using namespace polojson;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

static void test_parse_null()
{
	Json test;
	EXPECT_EQ_INT(ParseStatus::ok, test.parse("null"));
	EXPECT_EQ_INT(JsonType::Null, test.get_type());
}

static void test_parse_true()
{
	Json test;
	test.set_boolean(0);
	EXPECT_EQ_INT(ParseStatus::ok, test.parse("true"));
	EXPECT_EQ_INT(JsonType::True, test.get_type());
}

static void test_parse_false()
{
	Json test;
	test.set_boolean(1);
	EXPECT_EQ_INT(ParseStatus::ok, test.parse("false"));
	EXPECT_EQ_INT(JsonType::False, test.get_type());
}

#define TEST_NUMBER(expect, json)\
    do {\
        Json test;\
        EXPECT_EQ_INT(ParseStatus::ok, test.parse(json));\
        EXPECT_EQ_INT(JsonType::Number, test.get_type());\
        EXPECT_EQ_DOUBLE(expect, test.get_number());\
    } while(0)

static void test_parse_number() 
{
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect, json)\
    do {\
        Json test;\
        EXPECT_EQ_INT(ParseStatus::ok, test.parse(json));\
        EXPECT_EQ_INT(JsonType::String, test.get_type());\
        EXPECT_EQ_STRING(expect, test.get_string().c_str(), test.get_string_length());\
    } while(0)

static void test_parse_string() 
{
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}

#define TEST_ERROR(error, json)\
    do {\
        Json test;\
        test.set_boolean(0);\
        EXPECT_EQ_INT(error, test.parse(json));\
        EXPECT_EQ_INT(JsonType::Null, test.get_type());\
    } while(0)

static void test_parse_expect_value() 
{
	TEST_ERROR(ParseStatus::expect_value, "");
	TEST_ERROR(ParseStatus::expect_value, " ");
}

static void test_parse_invalid_value()
{
	TEST_ERROR(ParseStatus::invalid_value, "nul");
	TEST_ERROR(ParseStatus::invalid_value, "?");

#if 1
	/* invalid number */
	TEST_ERROR(ParseStatus::invalid_value, "+0");
	TEST_ERROR(ParseStatus::invalid_value, "+1");
	TEST_ERROR(ParseStatus::invalid_value, ".123"); /* at least one digit before '.' */
	TEST_ERROR(ParseStatus::invalid_value, "1.");   /* at least one digit after '.' */
	TEST_ERROR(ParseStatus::invalid_value, "INF");
	TEST_ERROR(ParseStatus::invalid_value, "inf");
	TEST_ERROR(ParseStatus::invalid_value, "NAN");
	TEST_ERROR(ParseStatus::invalid_value, "nan");
#endif
}

static void test_parse_root_not_singular()
{
	TEST_ERROR(ParseStatus::root_not_singular, "null x");
#if 1
	/* invalid number */
	TEST_ERROR(ParseStatus::root_not_singular, "0123"); /* after zero should be '.' or nothing */
	TEST_ERROR(ParseStatus::root_not_singular, "0x0");
	TEST_ERROR(ParseStatus::root_not_singular, "0x123");
#endif
}

static void test_parse_number_too_big()
{
#if 1
	TEST_ERROR(ParseStatus::number_too_big, "1e309");
	TEST_ERROR(ParseStatus::number_too_big, "-1e309");
#endif
}

static void test_parse_missing_quotation_mark()
{
	TEST_ERROR(ParseStatus::miss_quotation_mark, "\"");
	TEST_ERROR(ParseStatus::miss_quotation_mark, "\"abc");
}

static void test_parse_invalid_string_escape()
{
#if 1
	TEST_ERROR(ParseStatus::invalid_string_escape, "\"\\v\"");
	TEST_ERROR(ParseStatus::invalid_string_escape, "\"\\'\"");
	TEST_ERROR(ParseStatus::invalid_string_escape, "\"\\0\"");
	TEST_ERROR(ParseStatus::invalid_string_escape, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char()
{
#if 1
	TEST_ERROR(ParseStatus::invalid_string_char, "\"\x01\"");
	TEST_ERROR(ParseStatus::invalid_string_char, "\"\x1F\"");
#endif
}

static void test_access_null()
{
	Json test;
	test.set_string("a", 1);
	test.set_null();
	EXPECT_EQ_INT(JsonType::Null, test.get_type());
}

static void test_access_boolean()
{
	Json test;
	test.set_string("a", 1);
	test.set_boolean(1);
	EXPECT_TRUE(test.get_boolean());
	test.set_boolean(0);
	EXPECT_FALSE(test.get_boolean());
}

static void test_access_number()
{
	Json test;
	test.set_string("a", 1);
	test.set_number(1234.5);
	EXPECT_EQ_DOUBLE(1234.5, test.get_number());
}

static void test_access_string()
{
	Json test;
	test.set_string("", 0);
	EXPECT_EQ_STRING("", test.get_string().c_str(), test.get_string_length());
	test.set_string("Hello", 5);
	EXPECT_EQ_STRING("Hello", test.get_string().c_str(), test.get_string_length());
}

static void test_parse()
{
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_string();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();

	test_access_null();
	test_access_boolean();
	test_access_number();
	test_access_string();
}

int main()
{
#ifdef _WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
