#include <gtest/gtest.h>
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
