#pragma once

#include <utility>
#include <type_traits>

namespace aoc2025::functional
{
inline constexpr auto constant = []<typename T>(T&& value)
{
    return [value = std::forward<T>(value)](auto&&...) -> decltype(auto)
    {
        using Type = std::remove_cvref_t<T>;

        if constexpr (std::is_fundamental_v<Type> || std::is_pointer_v<Type>
                      || std::is_enum_v<Type>)
        {
            return value;
        }
        else
            return (value);  // braces allow compiler to deduce return type as reference
    };
};


namespace detail
{

// Overload selector
template <typename... Args>
struct selector
{
    template <typename R, typename C>
    constexpr auto operator()(R (C::*func)(Args...) const) const -> decltype(func)
    {
        return func;
    }

    template <typename R, typename C>
    constexpr auto operator()(R (C::*func)(Args...)) const -> decltype(func)
    {
        return func;
    }

    template <typename R>
    constexpr auto operator()(R (*func)(Args...)) const -> decltype(func)
    {
        return func;
    }
};
}  // namespace detail

/**
 * A helper that selects a specified overload of a function to pass as a parameter
 * or store.
 * Implemented as a less verbose and more expressive alternative to static_cast.
 * Works on both class members and standalone functions.
 * @example
 * // without helper
 * ::ranges::transform(input, output, static_cast<std::string(*)(int)>(&std::to_string));
 * // with helper
 * ::ranges::transform(input, output, functional::select<int>(&std::to_string));
 *
 * @tparam Args
 */
template <typename... Args>
inline constexpr detail::selector<Args...> select{};

template <std::size_t I>
inline constexpr auto selectIth = [](auto&& arg)
{
    return std::get<I>(std::forward<decltype(arg)>(arg));
};

}  // namespace aoc2025::functional