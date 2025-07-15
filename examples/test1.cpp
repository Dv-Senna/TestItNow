#include <print>

#include <TestItNow/entryPoint.hpp>
#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>
#include <TestItNow/macros.hpp>


TestItNow_NEW_TEST(test1, "tag1", "tag2", "tag3") {
	std::println("Test1 body");

	int a {10};
	int b {12};
	auto cmpRes {TestItNow::internals::ExpressionDestructor{
		TestItNow::internals::ExpressionInfos{"a == b", __FILE__, __LINE__}
	} < a == b};
	if (cmpRes)
		std::println("EQUAL");
	else
		return std::unexpected(TestItNow::TestFailureInfo{std::format("\033[31mFailure: {}\033[m", cmpRes.error())});

	return {};
}
