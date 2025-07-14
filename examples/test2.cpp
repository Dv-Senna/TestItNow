#include <print>

#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>


namespace tests {
	[[=TestItNow::Test, =TestItNow::Tags("tag1", "tag3")]]
	auto test2() -> TestItNow::TestResult {
		std::println("Test2 body");
		return {};
	};
}

static TestItNow::TestRegister<^^tests> _2 {__FILE__};
