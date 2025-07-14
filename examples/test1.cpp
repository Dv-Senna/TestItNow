#include <print>

#include <TestItNow/entryPoint.hpp>
#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>


TestItNow_NEW_TEST(test1, "tag1", "tag2", "tag3") {
	std::println("Test1 body");
	return {};
}
