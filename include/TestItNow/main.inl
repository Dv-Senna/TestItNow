#pragma once

#include <cstdio>
#include <cstdlib>

#include "TestItNow/output.hpp"
#include "TestItNow/testManager.hpp"


tin::Result test1() {
	return tin::Result::ePassed;
};


int main(int argc, char **argv) {
	std::vector<std::string> args {};
	args.reserve(argc - 1);
	for (int i {1}; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	std::vector<std::string> tests {tin::TestManager::getTests()};

	if (args.size() == 0) {
		tin::print<tin::Severity::eFailure> ("You must provide some tests to run, or '-a' to run everything");
		return EXIT_FAILURE;
	}

	if (args.size() == 1 && args[0] == "-a") {
		for (const auto &test : tests)
			tin::TestManager::runTest(test);
		return EXIT_SUCCESS;
	}

	if (args.size() == 1 && args[0] == "--version") {
		printf("Tests build with TestItNow version %d.%d.%d (%s)\n",
			TIN_MAJOR_VERSION,
			TIN_MINOR_VERSION,
			TIN_PATCH_VERSION,
			TIN_VARIANT_VERSION == 1 ? "dev" : "build"
		);
		return EXIT_SUCCESS;
	}

	for (const auto &arg : args)
		tin::TestManager::runTest(arg);

	return EXIT_SUCCESS;
}


#include "testManager.inl"
#include "test.inl"