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
#include <string>
#include <typeinfo>
#include <vector>

namespace pajlada {

// Deserialize is called when we load a json file into our library

template <typename Type, typename RJValue = rapidjson::Value,
          typename Enable = void>
struct Deserialize {
    static Type
    get(const RJValue & /*value*/, bool *error = nullptr)
    {
        // static_assert(false, "Unimplemented deserialize type");

        PAJLADA_REPORT_ERROR(error)

        return Type{};
    }
};

template <typename Type, typename RJValue>
struct Deserialize<
    Type, RJValue,
    typename std::enable_if<std::is_integral<Type>::value>::type> {
    static Type
    get(const RJValue &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return Type{};
        }

        return detail::GetNumber<Type>(value);
    }
};

template <typename RJValue>
struct Deserialize<bool, RJValue> {
    static bool
    get(const RJValue &value, bool *error = nullptr)
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

template <typename RJValue>
struct Deserialize<double, RJValue> {
    static double
    get(const RJValue &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return double{};
        }

        return value.GetDouble();
    }
};

template <typename RJValue>
struct Deserialize<float, RJValue> {
    static float
    get(const RJValue &value, bool *error = nullptr)
    {
        if (!value.IsNumber()) {
            PAJLADA_REPORT_ERROR(error)
            return float{};
        }

        return value.GetFloat();
    }
};

template <typename RJValue>
struct Deserialize<std::string, RJValue> {
    static std::string
    get(const RJValue &value, bool *error = nullptr)
    {
        if (!value.IsString()) {
            PAJLADA_REPORT_ERROR(error)
            return std::string{};
        }

        return value.GetString();
    }
};

template <typename ValueType, typename RJValue>
struct Deserialize<std::map<std::string, ValueType>, RJValue> {
    static std::map<std::string, ValueType>
    get(const RJValue &value, bool *error = nullptr)
    {
        std::map<std::string, ValueType> ret;

        if (!value.IsObject()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        for (typename RJValue::ConstMemberIterator it = value.MemberBegin();
             it != value.MemberEnd(); ++it) {
            ret.emplace(it->name.GetString(),
                        Deserialize<ValueType, RJValue>::get(it->value));
        }

        return ret;
    }
};

template <typename ValueType, typename RJValue>
struct Deserialize<std::vector<ValueType>, RJValue> {
    static std::vector<ValueType>
    get(const RJValue &value, bool *error = nullptr)
    {
        std::vector<ValueType> ret;

        if (!value.IsArray()) {
            PAJLADA_REPORT_ERROR(error)
            return ret;
        }

        for (const RJValue &innerValue : value.GetArray()) {
            ret.emplace_back(Deserialize<ValueType, RJValue>::get(innerValue));
        }

        return ret;
    }
};

template <typename ValueType, size_t Size, typename RJValue>
struct Deserialize<std::array<ValueType, Size>, RJValue> {
    static std::array<ValueType, Size>
    get(const RJValue &value, bool *error = nullptr)
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
            ret[i] = Deserialize<ValueType, RJValue>::get(value[i]);
        }

        return ret;
    }
};

template <typename Arg1, typename Arg2, typename RJValue>
struct Deserialize<std::pair<Arg1, Arg2>, RJValue> {
    static std::pair<Arg1, Arg2>
    get(const RJValue &value, bool *error = nullptr)
    {
        if (!value.IsArray()) {
            PAJLADA_REPORT_ERROR(error)
            return std::make_pair(Arg1(), Arg2());
        }

        if (value.Size() != 2) {
            PAJLADA_REPORT_ERROR(error)
            return std::make_pair(Arg1(), Arg2());
        }

        return std::make_pair(Deserialize<Arg1, RJValue>::get(value[0]),
                              Deserialize<Arg2, RJValue>::get(value[1]));
    }
};

#ifdef PAJLADA_BOOST_ANY_SUPPORT
template <typename RJValue>
struct Deserialize<boost::any, RJValue> {
    static boost::any
    get(const RJValue &value, bool *error = nullptr)
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
            return Deserialize<std::map<std::string, boost::any>, RJValue>::get(
                value);
        } else if (value.IsArray()) {
            return Deserialize<std::vector<boost::any>, RJValue>::get(value);
        }

        PAJLADA_REPORT_ERROR(error)
        return boost::any();
    }
};
#endif

}  // namespace pajlada
