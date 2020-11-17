#include <gtest/gtest.h>

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
