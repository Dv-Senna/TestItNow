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
		[[=CLIser::Long("seed"),
			=CLIser::Description("Set the random seed manually. Usefull to retest s-ome random values")
		]]
		std::optional<std::uint32_t> randomSeed;
	};


	template <typename Func>
	struct Janitor {
		constexpr Janitor(Func&& func) noexcept : m_func {func} {}
		constexpr ~Janitor() {m_func();}
		Func m_func;
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
	TestItNow::CommandLineArguments commandLineArguments {std::move(**commandLineArgumentsWithError)};

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
		TestItNow::Janitor _ {[](){std::println("\033[90m-------------------------------\033[m");}};
		if (!testResult) {
			std::println(stderr, "\033[31mFailure: {}\033[m", testResult.error());
			if (!commandLineArguments.continueTestingOnFailure)
				return EXIT_SUCCESS;
		}
		else
			std::println("{}", *testResult);
	}

	return EXIT_SUCCESS;
}


auto TestItNow::getTestList() noexcept -> std::vector<TestItNow::Test>& {
	static std::vector<TestItNow::Test> testList {};
	return testList;
}


auto TestItNow::Test::run(std::uint32_t randomSeed) const noexcept -> std::expected<std::string, std::string> {
	try {
		TestState state {
			.randomSeed = randomSeed,
			.internals = {
				.currentRandomSeed = randomSeed,
				.successCount = 0,
				.testCount = 0,
				.errors = {},
				.requirementNotFullfilled = false,
				.generators = {},
				.generatorStackTop = 0
			}
		};

		while (true) {
			auto instantiationResult {this->m_testInstantiation(state)};
			if (!instantiationResult)
				return instantiationResult.error();
			const auto& [endIteration, successMessage] {*instantiationResult};
			if (!endIteration)
				continue;
			if (!successMessage.empty())
				return successMessage;
			break;
		}

		std::string output {std::format("\033[32m{}/{} tests succeeded\033[m",
			state.internals.successCount,
			state.internals.testCount
		)};
		return output;
	}
	catch (std::exception &exception) {
		return std::unexpected(std::format("Uncaught exception thrown in test {} : {}",
			m_name,
			exception.what()
		));
	}
	catch (...) {
		return std::unexpected(std::format("Uncaught and unknown exception thrown in test {}", m_name));
	}
}


auto TestItNow::Test::m_testInstantiation(TestState& state) const
	-> std::expected<std::pair<bool, std::string>, std::string>
{
	m_callback(state);

	if (state.internals.testCount == 0)
		return std::pair{true, std::format("\033[33mYour test seems to be empty\033[m")};

	if (state.internals.testCount != state.internals.successCount) {
		std::ostringstream stream {};
		for (std::string_view delim {""}; const auto& str : state.internals.errors) {
			stream << delim << str;
			delim = "\n\t";
		}
		if (!state.internals.requirementNotFullfilled) {
			stream << std::format("\n{}/{} tests succeeded",
				state.internals.successCount,
				state.internals.testCount
			);
		}
		return std::unexpected{stream.str()};
	}

	if (state.internals.generators.empty())
		return std::pair{true, ""};

	do {
		state.internals.generators[state.internals.generatorStackTop]->advance();
		if (!state.internals.generators[state.internals.generatorStackTop]->isEmpty())
			break;

		state.internals.generators[state.internals.generatorStackTop].reset();
		if (state.internals.generatorStackTop == 0) {
			return std::pair{true, ""};
		}
		--state.internals.generatorStackTop;
	} while (!state.internals.generators[state.internals.generatorStackTop]->isEmpty());
	return std::pair{false, ""};
}
