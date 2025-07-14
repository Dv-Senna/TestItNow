#include <print>

#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>


TestItNow_NEW_TEST(test3, "tag1") {
	std::println("Test3 body");
	return {};
}

TestItNow_NEW_TEST(test3_2) {
	std::println("Test3_2 body");
	return {};
}
