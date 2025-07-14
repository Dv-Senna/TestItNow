#include <print>

#include <TestItNow/entryPoint.hpp>
#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>


namespace tests {
	[[=TestItNow::Test, =TestItNow::Tags("tag1", "tag2", "tag3")]]
	auto test1() -> TestItNow::TestResult {
		std::println("Test1 body");
		return {};
	};
}

static TestItNow::TestRegister<^^tests> _ {__FILE__};
