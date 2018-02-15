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

static void test_parse_null()
{
	Json test;
	EXPECT_EQ_INT(ParseStatus::ok, test.parse("null"));
	EXPECT_EQ_INT(JsonType::Null, test.get_type());
}

static void test_parse_true()
{
	Json test;
	EXPECT_EQ_INT(ParseStatus::ok, test.parse("true"));
	EXPECT_EQ_INT(JsonType::True, test.get_type());
}

static void test_parse_false()
{
	Json test;
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

static void test_parse_number() {
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
}

#define TEST_ERROR(error, json)\
    do {\
        Json test;\
        EXPECT_EQ_INT(error, test.parse(json));\
        EXPECT_EQ_INT(JsonType::Null, test.get_type());\
    } while(0)

static void test_parse_expect_value() {
	TEST_ERROR(ParseStatus::expect_value, "");
	TEST_ERROR(ParseStatus::expect_value, "");
}

static void test_parse_invalid_value() {
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

static void test_parse_root_not_singular() {
	TEST_ERROR(ParseStatus::root_not_singular, "null x");
#if 1
	/* invalid number */
	TEST_ERROR(ParseStatus::root_not_singular, "0123"); /* after zero should be '.' or nothing */
	TEST_ERROR(ParseStatus::root_not_singular, "0x0");
	TEST_ERROR(ParseStatus::root_not_singular, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 1
	TEST_ERROR(ParseStatus::number_too_big, "1e309");
	TEST_ERROR(ParseStatus::number_too_big, "-1e309");
#endif
}

static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
