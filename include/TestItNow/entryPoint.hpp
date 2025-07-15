#pragma once

#include <CLIser/argsConfig.hpp>
#include <cstdlib>
#include <print>

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

	for (const auto& test : TestItNow::getTestList()) {
		std::println("Running test {} with tags {}", test.getName(), test.getTags());
		TestItNow::TestResult testResult {test.run()};
		if (!testResult)
			std::println(stderr, "\033[31mTest failed : {}\033[m", testResult.error().message);
	}

	std::println("All : {}", commandLineArguments.runAll);
	std::println("Tests : {}", parser.getUnnamedArgs());
	return EXIT_SUCCESS;
}


auto TestItNow::getTestList() noexcept -> std::vector<TestItNow::Test>& {
	static std::vector<TestItNow::Test> testList {};
	return testList;
}
