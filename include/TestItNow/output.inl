#pragma once

#include "TestItNow/output.hpp"

#include <cstdio>
#include <format>
#include <string>



namespace tin {
	template <tin::Severity severity, typename ...Args>
	void print(std::string_view message, Args&& ...args) {
		std::string output {std::vformat(message, std::make_format_args(args...))};
		std::string severityFormat {"\033[0m"};

		if constexpr (severity == tin::Severity::eSuccess)
			severityFormat = "\033[92m";
		if constexpr (severity == tin::Severity::eFailure)
			severityFormat = "\033[31m";
		if constexpr (severity == tin::Severity::eWarning)
			severityFormat = "\033[33m";

		if constexpr (severity == tin::Severity::eSuccess || severity == tin::Severity::eNormal)
			printf("%s%s\033[0m\n", severityFormat.c_str(), output.c_str());
		else
			fprintf(stderr, "%s%s\033[0m\n", severityFormat.c_str(), output.c_str());
	}


} // namespace tin