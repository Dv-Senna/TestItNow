#include <print>

#include <TestItNow/entryPoint.hpp>
#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>
#include <TestItNow/macros.hpp>


TestItNow_NEW_TEST(test1, "tag1", "tag2", "tag3") {
	std::println("Test1 body");

	int a {10};
	int b {10};
	int c {12};
	TestItNow_ASSERT(a == c);
	TestItNow_REQUIRES(a == b);
	std::println("\033[34mEQUAL\033[m");
}
