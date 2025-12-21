#include <rapidjson/allocators.h>
#include <rapidjson/document.h>

#include <iostream>
#include <pajlada/serialize.hpp>

int
main(int argc, char **argv)
{
    int value = 5;
    rapidjson::Document d;
    auto middle = pajlada::Serialize<int>::get(value, d.GetAllocator());

    bool error = false;
    auto out = pajlada::Deserialize<int>::get(middle, &error);
    std::cout << "test: " << value << " == " << out << '\n';
    return 0;
}
