// SPDX-License-Identifier: CC-BY-SA-4.0
// Adapted from https://stackoverflow.com/questions/81870/is-it-possible-to-print-the-name-of-a-variables-type-in-standard-c/64490578#64490578
// Licensed under StackOverflow's CC-BY-SA 4.0 license https://creativecommons.org/licenses/by-sa/4.0/

#pragma once

#ifdef PAJLADA_SERIALIZE_LOG_VERBOSE

#include <source_location>  // IWYU pragma: keep
#include <string_view>      // IWYU pragma: keep

#endif

namespace pajlada::internal {

#ifdef PAJLADA_SERIALIZE_LOG_VERBOSE

template <typename T>
constexpr std::string_view type_name();

template <>
constexpr std::string_view
type_name<void>()
{
    return "void";
}

namespace detail {

using type_name_prober = void;

template <typename T>
constexpr std::string_view
wrapped_type_name()
{
    return std::source_location::current().function_name();
}

constexpr std::size_t
wrapped_type_name_prefix_length()
{
    return wrapped_type_name<type_name_prober>().find(
        type_name<type_name_prober>());
}

constexpr std::size_t
wrapped_type_name_suffix_length()
{
    return wrapped_type_name<type_name_prober>().length() -
           wrapped_type_name_prefix_length() -
           type_name<type_name_prober>().length();
}

}  // namespace detail

template <typename T>
constexpr std::string_view
type_name()
{
    constexpr auto wrapped_name = detail::wrapped_type_name<T>();
    constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
    constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
    constexpr auto type_name_length =
        wrapped_name.length() - prefix_length - suffix_length;
    return wrapped_name.substr(prefix_length, type_name_length);
}

#endif

}  // namespace pajlada::internal
