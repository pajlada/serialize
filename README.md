# PajladaSerialize

This library provides a serialize & deserialize interface based on the [RapidJSON](https://rapidjson.org/) library, allowing for serializing & deserializing of custom types.

## Example usage

```c++
// serialize the int with value 5 into a rpaidjson::Value in the `d` document
int value = 5;
rapidjson::Document d;
auto middle = pajlada::Serialize<int>::get(value, d.GetAllocator());

// middle is now a rapidjson::Value you can use in your own code

// or you can deserialize it into an int again
bool error = false;
auto out = pajlada::Deserialize<int>::get(middle, &error);
assert(!error);
assert(error == 5);
```
