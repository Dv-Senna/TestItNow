#pragma once

#include <string>

#include "TestItNow/output.hpp"
#include "TestItNow/result.hpp"



namespace tin {
	class Test {
		public:
			using Callback = tin::Result (*)();

			Test(const std::string &name, Callback test);
			~Test() = default;
	};

} // namespace tin


#define TIN_START_TEST(name) tin::Result name##_testFunc() {uint64_t __tin_successCount {0}; uint64_t __tin_testCount {0};
#define TIN_END_TEST(name, description) if (__tin_successCount >= __tin_testCount) {} else return tin::Result::eFailed;\
	return tin::Result::ePassed;\
	}; tin::Test name##_testObject {#name, name##_testFunc};

#define TIN_CHECK(...) __tin_testCount++;\
	if (!!(__VA_ARGS__)) {__tin_successCount++;}\
	else {tin::print<tin::Severity::eFailure> ("\tCheck failed in " __FILE__ ":{} > " #__VA_ARGS__, __LINE__);}

#define TIN_NAMED_CHECK(name, ...) __tin_testCount++;\
	if (!!(__VA_ARGS__)) {__tin_successCount++;}\
	else {tin::print<tin::Severity::eFailure> ("\tCheck '" name "' failed");}

#define TIN_MANDATORY_CHECK(...) __tin_testCount++;\
	if (!!(__VA_ARGS__)) {__tin_successCount++;}\
	else {\
		tin::print<tin::Severity::eFailure> ("\tMandatory check failed in " __FILE__ ":{} > " #__VA_ARGS__, __LINE__);\
		return tin::Result::eFailed;\
	}

#define TIN_NAMED_MANDATORY_CHECK(name, ...) __tin_testCount++;\
	if (!!(__VA_ARGS__)) {__tin_successCount++;}\
	else {\
		tin::print<tin::Severity::eFailure> ("\tMandatory check '" name "' failed");\
		return tin::Result::eFailed;\
	}
