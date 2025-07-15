#pragma once

#include <meta>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

#include "TestItNow/test.hpp"


namespace TestItNow {
	template <TestItNow::TestResult (*Func)()>
	struct TestRegister {
		inline TestRegister(std::string_view name, auto... rawTags) {
			std::vector<std::string_view> tags {rawTags...};
			TestItNow::getTestList().emplace_back(name, std::move(tags), Func);
		}
	};
}

#ifdef TestItNow_NEW_TEST
	#error Macro TestItNow_NEW_TEST must be undefined because TestItNow defines it
	#include <stop_include>
#endif
#define TestItNow_NEW_TEST(name, ...) auto TestItNow_test_##name##body() -> ::TestItNow::TestResult; \
	[[maybe_unused]] \
	static ::TestItNow::TestRegister<&TestItNow_test_##name##body> TestItNow_register_##name { \
		#name, __VA_ARGS__ \
	}; \
	auto TestItNow_test_##name##body() -> ::TestItNow::TestResult
