#pragma once

#include <rapidjson/document.h>

#include <any>
#include <cassert>
#include <cmath>
#include <map>
#include <pajlada/serialize/common.hpp>
#include <stdexcept>
#include <typeinfo>
#include <vector>

namespace pajlada {

namespace detail {

template <typename Type, typename RJValue>
inline void AddMember(RJValue &object, const char *key, const Type &value,
                      typename RJValue::AllocatorType &a);

template <typename Type, typename RJValue>
inline void PushBack(RJValue &array, const Type &value,
                     typename RJValue::AllocatorType &a);

}  // namespace detail

// Serialize is called when a settings value is being saved

// Create a rapidjson::Value from the templated value
template <typename Type, typename RJValue = rapidjson::Value>
struct Serialize {
    static RJValue
    get(const Type &value, typename RJValue::AllocatorType &)
    {
        RJValue ret(value);

        return ret;
    }
};

template <typename RJValue>
struct Serialize<float, RJValue> {
    static RJValue
    get(const float &value, typename RJValue::AllocatorType &)
    {
        if (std::isnan(value) || std::isinf(value)) {
            return RJValue{rapidjson::kNullType};
        }

        RJValue ret(value);
        return ret;
    }
};

template <typename RJValue>
struct Serialize<double, RJValue> {
    static RJValue
    get(const double &value, typename RJValue::AllocatorType &)
    {
        if (std::isnan(value) || std::isinf(value)) {
            return RJValue{rapidjson::kNullType};
        }

        RJValue ret(value);
        return ret;
    }
};

template <typename RJValue>
struct Serialize<std::string, RJValue> {
    static RJValue
    get(const std::string &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(value.c_str(), a);

        return ret;
    }
};

template <typename RJValue>
struct Serialize<std::string_view, RJValue> {
    static RJValue
    get(const std::string_view &value, typename RJValue::AllocatorType &a)
    {
        // Workaround for older rapidjson versions:
        // GenericStringRef and nullpointers (empty strings) wasn't supported.
        const auto *data = value.data();
        rapidjson::GenericStringRef<char> ref(
            data ? data : "", static_cast<rapidjson::SizeType>(value.size()));
        RJValue ret(ref, a);

        return ret;
    }
};

template <typename Arg1, typename Arg2, typename RJValue>
struct Serialize<std::pair<Arg1, Arg2>, RJValue> {
    static RJValue
    get(const std::pair<Arg1, Arg2> &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kArrayType);

        ret.PushBack(Serialize<Arg1, RJValue>::get(value.first, a), a);
        ret.PushBack(Serialize<Arg2, RJValue>::get(value.second, a), a);

        return ret;
    }
};

template <typename ValueType, typename RJValue>
struct Serialize<std::map<std::string, ValueType>, RJValue> {
    static RJValue
    get(const std::map<std::string, ValueType> &value,
        typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kObjectType);

        for (auto it = value.begin(); it != value.end(); ++it) {
            detail::AddMember<ValueType, RJValue>(ret, it->first.c_str(),
                                                  it->second, a);
        }

        return ret;
    }
};

template <typename ValueType, typename RJValue>
struct Serialize<std::vector<ValueType>, RJValue> {
    static RJValue
    get(const std::vector<ValueType> &value, typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kArrayType);

        for (const auto &innerValue : value) {
            detail::PushBack(ret, innerValue, a);
        }

        return ret;
    }
};

template <typename ValueType, size_t Size, typename RJValue>
struct Serialize<std::array<ValueType, Size>, RJValue> {
    static RJValue
    get(const std::array<ValueType, Size> &value,
        typename RJValue::AllocatorType &a)
    {
        RJValue ret(rapidjson::kArrayType);

        for (size_t i = 0; i < Size; i++) {
            detail::PushBack(ret, value[i], a);
        }

        return ret;
    }
};

template <typename RJValue>
struct Serialize<std::any, RJValue> {
    static RJValue
    get(const std::any &value, typename RJValue::AllocatorType &a)
    {
        using std::any_cast;

        if (!value.has_value()) {
            return RJValue(rapidjson::kNullType);
        }

        if (value.type() == typeid(int)) {
            return Serialize<int, RJValue>::get(any_cast<int>(value), a);
        } else if (value.type() == typeid(float)) {
            return Serialize<float, RJValue>::get(any_cast<float>(value), a);
        } else if (value.type() == typeid(double)) {
            return Serialize<double, RJValue>::get(any_cast<double>(value), a);
        } else if (value.type() == typeid(bool)) {
            return Serialize<bool, RJValue>::get(any_cast<bool>(value), a);
        } else if (value.type() == typeid(std::string)) {
            return Serialize<std::string, RJValue>::get(
                any_cast<std::string>(value), a);
        } else if (value.type() == typeid(const char *)) {
            return Serialize<std::string, RJValue>::get(
                any_cast<const char *>(value), a);
        } else if (value.type() == typeid(std::map<std::string, std::any>)) {
            return Serialize<std::map<std::string, std::any>, RJValue>::get(
                any_cast<std::map<std::string, std::any>>(value), a);
        } else if (value.type() == typeid(std::vector<std::any>)) {
            return Serialize<std::vector<std::any>, RJValue>::get(
                any_cast<std::vector<std::any>>(value), a);
        } else if (value.type() == typeid(std::vector<std::string>)) {
            return Serialize<std::vector<std::string>, RJValue>::get(
                any_cast<std::vector<std::string>>(value), a);
        } else {
            // PS_DEBUG("[std::any] Serialize: Unknown type of value");
        }

        return RJValue(rapidjson::kNullType);
    }
};

namespace detail {

template <typename Type, typename RJValue = rapidjson::Value>
inline void
AddMember(RJValue &object, const char *key, const Type &value,
          typename RJValue::AllocatorType &a)
{
    assert(object.IsObject());

    object.AddMember(RJValue(key, a).Move(),
                     Serialize<Type, RJValue>::get(value, a), a);
}

template <typename Type, typename RJValue = rapidjson::Value>
inline void
PushBack(RJValue &array, const Type &value, typename RJValue::AllocatorType &a)
{
    assert(array.IsArray());

    array.PushBack(Serialize<Type, RJValue>::get(value, a), a);
}

}  // namespace detail

}  // namespace pajlada
