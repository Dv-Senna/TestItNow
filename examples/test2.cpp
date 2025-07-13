#include <print>

#include <TestItNow/test.hpp>


namespace tests {
//	[[=TestItNow::Test, =TestItNow::Tags("tag1", "tag3")]]
	auto test2() -> TestItNow::TestResult {
		std::println("Test2 body");
		return {};
	};
}

//static TestItNow::testRegister<^^tests> _ {};
