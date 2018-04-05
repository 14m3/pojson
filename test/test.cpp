#ifdef _WIN32
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
#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif

static void test_parse_null()
{
	Json test;
    JsonElem result = test.Parse("null");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kNull, result.type());
}

static void test_parse_true()
{
	Json test;
    JsonElem result = test.Parse("true");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kTrue, result.type());
}

static void test_parse_false()
{
	Json test;
    JsonElem result = test.Parse("false");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kFalse, result.type());
}

#define TEST_NUMBER(expect, json)\
    do {\
        Json test;\
        JsonElem result = test.Parse(json);\
        EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());\
        EXPECT_EQ_INT(JsonType::kNumber, result.type());\
        EXPECT_EQ_DOUBLE(expect, result.ToNumber());\
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
        JsonElem result = test.Parse(json);\
        EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());\
        EXPECT_EQ_INT(JsonType::kString, result.type());\
        EXPECT_EQ_STRING(expect, result.ToString().c_str(), result.ToString().length());\
    } while(0)

static void test_parse_string() 
{
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

#define TEST_ERROR(error, json)\
    do {\
        Json test;\
        JsonElem result = test.Parse(json);\
        EXPECT_EQ_INT(error, test.GetErrorCode());\
        EXPECT_EQ_INT(JsonType::kNull, result.type());\
    } while(0)

static void test_parse_expect_value() 
{
	TEST_ERROR(ParseErrorCode::kExpectValue, "");
	TEST_ERROR(ParseErrorCode::kExpectValue, " ");
}

static void test_parse_invalid_value()
{
	TEST_ERROR(ParseErrorCode::kInvalidValue, "nul");
	TEST_ERROR(ParseErrorCode::kInvalidValue, "?");

#if 1
	/* invalid number */
	TEST_ERROR(ParseErrorCode::kInvalidValue, "+0");
	TEST_ERROR(ParseErrorCode::kInvalidValue, "+1");
	TEST_ERROR(ParseErrorCode::kInvalidValue, ".123"); /* at least one digit before '.' */
	TEST_ERROR(ParseErrorCode::kInvalidValue, "1.");   /* at least one digit after '.' */
	TEST_ERROR(ParseErrorCode::kInvalidValue, "INF");
	TEST_ERROR(ParseErrorCode::kInvalidValue, "inf");
	TEST_ERROR(ParseErrorCode::kInvalidValue, "NAN");
	TEST_ERROR(ParseErrorCode::kInvalidValue, "nan");
#endif
}

static void test_parse_root_not_singular()
{
	TEST_ERROR(ParseErrorCode::kRootNotSingular, "null x");
	/* invalid number */
	TEST_ERROR(ParseErrorCode::kRootNotSingular, "0123"); /* after zero should be '.' or nothing */
	TEST_ERROR(ParseErrorCode::kRootNotSingular, "0x0");
	TEST_ERROR(ParseErrorCode::kRootNotSingular, "0x123");
}

static void test_parse_number_too_big()
{
	TEST_ERROR(ParseErrorCode::kNumberTooBig, "1e309");
	TEST_ERROR(ParseErrorCode::kNumberTooBig, "-1e309");
}

static void test_parse_missing_quotation_mark()
{
	TEST_ERROR(ParseErrorCode::kMissQuotationMark, "\"");
	TEST_ERROR(ParseErrorCode::kMissQuotationMark, "\"abc");
}

static void test_parse_invalid_string_escape()
{
#if 1
	TEST_ERROR(ParseErrorCode::kInvalidStringEscape, "\"\\v\"");
	TEST_ERROR(ParseErrorCode::kInvalidStringEscape, "\"\\'\"");
	TEST_ERROR(ParseErrorCode::kInvalidStringEscape, "\"\\0\"");
	TEST_ERROR(ParseErrorCode::kInvalidStringEscape, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char()
{
#if 1
	TEST_ERROR(ParseErrorCode::kInvalidStringChar, "\"\x01\"");
	TEST_ERROR(ParseErrorCode::kInvalidStringChar, "\"\x1F\"");
#endif
}

static void test_parse_invalid_unicode_hex()
{
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u0\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u01\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u012\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u/000\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\uG000\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u0/00\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u0G00\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u0/00\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u00G0\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u000/\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeHex, "\"\\u000G\"");
}

static void test_parse_invalid_unicode_surrogate()
{
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeSurrogate, "\"\\uD800\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeSurrogate, "\"\\uDBFF\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeSurrogate, "\"\\uD800\\\\\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeSurrogate, "\"\\uD800\\uDBFF\"");
	TEST_ERROR(ParseErrorCode::kInvalidUnicodeSurrogate, "\"\\uD800\\uE000\"");
}

static void test_parse_array()
{
	Json test;
    JsonElem result = test.Parse("[ ]");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kArray, result.type());
	EXPECT_EQ_SIZE_T(0, result.ToArray().size());
    
    result = test.Parse("[ null , false , true , 123 , \"abc\" ]");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kArray, result.type());
	EXPECT_EQ_SIZE_T(5, result.ToArray().size());
	
	EXPECT_EQ_INT(JsonType::kNull, result.ToArray()[0].type());
	EXPECT_EQ_INT(JsonType::kFalse, result.ToArray()[1].type());
	EXPECT_EQ_INT(JsonType::kTrue, result.ToArray()[2].type());
	EXPECT_EQ_INT(JsonType::kNumber, result.ToArray()[3].type());
	EXPECT_EQ_INT(JsonType::kString, result.ToArray()[4].type());
	EXPECT_EQ_DOUBLE(123.0, result.ToArray()[3].ToNumber());
	EXPECT_EQ_STRING("abc", result.ToArray()[4].ToString().c_str(), result.ToArray()[4].ToString().length());
	
    result = test.Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
	EXPECT_EQ_INT(ParseErrorCode::kOK, test.GetErrorCode());
	EXPECT_EQ_INT(JsonType::kArray, result.type());
	EXPECT_EQ_SIZE_T(4, result.ToArray().size());

    for (size_t i = 0; i < 4; i++) 
    {
        JsonElem e = result.ToArray()[i];
        EXPECT_EQ_INT(JsonType::kArray, e.type());
        EXPECT_EQ_SIZE_T(i, e.ToArray().size());
        for (size_t j = 0; j < i; j++) {
            JsonElem ee = e.ToArray()[j];
            EXPECT_EQ_INT(JsonType::kNumber, ee.type());
            EXPECT_EQ_DOUBLE((double)j, ee.ToNumber());
        }
    }
    
}

