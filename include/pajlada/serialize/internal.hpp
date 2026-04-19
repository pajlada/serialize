#pragma once

#ifdef PAJLADA_SERIALIZE_LOG_VERBOSE

#include <rapidjson/document.h>      // IWYU pragma: keep
#include <rapidjson/prettywriter.h>  // IWYU pragma: keep
#include <rapidjson/writer.h>        // IWYU pragma: keep

#include <iostream>                                 // IWYU pragma: keep
#include <pajlada/serialize/internal-typename.hpp>  // IWYU pragma: keep

#endif

namespace pajlada::internal {

#ifdef PAJLADA_SERIALIZE_LOG_VERBOSE

inline std::string
pp(const rapidjson::Value &v)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    v.Accept(writer);
    return {buffer.GetString()};
}

#define PSE_DEBUG(x) std::cout << x << '\n';
#else
#define PSE_DEBUG(x)
#endif

}  // namespace pajlada::internal
