#pragma once

#include <cstdlib>
#include <print>

#include <CLIser/CLIser.hpp>


namespace TestItNow {
	struct [[
		=CLIser::Help,
		=CLIser::Name("TestItNow"),
		=CLIser::Version(TestItNow_VERSION)
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
	auto commandLineArguments {std::move(*commandLineArgumentsWithError)};

	std::println("Hello");
	std::println("All : {}", commandLineArguments.runAll);
	std::println("Tests : {}", parser.getUnnamedArgs());
	return EXIT_SUCCESS;
}
