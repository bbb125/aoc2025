#pragma once

#include <algorithm>
#include <ranges>

namespace aoc2025::algorithm
{
template <std::ranges::input_range R>
constexpr auto sum(R&& rng, std::ranges::range_value_t<R> init = {})
{
    return std::ranges::fold_left(  //
        std::forward<R>(rng),
        init,
        std::plus<>{});
}
}  // namespace aoc2025::algorithm