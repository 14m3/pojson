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
    case JsonType::kObject:
        value_ = std::make_unique<JsonObject>(e.ToObject());
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

polojson::JsonElem::JsonElem(const object_t& val):
    value_(std::make_unique<JsonObject>(val)) {}

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

bool polojson::JsonElem::IsObject() const
{
    return type() == JsonType::kObject;
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
    return value_->ToArray();
}

const object_t & polojson::JsonElem::ToObject() const
{
    assert(IsObject());
    return value_->ToObject();
}

std::string polojson::JsonElem::Stringify() const
{
    switch (value_->type())
    {
    case JsonType::kNull:
        return "null";
    case JsonType::kTrue:
        return "true";
    case JsonType::kFalse:
        return "false";
    case JsonType::kNumber:
    {
        char buffer[32];
        int length = sprintf_s(buffer, "%.17g", value_->ToNumber());
        return buffer;
    }
    case JsonType::kString:
        return StringifyString();
    case JsonType::kArray:
        return StringfyArray();
    case JsonType::kObject:
        return StringfyObject();
    default:
        break;
    }
    throw std::runtime_error("invalid type");
}

std::string polojson::JsonElem::StringifyString() const
{
    static const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::string ret = "\"";
    for (auto e : value_->ToString())
    {
        switch (e)
        {
        case '\"': ret += "\\\""; break;
        case '\\': ret += "\\\\"; break;
        case '\b': ret += "\\b"; break;
        case '\f': ret += "\\f"; break;
        case '\n': ret += "\\n"; break;
        case '\r': ret += "\\r"; break;
        case '\t': ret += "\\t"; break;
        default:
            unsigned char ch = static_cast<unsigned char>(e);
            if (ch < 0x20)
            {
                ret += "\\u00";
                ret += hex_digits[ch >> 4];
                ret += hex_digits[ch & 0b1111];
            }
            else
                ret += e;
        }
    }
    return ret + '"';
}

std::string polojson::JsonElem::StringfyArray() const
{
    std::string ret = "[";
    for (size_t i = 0; i < value_->ToArray().size(); ++i)
    {
        if (i > 0)
            ret += ",";
        ret += value_->ToArray()[i].Stringify();
    }
    ret += "]";
    return ret;
}

std::string polojson::JsonElem::StringfyObject() const
{
    std::string ret = "{";
    for (auto i = value_->ToObject().begin(); i != value_->ToObject().end();
        ++i)
    {
        if(i!=value_->ToObject().begin())
            ret += ",";
        ret += "\"" + i->first + "\"";
        ret += ":" + i->second.Stringify();
    }
    ret += "}";
    return ret;
}
