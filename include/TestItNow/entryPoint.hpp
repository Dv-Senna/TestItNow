#pragma once

#include <cstdlib>
#include <optional>
#include <print>
#include <random>

#include <CLIser/CLIser.hpp>

#include "TestItNow/test.hpp"


namespace TestItNow {
	struct [[
		=CLIser::Help,
		=CLIser::Name("TestItNow"),
		=CLIser::Version(TestItNow_VERSION),
		=CLIser::FatalUnknown
	]] CommandLineArguments {
		[[=CLIser::Short("a"), =CLIser::Long("all"), =CLIser::Description("Run all the tests")]]
		bool runAll;
		[[=CLIser::Long("continue-on-failure"), =CLIser::Description("Run the next tests, even if one failed")]]
		bool continueTestingOnFailure;
		[[=CLIser::Long("seed"), =CLIser::Description("Set the random seed manually. Usefull to retest some values")]]
		std::optional<std::uint32_t> randomSeed;
	};
}

auto main(int argc, char** argv) -> int {
	auto parserWithError {CLIser::Parser::create({
		.args = std::span{argv, argv + argc}
	})};
	if (!parserWithError)
		return std::println(stderr, "Can't initialize CLIser parser : {}", parserWithError.error()), EXIT_FAILURE;
	auto parser {std::move(*parserWithError)};

	auto commandLineArgumentsWithError {parser.parse<TestItNow::CommandLineArguments> ()};
	if (!commandLineArgumentsWithError) {
		std::println(stderr, "CLIser can't parse given argument : {}", commandLineArgumentsWithError.error());
		return EXIT_FAILURE;
	}
	if (!*commandLineArgumentsWithError)
		return EXIT_SUCCESS;
	auto commandLineArguments {std::move(**commandLineArgumentsWithError)};

	std::vector<TestItNow::Test> testsToRun {};
	if (commandLineArguments.runAll)
		testsToRun = std::move(TestItNow::getTestList());
	else {
		testsToRun = TestItNow::getTestList()
			| std::views::filter([&parser](const auto& test) {
				return std::ranges::find(parser.getUnnamedArgs(), test.getName()) != parser.getUnnamedArgs().end();
			})
			| std::views::transform([](auto& test) {return std::move(test);})
			| std::ranges::to<std::vector> ();
	}

	auto unkownTests {parser.getUnnamedArgs()
		| std::views::filter([&testsToRun](const auto& arg) {
			return std::ranges::find_if(testsToRun, [&arg](const auto& test) {
				return test.getName() == arg;
			}) == testsToRun.end();
		})
	};
	for (const auto& arg : unkownTests)
		std::println("Warning: specified test '{}' is not known", arg);

	if (testsToRun.empty())
		return std::println("Error: you must run some valid tests"), EXIT_FAILURE;

	const std::uint32_t randomSeed {[&commandLineArguments]() {
		if (commandLineArguments.randomSeed)
			return *commandLineArguments.randomSeed;
		std::random_device device {};
		return device();
	} ()};
	std::println("\033[90mRandom seed used \033[1;90m{}\033[m", randomSeed);

	for (const auto& test : testsToRun) {
		std::println("\033[36mRunning test '\033[1;36m{}\033[36m'\033[m", test.getName());
		std::println("\033[36m===============================\033[m");
		std::expected testResult {test.run(randomSeed)};
		if (!testResult)
			std::println(stderr, "\033[31mFailure: {}\033[m", testResult.error());
		else
			std::println("{}", *testResult);
		std::println("\033[90m-------------------------------\033[m");
	}

	return EXIT_SUCCESS;
}


auto TestItNow::getTestList() noexcept -> std::vector<TestItNow::Test>& {
	static std::vector<TestItNow::Test> testList {};
	return testList;
}
