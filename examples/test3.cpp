#include <print>

#include <TestItNow/test.hpp>


namespace tests {
//	[[=TestItNow::Test("test3"), =TestItNow::Tags("tag1")]]
	auto testBody3() -> TestItNow::TestResult {
		std::println("Test2 body");
		return {};
	};
}

//static TestItNow::testRegister<^^tests> _ {};
