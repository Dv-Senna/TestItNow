#include <print>

#include <TestItNow/entryPoint.hpp>
#include <TestItNow/test.hpp>
#include <TestItNow/testRegister.hpp>
#include <TestItNow/macros.hpp>
#include <TestItNow/generator.hpp>


TestItNow_NEW_TEST(test1, "tag1", "tag2", "tag3") {
	const auto value {TestItNow_GENERATE(std::views::iota(0uz, 5uz))};
	const auto value2 {TestItNow_GENERATE(std::views::iota(0uz, 2uz))};
	std::println("random value   : {}", value);
	std::println("random value 2 : {}", value2);

	int a {10};
	int b {10};
	int c {10};
	TestItNow_ASSERT(a == c);
	TestItNow_REQUIRES(a == b);
	TestItNow_ASSERT(a == c);
}
