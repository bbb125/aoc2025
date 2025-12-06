#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <span>
#include <vector>
#include <ranges>

namespace aoc2025::day05
{
using Id = std::int64_t;
struct IdRange
{
    Id start;
    Id end;
};

constexpr auto combineOverlappingRanges(std::vector<IdRange> ranges)
{
    std::ranges::sort(ranges, std::less{}, &IdRange::start);

    std::vector<IdRange> unique;
    unique.reserve(ranges.size());

    for (const auto& range : ranges)
    {
        if (unique.empty() || range.start > unique.back().end)
            unique.push_back(range);
        else
            unique.back().end = std::max(unique.back().end, range.end);
    }
    return unique;
}

constexpr int solve1(std::vector<IdRange> ranges, std::span<const std::int64_t> ids)
{
    return std::ranges::count_if(  //
        ids,
        [uniqueRanges = combineOverlappingRanges(std::move(ranges))](auto id)
        {
            auto it =
                std::ranges::lower_bound(uniqueRanges, id, std::less{}, &IdRange::end);
            return it != std::end(uniqueRanges) && id >= it->start;
        });
}

static_assert(
    []
    {
        auto ids = std::to_array<Id>({1, 4, 7, 11, 15, 21});
        return solve1(std::vector<IdRange>{{3, 5}, {10, 14}, {16, 20}, {12, 18}}, ids)
               == 3;
    }());


constexpr auto solve2(std::vector<IdRange> ranges)
{
    namespace rv = std::ranges::views;
    return std::ranges::fold_left(  //
        combineOverlappingRanges(std::move(ranges))
            | rv::transform([](const auto& range)
                            { return range.end - range.start + 1; }),
        Id{},
        std::plus{});
}

static_assert(
    []
    {
        return solve2(std::vector<IdRange>{{3, 5}, {10, 14}, {16, 20}, {12, 18}}) == 14;
    }());

}  // namespace aoc2025::day05

int main()
{
    using namespace aoc2025::day05;
    std::ifstream file("./input.txt");
    if (!file)
    {
        fmt::println("Failed to read input file");
        return 1;
    }
    auto ranges = [&]
    {
        std::vector<IdRange> ranges;

        for (std::string line; std::getline(file, line) && !line.empty();)
        {
            if (auto pos = line.find('-'); pos != std::string::npos)
            {
                Id start = std::stoll(line.substr(0, pos));
                Id end = std::stoll(line.substr(pos + 1));
                ranges.push_back({start, end});
            }
        }
        return ranges;
    }();

    auto ids = [&]
    {
        std::vector<Id> ids;
        for (std::string line; std::getline(file, line);)
            ids.push_back(std::stoll(line));
        return ids;
    }();

    fmt::println("day05.solution1: {}", solve1(ranges, ids));
    fmt::println("day05.solution2: {}", solve2(ranges));
}