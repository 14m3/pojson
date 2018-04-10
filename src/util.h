#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace polojson
{
    class JsonElem;
    class JsonValue;

    using array_t = std::vector<JsonElem>;
    using object_t = std::unordered_map<std::string, JsonElem>;

    enum class JsonType
    {
        kNull,
        kTrue,
        kFalse,
        kNumber,
        kString,
        kArray,
        kObject
    };

    enum class ParseErrorCode
    {
        kOK = 0,
        kExpectValue,
        kInvalidValue,
        kRootNotSingular,
        kNumberTooBig,
        kMissQuotationMark,
        kInvalidStringEscape,
        kInvalidStringChar,
        kInvalidUnicodeHex,
        kInvalidUnicodeSurrogate,
        kMissCommaOrSquareBracket,
        kMissKey,
        kMissColon,
        kMissCommaOrCurlyBracket,
        kUnknown
    };

    class JsonElem
    {
    public:
        JsonElem() { value_ = nullptr; }
        JsonElem(const JsonElem&); //copy constructor
        JsonElem(JsonElem&&) noexcept;
        ~JsonElem();
        JsonElem& operator=(JsonElem);

        explicit JsonElem(std::nullptr_t);      // null
        explicit JsonElem(bool);                // true or false
        explicit JsonElem(double);              // number
        explicit JsonElem(const std::string&);  // string
        explicit JsonElem(const array_t&);      // array
        explicit JsonElem(const object_t&);     // object

        JsonType type() const noexcept;

        bool IsNull() const;
        bool IsBoolean() const;
        bool IsNumber() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsObject() const;

        void SetNull();
        void SetBoolean(bool);
        void SetNumber(double);
        void SetString(const std::string&);
        //void SetArray();

        bool ToBoolean() const;
        double ToNumber() const;
        const std::string& ToString() const;
        const array_t& ToArray() const;
        const object_t& ToObject() const;

        size_t size() const;

    private:
        std::unique_ptr<JsonValue> value_;
    };

    class JsonValue
    {
    public:
        virtual ~JsonValue() = default;
        virtual JsonType type() const = 0;
        
        virtual double ToNumber() const
        {
            throw std::runtime_error("Not a JsonNumber object");
        }

        virtual const std::string& ToString() const
        {
            throw std::runtime_error("Not a JsonString object");
        }

        virtual const array_t& toArray() const
        {
            throw std::runtime_error("Not a JsonArray object");
        }

        virtual const object_t& toObject() const
        {
            throw std::runtime_error("Not a JsonObject object");
        }

        virtual JsonElem& operator[](size_t)
        {
            throw std::runtime_error("Not a JsonArray object");
        }

        virtual const JsonElem& operator[](size_t) const
        {
            throw std::runtime_error("Not a JsonArray object");
        }
        
    };

    template<typename T, JsonType E>
    class JsonValueExt :public JsonValue
    {
    protected:
        T value_;
    public:
        JsonValueExt(const T& value) :value_(value) {}
        JsonValueExt(T&& value) :value_(value) {} //move constructor
        JsonType type() const { return E; }
    };

  
    class JsonNull :public JsonValueExt<std::nullptr_t, JsonType::kNull>
    {
    public:
        explicit JsonNull(std::nullptr_t) :JsonValueExt(nullptr) {};
    };

    class JsonTrue :public JsonValueExt<std::nullptr_t, JsonType::kTrue>
    {
    public:
        explicit JsonTrue(std::nullptr_t) :JsonValueExt(nullptr) {}
    };

    class JsonFalse :public JsonValueExt<std::nullptr_t, JsonType::kFalse>
    {
    public:
        explicit JsonFalse(std::nullptr_t) :JsonValueExt(nullptr) {}
    };

    class JsonNumber :public JsonValueExt<double, JsonType::kNumber>
    {
    public:
        explicit JsonNumber(double value) :JsonValueExt(value) {}
        double ToNumber() const override { return value_; }
    };

    class JsonString :public JsonValueExt<std::string, JsonType::kString>
    {
    public:
        explicit JsonString(std::string value) :JsonValueExt(value) {}
        explicit JsonString(std::string&& value) :
            JsonValueExt(std::move(value)) {} //move constructor
        const std::string& ToString() const override { return value_; }
    };

    class JsonArray :public JsonValueExt<array_t, JsonType::kArray>
    {
    public:
        explicit JsonArray(const array_t& val) : JsonValueExt(val) {}
        explicit JsonArray(array_t&& val) : JsonValueExt(std::move(val)) {}

        const array_t& toArray() const override { return value_; }
        /*
        JsonElem& operator[](size_t i) override
        {
            return value_[i];
        }

        const JsonElem& operator[](size_t i) const override
        {
            return value_[i];
        }*/
    };
    
    class JsonObject :public JsonValueExt<object_t, JsonType::kObject>
    {
    public:
        explicit JsonObject(const object_t& val) : JsonValueExt(val) {}
        explicit JsonObject(object_t&& val) : JsonValueExt(std::move(val)) {}

        const object_t& toObject() const override { return value_; }
        /*
        JsonElem& operator[](std::string key) override
        {
            return value_[i];
        }

        const JsonElem& operator[](std::string key) const override
        {
            return value_[i];
        }
        */
    };
}