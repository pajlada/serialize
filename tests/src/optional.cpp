#include <gtest/gtest.h>

#include <optional>
#include <pajlada/serialize.hpp>
#include <string>

using namespace pajlada;

TEST(Optional, SerializeString)
{
    rapidjson::Document d;
    rapidjson::Value actual;

    std::optional<std::string> v;

    v = std::nullopt;
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_TRUE(actual.IsNull());

    v = "";
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_FALSE(actual.IsNull());
    ASSERT_TRUE(actual.IsString());
    ASSERT_EQ(std::string(actual.GetString()), "");

    v = "forsen";
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_FALSE(actual.IsNull());
    ASSERT_TRUE(actual.IsString());
    ASSERT_EQ(std::string(actual.GetString()), "forsen");
}

TEST(Optional, SerializeInt)
{
    rapidjson::Document d;
    rapidjson::Value actual;

    std::optional<int> v;

    v = std::nullopt;
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_TRUE(actual.IsNull());

    v = 0;
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_FALSE(actual.IsNull());
    ASSERT_TRUE(actual.IsNumber());
    ASSERT_EQ(actual.GetInt(), 0);

    v = 69;
    actual = Serialize<decltype(v)>::get(v, d.GetAllocator());
    ASSERT_FALSE(actual.IsNull());
    ASSERT_TRUE(actual.IsNumber());
    ASSERT_EQ(actual.GetInt(), 69);
}

TEST(Optional, DeserializeString)
{
    bool error;
    rapidjson::Document d;
    rapidjson::Value in;
    std::optional<std::string> out;

    error = false;
    in = rapidjson::kNullType;
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_FALSE(out.has_value());

    error = false;
    in = rapidjson::Value("");
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(out.has_value());
    ASSERT_EQ(out.value(), "");

    error = false;
    in = rapidjson::Value("forsen");
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(out.has_value());
    ASSERT_EQ(out.value(), "forsen");
}

TEST(Optional, DeserializeInt)
{
    bool error;
    rapidjson::Document d;
    rapidjson::Value in;
    std::optional<int> out;

    error = false;
    in = rapidjson::kNullType;
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_FALSE(out.has_value());

    error = false;
    in = rapidjson::Value(0);
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(out.has_value());
    ASSERT_EQ(out.value(), 0);

    error = false;
    in = rapidjson::Value(69);
    out = Deserialize<decltype(out)>::get(in, &error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(out.has_value());
    ASSERT_EQ(out.value(), 69);
}