static void test_parse_miss_comma_or_square_bracket()
{
#if 1
	TEST_ERROR(ParseErrorCode::kMissCommaOrSquareBracket, "[1");
	TEST_ERROR(ParseErrorCode::kMissCommaOrSquareBracket, "[1}");
	TEST_ERROR(ParseErrorCode::kMissCommaOrSquareBracket, "[1 2");
	TEST_ERROR(ParseErrorCode::kMissCommaOrSquareBracket, "[[]");
#endif
}

static void test_access_null()
{
	JsonElem e;
	e.SetString("a");
	e.SetNull();
	EXPECT_EQ_INT(JsonType::kNull, e.type());
}

static void test_access_boolean()
{
    JsonElem e;
	e.SetString("a");
	e.SetBoolean(1);
	EXPECT_TRUE(e.ToBoolean());
    e.SetBoolean(0);
	EXPECT_FALSE(e.ToBoolean());
}

static void test_access_number()
{
    JsonElem e;
    e.SetString("a");
	e.SetNumber(1234.5);
	EXPECT_EQ_DOUBLE(1234.5, e.ToNumber());
}

static void test_access_string()
{
    JsonElem e;
	e.SetString("");
	EXPECT_EQ_STRING("", e.ToString().c_str(), e.ToString().length());
	e.SetString("Hello");
	EXPECT_EQ_STRING("Hello", e.ToString().c_str(), e.ToString().length());
}

static void test_parse()
{
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_string();
	test_parse_array();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
	test_parse_invalid_unicode_hex();
	test_parse_invalid_unicode_surrogate();
	test_parse_miss_comma_or_square_bracket();
}

static void test_access()
{
	test_access_null();
	test_access_boolean();
	test_access_number();
	test_access_string();
}

int main()
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	test_parse();
	test_access();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
