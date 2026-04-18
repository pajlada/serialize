#include <gtest/gtest.h>

#include <pajlada/serialize.hpp>

using namespace pajlada;
using namespace std;

struct Complex {
    int a;
    int b;
    std::string c;

    bool operator==(const Complex &other) const = default;
};

namespace pajlada {

template <typename RJValue>
struct Serialize<Complex, RJValue> {
    static RJValue
    get(const Complex &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kObjectType);

        detail::AddMember<int, RJValue>(ret, "a", value.a, a);
        detail::AddMember<int, RJValue>(ret, "b", value.b, a);
        detail::AddMember<std::string, RJValue>(ret, "c", value.c, a);

        return ret;
    }
};

template <typename RJValue>
struct Deserialize<Complex, RJValue> {
    static Complex
    get(const RJValue &value, bool *error = nullptr)
    {
        // assert(false);
        Complex ret;

        std::cout << "complex deserialize attempt\n";

        if (!value.IsObject()) {
            std::cout << "no object\n";
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        const auto &obj = value.GetObject();

        if (obj.HasMember("a")) {
            std::cout << "before:" << ret.a << "\n";
            ret.a = Deserialize<int>::get(obj["a"], error);
            std::cout << "after:" << ret.a << "\n";
            if (error != nullptr && *error) {
                std::cout << "a bad\n";
                return {};
            }
        }

        if (obj.HasMember("b")) {
            ret.b = Deserialize<int>::get(obj["b"], error);
            if (error != nullptr && *error) {
                std::cout << "b bad\n";
                return {};
            }
        }

        if (obj.HasMember("c")) {
            ret.c = Deserialize<std::string>::get(obj["c"], error);
            if (error != nullptr && *error) {
                std::cout << "c bad\n";
                return {};
            }
        }

        std::cout << "all good?" << ret.a << "\n";
        return ret;
    }
};

}  // namespace pajlada

TEST(Variant, Two)
{
    bool error;
    rapidjson::Document d;
    rapidjson::Value actual;

    std::variant<std::string, int> in;
    std::variant<std::string, int> out;

    in = "";
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    out = Deserialize<std::variant<std::string, int>>::get(actual, &error);
    ASSERT_EQ(in, out);
    ASSERT_FALSE(error);

    in = "forsen";
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    out = Deserialize<std::variant<std::string, int>>::get(actual, &error);
    ASSERT_EQ(in, out);
    ASSERT_FALSE(error);

    in = 69;
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    out = Deserialize<std::variant<std::string, int>>::get(actual, &error);
    ASSERT_EQ(in, out);
    auto xd = std::variant<std::string, int>(69);
    ASSERT_EQ(out, xd);
    ASSERT_FALSE(error);

    actual = Serialize<std::vector<std::string>>::get({"foo", "bar"},
                                                      d.GetAllocator());
    out = Deserialize<std::variant<std::string, int>>::get(actual, &error);
    ASSERT_TRUE(error);
}

TEST(Variant, Three)
{
    rapidjson::Document d;
    rapidjson::Value actual;

    std::variant<std::string, double, int> in;

    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value(""), actual);

    in = "";
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value(""), actual);

    in = "forsen";
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value("forsen"), actual);

    in = 69;
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value(69), actual);

    in = 42.5;
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value(42.5), actual);

    bool error;
    auto out = Deserialize<std::variant<std::string, double, int>>::get(actual,
                                                                        &error);

    ASSERT_EQ(out, in);
}

void
AssertMatch(std::string_view expected, const rapidjson::Value &v)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    v.Accept(writer);
    std::string actual(buffer.GetString());

    ASSERT_EQ(expected, actual);
}

TEST(Variant, Complex)
{
    rapidjson::Document d;
    rapidjson::Value actual;

    std::variant<std::string, int, Complex> in;

    in = "forsen";
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value("forsen"), actual);

    in = 5;
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    ASSERT_EQ(rapidjson::Value(5), actual);

    in = Complex{
        .a = 3,
        .b = 7,
        .c = "peppah",
    };
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    AssertMatch(R"({
    "a": 3,
    "b": 7,
    "c": "peppah"
})",
                actual);

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(actual, &error);
    ASSERT_ANY_THROW(std::get<std::string>(out));
    auto outComplex = std::get<Complex>(out);
    auto expectedComplex = Complex{
        .a = 3,
        .b = 7,
        .c = "peppah",
    };
    ASSERT_EQ(expectedComplex, outComplex);
}

/**
 * MessageHighlight and UserHighlight are internally typed.
 * Both types expect an object with a "type" key.
 */
struct MessageHighlight {
    std::string pattern;
};

struct UserHighlight {
    std::string username;
};

namespace pajlada {

template <typename RJValue>
struct Deserialize<MessageHighlight, RJValue> {
    static MessageHighlight
    get(const RJValue &value, bool *error = nullptr)
    {
        MessageHighlight ret;

        if (!value.IsObject()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        const auto &obj = value.GetObject();

        auto type = obj.FindMember("type");
        if (type == obj.MemberEnd()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        if (!type.IsString() || type.GetString() != "message") {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        auto pattern = obj.FindMember("pattern");
        if (pattern == obj.MemberEnd()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        if (!pattern.IsString()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        ret.pattern = pattern.GetString();

        return ret;
    }
};

template <typename RJValue>
struct Serialize<MessageHighlight, RJValue> {
    static RJValue
    get(const MessageHighlight &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kObjectType);

        detail::AddMember<std::string, RJValue>(ret, "type", "message", a);
        detail::AddMember<std::string, RJValue>(ret, "pattern", value.pattern,
                                                a);

        return ret;
    }
};

template <typename RJValue>
struct Deserialize<UserHighlight, RJValue> {
    static UserHighlight
    get(const RJValue &value, bool *error = nullptr)
    {
        UserHighlight ret;

        if (!value.IsObject()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        const auto &obj = value.GetObject();

        auto type = obj.FindMember("type");
        if (type == obj.MemberEnd()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        if (!type.IsString() || type.GetString() != "user") {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        auto pattern = obj.FindMember("pattern");
        if (pattern == obj.MemberEnd()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        if (!pattern.IsString()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        ret.username = pattern.GetString();

        return ret;
    }
};

template <typename RJValue>
struct Serialize<UserHighlight, RJValue> {
    static RJValue
    get(const UserHighlight &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kObjectType);

        detail::AddMember<std::string, RJValue>(ret, "type", "user", a);
        detail::AddMember<std::string, RJValue>(ret, "username", value.username,
                                                a);

        return ret;
    }
};

}  // namespace pajlada

TEST(Variant, Highlights)
{
    rapidjson::Document d;
    rapidjson::Value actual;

    std::variant<MessageHighlight, UserHighlight> in;

    in = MessageHighlight{
        .pattern = "forsen",
    };
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    AssertMatch(R"({
    "type": "message",
    "pattern": "forsen"
})",
                actual);

    in = UserHighlight{
        .username = "peppah",
    };
    actual = Serialize<decltype(in)>::get(in, d.GetAllocator());
    AssertMatch(R"({
    "type": "user",
    "username": "peppah"
})",
                actual);
}
