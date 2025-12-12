#include "util/algorithm.h"
#include "util/views.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstdint>
#include <vector>
#include <ranges>
#include <fstream>
#include <functional>

namespace aoc2025::day06
{
enum class Instruction
{
    Add = '+',
    Mul = '*',
};
constexpr std::vector<std::int64_t> findTotal(
    const std::vector<std::vector<std::int64_t>>& input,
    const std::vector<Instruction>& instructions)
{
    return std::ranges::fold_left(  //
        input | std::views::drop(1),
        input[0],
        [&instructions](auto&& acc, const auto& next)
        {
            for (auto [instruction, a, b] : std::views::zip(instructions, acc, next))
            {
                switch (instruction)
                {
                case Instruction::Add:
                    a += b;
                    break;
                case Instruction::Mul:
                    a *= b;
                    break;
                }
            }
            return std::move(acc);
        });
}
constexpr std::int64_t solve1(const std::vector<std::vector<std::int64_t>>& input,
                              const std::vector<Instruction>& instructions)
{
    return algorithm::sum(findTotal(input, instructions));
}

static_assert(
    []
    {
        auto result = solve1(  //
            std::vector{std::vector<std::int64_t>{123, 328, 51, 64},
                        std::vector<std::int64_t>{45, 64, 387, 23},
                        std::vector<std::int64_t>{6, 98, 215, 314}},
            std::vector{Instruction::Mul, Instruction::Add, Instruction::Mul, Instruction::Add});
        return result == 4277556;
    }());

constexpr std::int64_t processInput2(const std::vector<std::string>& lines)
{
    // ugly, but surprisingly, still can be constexpr
    namespace rv = std::views;
    auto columns = std::ssize(lines[0]);
    auto rows = std::ssize(lines);
    auto notEmpty = rv::filter(std::bind_front(std::not_equal_to{}, ' '));
    auto numbers =  // make a series of numbers from columns
        rv::iota(0, columns)
        | rv::transform(
            [&](auto col)
            {
                auto val = std::ranges::fold_left(  //
                    lines | rv::take(rows - 1)
                        | rv::transform([&](const auto& line)
                                        { return line[col]; })
                        | notEmpty,
                    0ll,
                    [](auto acc, auto ch) { return acc * 10 + (ch - '0'); });
                return val;
            })
        | std::ranges::to<std::vector>();
    auto instructions =  // extract instructions
        lines.back() | notEmpty
        | rv::transform(
            [](auto ch)
            { return ch == '*' ? Instruction::Mul : Instruction::Add; })
        | std::ranges::to<std::vector<Instruction>>();
    // zeroes are now delimiters - group by zeroes and apply instruction
    auto groupResults =  //
        numbers | rv::split(0) | rv::enumerate
        | rv::transform(
            [&](auto chunked)
            {
                auto [index, chunk] = chunked;
                return std::ranges::fold_left(  //
                    chunk,
                    instructions[index] == Instruction::Mul ? 1ll : 0ll,
                    [&](auto acc, auto val)
                    {
                        switch (instructions[index])
                        {
                        case Instruction::Add:
                            return acc + val;
                        case Instruction::Mul:
                            return acc * val;
                        }
                        std::unreachable();
                    });
            });
    return std::ranges::fold_left(groupResults, 0ll, std::plus<>{});
}

static_assert(
    []
    {
        return processInput2({
                   //
                   "123 328  51 64 ",
                   " 45 64  387 23 ",
                   "  6 98  215 314",
                   "*   +   *   +  "  //
               })
               == 3263827;
    }());
}  // namespace aoc2025::day06

int main()
{
    using namespace aoc2025::day06;
    auto [input01, instructions01] = []
    {
        std::ifstream file("./input.txt");
        if (not file)
        {
            fmt::println("Failed to open file");
            std::exit(EXIT_FAILURE);
        }

        std::vector<std::vector<std::int64_t>> input;
        std::vector<Instruction> instructions;
        for (std::string line; std::getline(file, line);)
        {
            if (line[0] == '*' || line[0] == '+')
            {
                instructions =
                    std::views::split(line, ' ') | aoc2025::views::notEmpty
                    | std::views::transform(
                        [](auto part)
                        {
                            return std::string_view{std::begin(part), std::end(part)} == "*"
                                       ? Instruction::Mul
                                       : Instruction::Add;
                        })
                    | std::ranges::to<std::vector<Instruction>>();
            }
            else
            {
                input.push_back(std::views::split(line, ' ') | aoc2025::views::notEmpty
                                | std::views::transform(
                                    [](auto&& part)
                                    {
                                        return std::stoll(
                                            part | std::ranges::to<std::string>());
                                    })
                                | std::ranges::to<std::vector>());
            }
        }
        return std::pair{std::move(input), std::move(instructions)};
    }();
    fmt::println("day06.solution1: {}", solve1(input01, instructions01));

    fmt::println("test02: {}",
                 processInput2(std::vector<std::string>{"123 328  51 64 ",
                                                        " 45 64  387 23 ",
                                                        "  6 98  215 314",
                                                        "*   +   *   +  "}));  // 123456 + 789012 + 567890 = 1480358

    auto solution2 = []
    {
        std::ifstream file("./input.txt");
        if (not file)
        {
            fmt::println("Failed to open file");
            std::exit(EXIT_FAILURE);
        }

        std::vector<std::string> lines;
        for (std::string line; std::getline(file, line);)
            lines.push_back(std::move(line));
        return processInput2(lines);
    }();
    fmt::println("day06.solution2: {}", solution2);
}