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

TEST(Serialize, SimpleAnyMap)
{
    using std::any_cast;

    std::map<std::string, std::any> test{{"a", 1}, {"b", "asd"}, {"c", 3.14}};

    rapidjson::Document d;
    auto middle = Serialize<decltype(test)>::get(test, d.GetAllocator());

    EXPECT_TRUE(middle.IsObject());

    bool error = false;
    auto out = Deserialize<decltype(test)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_TRUE(out.size() == 3);
    EXPECT_TRUE(any_cast<int>(out["a"]) == 1);
    EXPECT_TRUE(any_cast<std::string>(out["b"]) == "asd");
    EXPECT_DOUBLE_EQ(any_cast<double>(out["c"]), 3.14);
}

TEST(Serialize, ComplexAnyMap)
{
    using std::any_cast;
    using AnyMap = std::map<std::string, std::any>;
    using AnyVector = std::vector<std::any>;

    AnyMap test{
        {
            "a",
            5,
        },
        {
            "innerMap",
            AnyMap{
                {"a", 420},
                {"b", 320},
                {"c", 13.37},
            },
        },
        {
            "innerArray",
            AnyVector{
                1,
                2,
                3,
                4,
                "testman",
                true,
                false,
                4.20,
                AnyMap{
                    {"a", 1},
                    {"b", 2},
                    {"c", 3},
                },
            },
        },
    };

    rapidjson::Document d;
    auto middle = Serialize<decltype(test)>::get(test, d.GetAllocator());

    EXPECT_TRUE(middle.IsObject());

    bool error = false;
    auto out = Deserialize<decltype(test)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_TRUE(out.size() == 3);
    EXPECT_TRUE(any_cast<int>(out["a"]) == 5);

    auto innerMap = any_cast<std::map<std::string, std::any>>(out["innerMap"]);
    EXPECT_TRUE(innerMap.size() == 3);
    EXPECT_TRUE(any_cast<int>(innerMap["a"]) == 420);
    EXPECT_TRUE(any_cast<int>(innerMap["b"]) == 320);
    EXPECT_DOUBLE_EQ(any_cast<double>(innerMap["c"]), 13.37);

    auto innerArray = any_cast<std::vector<std::any>>(out["innerArray"]);
    EXPECT_TRUE(innerArray.size() == 9);
    EXPECT_TRUE(any_cast<int>(innerArray[0]) == 1);
    EXPECT_TRUE(any_cast<int>(innerArray[1]) == 2);
    EXPECT_TRUE(any_cast<int>(innerArray[2]) == 3);
    EXPECT_TRUE(any_cast<int>(innerArray[3]) == 4);
    EXPECT_TRUE(any_cast<std::string>(innerArray[4]) == "testman");
    EXPECT_TRUE(any_cast<bool>(innerArray[5]) == true);
    EXPECT_TRUE(any_cast<bool>(innerArray[6]) == false);
    EXPECT_DOUBLE_EQ(any_cast<double>(innerArray[7]), 4.20);

    auto innerArrayMap =
        any_cast<std::map<std::string, std::any>>(innerArray[8]);
    EXPECT_TRUE(innerArrayMap.size() == 3);
    EXPECT_TRUE(any_cast<int>(innerArrayMap["a"]) == 1);
    EXPECT_TRUE(any_cast<int>(innerArrayMap["b"]) == 2);
    EXPECT_TRUE(any_cast<int>(innerArrayMap["c"]) == 3);
}

TEST(Serialize, Int)
{
    int in = 37;

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_TRUE(
        middle
            .IsUint());  // The JSON number type represents all integral values the same, so all of these are expected to be true assuming the value could potentially fit
    EXPECT_TRUE(
        middle
            .IsUint64());  // The JSON number type represents all integral values the same, so all of these are expected to be true assuming the value could potentially fit
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, NegativeInt)
{
    int in = -37;

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_FALSE(middle.IsUint());
    EXPECT_FALSE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Int64Max)
{
    int64_t in = std::numeric_limits<decltype(in)>::max();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_FALSE(middle.IsInt());
    EXPECT_FALSE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Int64Min)
{
    int64_t in = std::numeric_limits<decltype(in)>::min();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_FALSE(middle.IsInt());
    EXPECT_FALSE(middle.IsUint());
    EXPECT_FALSE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Int32Max)
{
    int32_t in = std::numeric_limits<decltype(in)>::max();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_TRUE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Int32Min)
{
    int32_t in = std::numeric_limits<decltype(in)>::min();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_FALSE(middle.IsUint());
    EXPECT_FALSE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Uint32Max)
{
    uint32_t in = std::numeric_limits<decltype(in)>::max();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_FALSE(middle.IsInt());
    EXPECT_TRUE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Uint32Min)
{
    uint32_t in = std::numeric_limits<decltype(in)>::min();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_TRUE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Uint64Max)
{
    uint64_t in = std::numeric_limits<decltype(in)>::max();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_FALSE(middle.IsInt());
    EXPECT_FALSE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_FALSE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}

TEST(Serialize, Uint64Min)
{
    uint64_t in = std::numeric_limits<decltype(in)>::min();

    rapidjson::Document d;
    auto middle = Serialize<decltype(in)>::get(in, d.GetAllocator());

    EXPECT_TRUE(middle.IsNumber());

    EXPECT_TRUE(middle.IsInt());
    EXPECT_TRUE(middle.IsUint());
    EXPECT_TRUE(middle.IsUint64());
    EXPECT_TRUE(middle.IsInt64());

    EXPECT_FALSE(middle.IsFloat());
    EXPECT_FALSE(middle.IsDouble());

    bool error = false;
    auto out = Deserialize<decltype(in)>::get(middle, &error);
    EXPECT_FALSE(error);

    EXPECT_EQ(typeid(in), typeid(out));
    EXPECT_EQ(in, out);
}
