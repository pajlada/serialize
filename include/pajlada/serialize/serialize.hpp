#pragma once

#include <pajlada/serialize/common.hpp>

#include <rapidjson/document.h>

#ifdef PAJLADA_BOOST_ANY_SUPPORT
#include <boost/any.hpp>
#endif

#include <cassert>
#include <cmath>
#include <map>
#include <stdexcept>
#include <typeinfo>
#include <vector>

namespace pajlada {

namespace detail {

template <typename Type>
inline void AddMember(rapidjson::Value &object, const char *key,
                      const Type &value, rapidjson::Document::AllocatorType &a);

template <typename Type>
inline void PushBack(rapidjson::Value &array, const Type &value,
                     rapidjson::Document::AllocatorType &a);

}  // namespace detail

// Serialize is called when a settings value is being saved

// Create a rapidjson::Value from the templated value
template <typename Type>
struct Serialize {
    static rapidjson::Value
    get(const Type &value, rapidjson::Document::AllocatorType &)
    {
        rapidjson::Value ret(value);

        return ret;
    }
};

template <>
struct Serialize<std::string> {
    static rapidjson::Value
    get(const std::string &value, rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(value.c_str(), a);

        return ret;
    }
};

template <typename Arg1, typename Arg2>
struct Serialize<std::pair<Arg1, Arg2>> {
    static rapidjson::Value
    get(const std::pair<Arg1, Arg2> &value,
        rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kArrayType);

        ret.PushBack(Serialize<Arg1>::get(value.first, a), a);
        ret.PushBack(Serialize<Arg2>::get(value.second, a), a);

        return ret;
    }
};

template <typename ValueType>
struct Serialize<std::map<std::string, ValueType>> {
    static rapidjson::Value
    get(const std::map<std::string, ValueType> &value,
        rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kObjectType);

        for (auto it = value.begin(); it != value.end(); ++it) {
            detail::AddMember(ret, it->first.c_str(), it->second, a);
        }

        return ret;
    }
};

template <typename ValueType>
struct Serialize<std::vector<ValueType>> {
    static rapidjson::Value
    get(const std::vector<ValueType> &value,
        rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kArrayType);

        for (const auto &innerValue : value) {
            detail::PushBack(ret, innerValue, a);
        }

        return ret;
    }
};

#ifdef PAJLADA_BOOST_ANY_SUPPORT
template <>
struct Serialize<boost::any> {
    static rapidjson::Value
    get(const boost::any &value, rapidjson::Document::AllocatorType &a)
    {
        using boost::any_cast;

        if (value.empty()) {
            return rapidjson::Value(rapidjson::kNullType);
        }

        if (value.type() == typeid(int)) {
            return Serialize<int>::get(any_cast<int>(value), a);
        } else if (value.type() == typeid(float)) {
            return Serialize<float>::get(any_cast<float>(value), a);
        } else if (value.type() == typeid(double)) {
            return Serialize<double>::get(any_cast<double>(value), a);
        } else if (value.type() == typeid(bool)) {
            return Serialize<bool>::get(any_cast<bool>(value), a);
        } else if (value.type() == typeid(std::string)) {
            return Serialize<std::string>::get(any_cast<std::string>(value), a);
        } else if (value.type() == typeid(const char *)) {
            return Serialize<std::string>::get(any_cast<const char *>(value),
                                               a);
        } else if (value.type() == typeid(std::map<std::string, boost::any>)) {
            return Serialize<std::map<std::string, boost::any>>::get(
                any_cast<std::map<std::string, boost::any>>(value), a);
        } else if (value.type() == typeid(std::vector<boost::any>)) {
            return Serialize<std::vector<boost::any>>::get(
                any_cast<std::vector<boost::any>>(value), a);
        } else if (value.type() == typeid(std::vector<std::string>)) {
            return Serialize<std::vector<std::string>>::get(
                any_cast<std::vector<std::string>>(value), a);
        } else {
            // PS_DEBUG("[boost::any] Serialize: Unknown type of value");
        }

        return rapidjson::Value(rapidjson::kNullType);
    }
};
#endif

namespace detail {

template <typename Type>
inline void
AddMember(rapidjson::Value &object, const char *key, const Type &value,
          rapidjson::Document::AllocatorType &a)
{
    assert(object.IsObject());

    object.AddMember(rapidjson::Value(key, a).Move(),
                     Serialize<Type>::get(value, a), a);
}

template <typename Type>
inline void
PushBack(rapidjson::Value &array, const Type &value,
         rapidjson::Document::AllocatorType &a)
{
    assert(array.IsArray());

    array.PushBack(Serialize<Type>::get(value, a), a);
}

}  // namespace detail

}  // namespace pajlada
