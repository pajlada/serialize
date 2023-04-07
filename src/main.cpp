#include <gtest/gtest.h>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>

#include <array>
#include <pajlada/serialize.hpp>

using namespace pajlada;
using namespace std;

// TODO(pajlada): Make tests that try to put connections/signals in various STL containers

TEST(Serialize, SerializeArrayDefault)
{
    std::array<int, 2> in;

    rapidjson::Document d;
    auto middle = Serialize<std::array<int, 2>>::get(in, d.GetAllocator());

    bool error = false;
    auto out = Deserialize<std::array<int, 2>>::get(middle, &error);
    EXPECT_FALSE(false);
    EXPECT_EQ(in, out);
}

TEST(Serialize, SerializeArray)
{
    std::array<int, 2> in{1, 2};

    rapidjson::Document d;
    auto middle = Serialize<std::array<int, 2>>::get(in, d.GetAllocator());

    bool error = false;
    auto out = Deserialize<std::array<int, 2>>::get(middle, &error);
    EXPECT_FALSE(error);
    EXPECT_EQ(1, out[0]);
    EXPECT_EQ(2, out[1]);
}

TEST(Serialize, SerializeArrayMismatchingSize)
{
    std::array<int, 2> in{1, 2};

    rapidjson::Document d;
    auto middle = Serialize<std::array<int, 2>>::get(in, d.GetAllocator());

    bool error = false;
    auto out = Deserialize<std::array<int, 3>>::get(middle, &error);
    EXPECT_TRUE(error);
    EXPECT_EQ(out, (std::array<int, 3>{0, 0, 0}));
}

TEST(Serialize, Float)
{
    float in = 3.7f;

    rapidjson::Document d;
    auto middle = Serialize<float>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    bool error = false;
    auto out = Deserialize<float>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_FLOAT_EQ(in, out);
}

TEST(Serialize, FloatNaN)
{
    // Infinity should serialize to NULL
    float in = std::numeric_limits<float>::infinity();

    rapidjson::Document d;
    auto middle = Serialize<float>::get(in, d.GetAllocator());

    EXPECT_FALSE(middle.IsNumber());
    EXPECT_TRUE(middle.IsNull());

    // NULL should deserialize to NaN
    bool error = false;
    auto out = Deserialize<float>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_TRUE(std::isnan(out));
}

TEST(Serialize, FloatString)
{
    std::string in = "3.7";

    rapidjson::Document d;
    auto middle = Serialize<std::string>::get(in, d.GetAllocator());

    EXPECT_FALSE(middle.IsNumber());
    EXPECT_TRUE(middle.IsString());

    bool error = false;
    auto out = Deserialize<float>::get(middle, &error);
    EXPECT_TRUE(error);
    EXPECT_FLOAT_EQ(out, 0.0f);
}

TEST(Deserialize, StringToStringView)
{
    std::string in = "forsen";

    rapidjson::Document d;
    auto middle = Serialize<std::string>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsString());

    bool error = false;
    auto out = Deserialize<std::string_view>::get(middle, &error);
    EXPECT_FALSE(error);
    EXPECT_EQ(out, "forsen");
}

TEST(Serialize, StringViewToString)
{
    std::string_view in = "forsen";

    rapidjson::Document d;
    auto middle = Serialize<std::string_view>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsString());

    bool error = false;
    auto out = Deserialize<std::string>::get(middle, &error);
    EXPECT_FALSE(error);
    EXPECT_EQ(out, "forsen");
}

TEST(Serialize, StringViewToStringView)
{
    std::string_view in = "forsen";

    rapidjson::Document d;
    auto middle = Serialize<std::string_view>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsString());

    bool error = false;
    auto out = Deserialize<std::string_view>::get(middle, &error);
    EXPECT_FALSE(error);
    EXPECT_EQ(out, "forsen");
}

TEST(Serialize, EmptyStringView)
{
    std::string_view in;

    rapidjson::Document d;
    auto middle = Serialize<std::string_view>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsString());

    bool error = false;
    auto out = Deserialize<std::string_view>::get(middle, &error);
    EXPECT_FALSE(error);
    EXPECT_EQ(out, "");
}
