#include <print>

#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>


namespace tests {
	[[=TestItNow::Test("test3"), =TestItNow::Tags("tag1")]]
	auto testBody3() -> TestItNow::TestResult {
		std::println("Test2 body");
		return {};
	};
}

static TestItNow::TestRegister<^^tests> _3 {__FILE__};
