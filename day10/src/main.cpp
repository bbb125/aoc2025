#include <ctre.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <functional>
#include <fstream>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace aoc2025::day10
{
constexpr auto maxSize = 16;
using Container = std::bitset<maxSize>;
struct MachineConfiguration
{
    Container targetState;
    std::vector<Container> switchers;
};

constexpr auto solve1(const MachineConfiguration& input)
{
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;

    const auto numberOfOptions = (1 << std::size(input.switchers)) - 1;
    return rv::iota(0, numberOfOptions)
           | rv::filter(
               [&](Container candidate)
               {
                   return rng::fold_left(  //
                              input.switchers | rv::enumerate
                                  | rv::transform(
                                      [&](auto val)
                                      {
                                          auto [i, switcher] = val;
                                          return candidate[i] ? switcher
                                                              : Container{};
                                      }),
                              Container{},
                              std::bit_xor{})
                          == input.targetState;
               });
}

constexpr auto solve1(std::span<const MachineConfiguration> input)
{
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;

    return rng::fold_left(  //
        input
            | rv::transform(
                [](const auto& config)
                {
                    return rng::min(
                        solve1(config)
                        | rv::transform([](Container c) { return c.count(); }));
                }),
        0,
        std::plus{});
}

constexpr auto deserializeState(std::string_view str)
{
    Container result;
    for (auto [i, ch] : str | std::views::enumerate)
        result.set(i, ch == '#');
    return result;
}
static_assert(deserializeState(std::string_view{"#..##.."})
              == Container{0b0011001});  // 0b1001100

constexpr auto deserializeSwitcher(std::span<const std::int8_t> config)
{
    Container result;
    for (const auto num : config)
        result.set(num);
    return result;
}
static_assert(deserializeSwitcher(std::to_array<std::int8_t>({0, 1, 2, 4, 5, 7}))
              == Container{0b10110111});

constexpr MachineConfiguration deserializeConfig(std::string_view target,
                                                 std::span<const Container> switchers)
{
    return {
        .targetState = deserializeState(target),
        .switchers{std::begin(switchers), std::end(switchers)},
    };
}
// [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
// [...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
// [.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
//
static_assert(
    []
    {
        auto testSet = std::to_array(  //
            {deserializeConfig(        //
                 ".##.",
                 std::to_array<Container>({0b1000, 0b1010, 0b0100, 0b1100, 0b0101, 0b0011})),
             deserializeConfig(  //
                 "...#.",
                 std::to_array<Container>({0b1101, 0b1100, 0b1001, 0b0111, 0b11110})),
             deserializeConfig(  //
                 ".###.#",
                 std::to_array<Container>({0x011111, 0b011001, 0b110111, 0b000110}))});
        return solve1(testSet) == 7;
    }());

}  // namespace aoc2025::day10

int main()
{
    using namespace aoc2025::day10;
    auto testSet = std::to_array(  //
        {deserializeConfig(        //
             ".##.",
             std::to_array<Container>({0b1000, 0b1010, 0b0100, 0b1100, 0b0101, 0b0011})),
         deserializeConfig(  //
             "...#.",
             std::to_array<Container>({0b1101, 0b1100, 0b1001, 0b0111, 0b11110})),
         deserializeConfig(  //
             ".###.#",
             std::to_array<Container>({0x011111, 0b011001, 0b110111, 0b000110}))});
    fmt::println("day10.test1: {}", solve1(testSet));


    std::ifstream file("./input.txt");
    if (not file)
    {
        fmt::println("Could not open input.txt");
        return 1;
    }

    std::vector<MachineConfiguration> configurations;
    for (std::string line; std::getline(file, line);)
    {
        auto [_1, mask, buttons, _2] =
            ctre::match<"\\[([\\.#]+)\\] (\\(.*\\)) \\{(.*)\\}">(line);
        // fmt::println("mask: {}, buttons: {}", mask.to_view(), buttons.to_view());
        [[maybe_unused]] auto switchers =
            buttons.to_view() | std::views::split(' ')
            | std::views::transform(
                [](auto v)
                {
                    std::string_view str{v};
                    return deserializeSwitcher(
                        str.substr(1, str.size() - 2) | std::views::split(',')
                        | std::views::transform(
                            [](auto numView)
                            {
                                auto numStr = std::string_view{numView}
                                              | std::ranges::to<std::string>();
                                return static_cast<std::int8_t>(std::stoi(numStr));
                            })
                        | std::ranges::to<std::vector<std::int8_t>>());
                })
            | std::ranges::to<std::vector>();

        configurations.push_back(deserializeConfig(mask.to_view(), switchers));
    }
    fmt::println("day10.solution1: {}", solve1(configurations));
}