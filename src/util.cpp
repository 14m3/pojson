#include "util.h"
using namespace polojson;

polojson::JsonElem::JsonElem(const JsonElem& e)
{
    switch (e.type())
    {
    case JsonType::kNull:
        value_ = std::make_unique<JsonNull>(nullptr);
        break;
    case JsonType::kFalse:
        value_ = std::make_unique<JsonFalse>(nullptr);
        break;
    case JsonType::kTrue:
        value_ = std::make_unique<JsonTrue>(nullptr);
        break;
    case JsonType::kNumber:
        value_ = std::make_unique<JsonNumber>(e.ToNumber());
        break;
    case JsonType::kString:
        value_ = std::make_unique<JsonString>(e.ToString());
        break;
    case JsonType::kArray:
        value_ = std::make_unique<JsonArray>(e.ToArray());
        break;
    default:
        value_ = std::make_unique<JsonNull>(nullptr);
        break;
    }
}

polojson::JsonElem::JsonElem(JsonElem&& e) noexcept
{
    value_ = std::move(e.value_);
    e.value_.reset(nullptr);
}

polojson::JsonElem::~JsonElem() {}

JsonElem& polojson::JsonElem::operator=(JsonElem other)
{
    std::swap(value_, other.value_);
    return *this;
}

polojson::JsonElem::JsonElem(std::nullptr_t) :
    value_(std::make_unique<JsonNull>(nullptr)) {}

polojson::JsonElem::JsonElem(bool val)
{
    if (val)
        value_ = std::make_unique<JsonTrue>(nullptr);
    else
        value_ = std::make_unique<JsonFalse>(nullptr);
}

polojson::JsonElem::JsonElem(double val) :
    value_(std::make_unique<JsonNumber>(val)) {}

polojson::JsonElem::JsonElem(const std::string& val) :
    value_(std::make_unique<JsonString>(val)) {}

polojson::JsonElem::JsonElem(const array_t& val) :
    value_(std::make_unique<JsonArray>(val)) {}

JsonType polojson::JsonElem::type() const noexcept
{
    return value_->type();
}

bool polojson::JsonElem::IsNull() const
{
    return type() == JsonType::kNull;
}

bool polojson::JsonElem::IsBoolean() const
{
    return (type() == JsonType::kTrue) ||
        (type() == JsonType::kFalse);
}

bool polojson::JsonElem::IsNumber() const
{
    return type() == JsonType::kNumber;
}

bool polojson::JsonElem::IsString() const
{
    return type() == JsonType::kString;
}

bool polojson::JsonElem::IsArray() const
{
    return type() == JsonType::kArray;
}

void polojson::JsonElem::SetNull()
{
    value_.reset(nullptr);
    value_ = std::make_unique<JsonNull>(nullptr);
}

void polojson::JsonElem::SetBoolean(bool val)
{
    value_.reset(nullptr);
    if (val)
        value_ = std::make_unique<JsonTrue>(nullptr);
    else
        value_ = std::make_unique<JsonFalse>(nullptr);
}


void polojson::JsonElem::SetNumber(double val)
{
    value_.reset(nullptr);
    value_ = std::make_unique<JsonNumber>(val);
}

void polojson::JsonElem::SetString(const std::string& val)
{
    value_.reset(nullptr);
    value_ = std::make_unique<JsonString>(val);
}

bool polojson::JsonElem::ToBoolean() const
{
    assert(IsBoolean());
    return type() == JsonType::kTrue;
}

double polojson::JsonElem::ToNumber() const
{
    assert(IsNumber());
    return value_->ToNumber();;
}

const std::string & polojson::JsonElem::ToString() const
{
    assert(IsString());
    return value_->ToString();
}

const array_t & polojson::JsonElem::ToArray() const
{
    assert(IsArray());
    return value_->toArray();
}
