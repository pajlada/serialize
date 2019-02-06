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

// Deserialize is called when we load a json file into our library

template <typename Type, typename Enable = void>
struct Deserialize {
    static Type
    get(const rapidjson::Value & /*value*/, bool *error = nullptr)
    {
        // static_assert(false, "Unimplemented deserialize type");

        PAJLADA_REPORT_ERROR(error)

        return Type{};
    }
};

template <typename Type>
struct Deserialize<
    Type, typename std::enable_if<std::is_integral<Type>::value>::type> {
    static Type
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return Type{};
        }

        return detail::GetNumber<Type>(value);
    }
};

template <>
struct Deserialize<bool> {
    static bool
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (value.IsBool()) {
            // No conversion needed
            return value.GetBool();
        }

        if (value.IsInt()) {
            // Conversion from Int:
            // 1 == true
            // Anything else = false
            return value.GetInt() == 1;
        }

        PAJLADA_REPORT_ERROR(error)
        return false;
    }
};

template <>
struct Deserialize<double> {
    static double
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return double{};
        }

        return value.GetDouble();
    }
};

template <>
struct Deserialize<float> {
    static float
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return float{};
        }

        return value.GetFloat();
    }
};

template <>
struct Deserialize<std::string> {
    static std::string
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (!value.IsString()) {
            PAJLADA_REPORT_ERROR(error)
            return std::string{};
        }

        return value.GetString();
    }
};

template <typename ValueType>
struct Deserialize<std::map<std::string, ValueType>> {
    static std::map<std::string, ValueType>
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        std::map<std::string, ValueType> ret;

        if (!value.IsObject()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        for (rapidjson::Value::ConstMemberIterator it = value.MemberBegin();
             it != value.MemberEnd(); ++it) {
            ret.emplace(it->name.GetString(),
                        Deserialize<ValueType>::get(it->value));
        }

        return ret;
    }
};

template <typename ValueType>
struct Deserialize<std::vector<ValueType>> {
    static std::vector<ValueType>
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        std::vector<ValueType> ret;

        if (!value.IsArray()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        for (const rapidjson::Value &innerValue : value.GetArray()) {
            ret.emplace_back(Deserialize<ValueType>::get(innerValue));
        }

        return ret;
    }
};

template <typename ValueType, size_t Size>
struct Deserialize<std::array<ValueType, Size>> {
    static std::array<ValueType, Size>
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        std::array<ValueType, Size> ret;

        if (!value.IsArray()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        if (value.GetArray().Size() != Size) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        for (size_t i = 0; i < Size; ++i) {
            ret[i] = Deserialize<ValueType>::get(value[i]);
        }

        return ret;
    }
};

template <typename Arg1, typename Arg2>
struct Deserialize<std::pair<Arg1, Arg2>> {
    static std::pair<Arg1, Arg2>
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (!value.IsArray()) {
            PAJLADA_REPORT_ERROR(error)
            return std::make_pair(Arg1(), Arg2());
        }

        if (value.Size() != 2) {
            PAJLADA_REPORT_ERROR(error)
            return std::make_pair(Arg1(), Arg2());
        }

        return std::make_pair(Deserialize<Arg1>::get(value[0]),
                              Deserialize<Arg2>::get(value[1]));
    }
};

#ifdef PAJLADA_BOOST_ANY_SUPPORT
template <>
struct Deserialize<boost::any> {
    static boost::any
    get(const rapidjson::Value &value, bool *error = nullptr)
    {
        if (value.IsInt()) {
            return value.GetInt();
        } else if (value.IsFloat() || value.IsDouble()) {
            return value.GetDouble();
        } else if (value.IsString()) {
            return std::string(value.GetString());
        } else if (value.IsBool()) {
            return value.GetBool();
        } else if (value.IsObject()) {
            return Deserialize<std::map<std::string, boost::any>>::get(value);
        } else if (value.IsArray()) {
            return Deserialize<std::vector<boost::any>>::get(value);
        }

        PAJLADA_REPORT_ERROR(error)
        return boost::any();
    }
};
#endif

}  // namespace pajlada
