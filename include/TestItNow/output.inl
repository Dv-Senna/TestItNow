#pragma once

#include "TestItNow/output.hpp"

#include <print>
#include <utility>


namespace tin {
	template <tin::Severity severity, typename ...Args>
	void print(std::format_string<Args...> message, Args &&...args) {
		std::string output {std::format(message, std::forward<Args> (args)...)};
		std::string severityFormat {"\033[0m"};

		if constexpr (severity == tin::Severity::eSuccess)
			severityFormat = "\033[92m";
		if constexpr (severity == tin::Severity::eFailure)
			severityFormat = "\033[31m";
		if constexpr (severity == tin::Severity::eWarning)
			severityFormat = "\033[33m";

		if constexpr (severity == tin::Severity::eSuccess || severity == tin::Severity::eNormal)
			std::println("{}{}\033[0m", severityFormat, output);
		else
			std::println(stderr, "{}{}\033[0m", severityFormat, output);
	}


} // namespace tin


#ifdef TIN_DEFER
	#undef TIN_DEFER
#endif
