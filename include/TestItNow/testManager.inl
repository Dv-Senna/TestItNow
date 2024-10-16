#pragma once

#include <cstdio>

#include "TestItNow/output.hpp"
#include "TestItNow/testManager.hpp"



namespace tin {
	void TestManager::addTest(const std::string &name, TestCallback testFunc) {
		if (s_tests == nullptr)
			s_tests = new std::map<std::string, TestCallback> ();

		(*s_tests)[name] = testFunc;
	}


	void TestManager::runTest(const std::string &testName) {
		auto test {s_tests->find(testName)};
		if (test == s_tests->end())
			return tin::print<tin::Severity::eWarning> ("Can't find test '%s'", testName.c_str());

		tin::print("Running test '%s'", testName.c_str());

		tin::Result result {test->second()};
		if (result != tin::Result::ePassed)
			return tin::print<tin::Severity::eFailure> ("Test '%s' failed with code %u", testName.c_str(), (uint32_t)result);

		tin::print<tin::Severity::eSuccess> ("Test '%s' passed", testName.c_str());
	}


	void TestManager::cleanup() {
		if (s_tests != nullptr)
			delete s_tests;
	}


	std::vector<std::string> TestManager::getTests() {
		std::vector<std::string> output {};
		output.reserve(s_tests->size());
		for (const auto &test : *s_tests)
			output.push_back(test.first);
		return output;
	}


	std::map<std::string, TestManager::TestCallback> *TestManager::s_tests {nullptr};

} // namespace tin
