#pragma once

#include <map>
#include <string_view>
#include <string>

#include "TestItNow/result.hpp"



namespace tin {
	class TestManager {
		public:
			using TestCallback = tin::Result (*)();

			static void addTest(const std::string &name, TestCallback testFunc);
			static void runTest(const std::string &testName);
			static void cleanup();

		private:
			TestManager();

			static std::map<std::string, TestCallback> *s_tests;
	};

} // namespace tin