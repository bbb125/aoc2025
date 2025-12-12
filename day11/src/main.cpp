#include "util/algorithm.h"
#include "util/functors.h"
#include "util/views.h"


#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
#include <cstdint>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace aoc2025::day11
{
using Vertex = std::string;
using Connections = std::unordered_map<Vertex, std::vector<Vertex>>;

auto solve1(const Connections& connections, const Vertex& from, const Vertex& to)
{
    // use DFS with cache to find a number of paths from 'from' to 'to'
    auto cache = std::unordered_map<Vertex, std::int64_t>{};
    auto dfs = [&](this auto self, const Vertex& current) -> std::int64_t
    {
        if (current == to)
            return 1;

        if (cache.contains(current))
            return cache[current];

        auto connectionsIt = connections.find(current);
        if (connectionsIt == std::end(connections))
            return 0;

        auto pathCount = algorithm::sum(  //
            connectionsIt->second | std::views::transform(self));

        cache[current] = pathCount;
        return pathCount;
    };
    return dfs(from);
}

auto solve2(const Connections& connections,
            const Vertex& from,
            const Vertex& to,
            const std::pair<Vertex, Vertex>& mustVisit)
{
    // find the number of path from 'from' to 'to' that visit all 'mustVisit'
    // vertices use solve1 a few times
    return solve1(connections, from, mustVisit.first)
               * solve1(connections, mustVisit.first, mustVisit.second)
               * solve1(connections, mustVisit.second, to)
           + solve1(connections, from, mustVisit.second)
                 * solve1(connections, mustVisit.second, mustVisit.first)
                 * solve1(connections, mustVisit.first, to);
}

void test1()
{
    using std::string_literals::operator""s;
    Connections connections{
        {"aaa"s, std::vector{"you"s, "hhh"s}},
        {"you"s, std::vector{"bbb"s, "ccc"s}},
        {"bbb"s, std::vector{"ddd"s, "eee"s}},
        {"ccc"s, std::vector{"ddd"s, "eee"s, "fff"s}},
        {"ddd"s, std::vector{"ggg"s}},
        {"eee"s, std::vector{"out"s}},
        {"fff"s, std::vector{"out"s}},
        {"ggg"s, std::vector{"out"s}},
        {"hhh"s, std::vector{"ccc"s, "fff"s, "iii"s}},
        {"iii"s, std::vector{"out"s}},
    };
    assert(solve1(connections, "you", "out") == 5);
}

void test2()
{
    using std::string_literals::operator""s;
    Connections connections{
        {"svr"s, std::vector{"aaa"s, "bbb"s}},
        {"aaa"s, std::vector{"fft"s}},
        {"fft"s, std::vector{"ccc"s}},
        {"bbb"s, std::vector{"tty"s}},
        {"tty"s, std::vector{"ccc"s}},
        {"ccc"s, std::vector{"ddd"s, "eee"s}},
        {"ddd"s, std::vector{"hub"s}},
        {"hub"s, std::vector{"fff"s}},
        {"eee"s, std::vector{"dac"s}},
        {"dac"s, std::vector{"fff"s}},
        {"fff"s, std::vector{"ggg"s, "hhh"s}},
        {"ggg"s, std::vector{"out"s}},
        {"hhh"s, std::vector{"out"s}},
    };
    assert(solve2(connections, "svr", "out", {"fft", "dac"}) == 2);
}
}  // namespace aoc2025::day11

int main()
{
    using namespace aoc2025::day11;
    test1();
    test2();

    std::ifstream file("./input.txt");
    if (not file)
    {
        fmt::println("Failed to open file");
        return 1;
    }

    Connections connections;
    for (std::string line; std::getline(file, line);)
    {
        auto separator = line.find(':');
        auto vertex = std::string_view{line}.substr(0, separator);
        auto edges = std::string_view{line}.substr(separator + 1);
        connections.emplace(  //
            vertex,
            edges | std::views::split(' ') | aoc2025::views::notEmpty
                | std::views::transform(
                    [](auto part)
                    { return part | std::ranges::to<std::string>(); })
                | std::ranges::to<std::vector>());
    }
    fmt::println("day11.solution1: {}", solve1(connections, "you", "out"));  // 497
    fmt::println("day11.solution2: {}",
                 solve2(connections, "svr", "out", {"fft", "dac"}));  // 358564784931864
}