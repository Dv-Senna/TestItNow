#pragma once

#include "TestItNow/output.hpp"

#include <iostream>
#include <cstdio>
#include <cstdarg>


#ifndef TIN_DEFER
	struct tin_defer_dummy {};
	template <class F> struct tin_deferrer { F f; ~tin_deferrer() { f(); } };
	template <class F> tin_deferrer<F> operator*(tin_defer_dummy, F f) { return {f}; }
	#define TIN_DEFER2_(LINE) zz_tin_defer##LINE
	#define TIN_DEFER_(LINE) TIN_DEFER2_(LINE)
	#define TIN_DEFER auto TIN_DEFER_(__LINE__) = tin_defer_dummy{} *[&]()
#endif


namespace tin {
	template <tin::Severity severity>
	void print(const std::string &message, ...) {
		va_list args {};
		va_start(args, message);
		TIN_DEFER {va_end(args);};

		int stringSize {vsnprintf(nullptr, 0, message.c_str(), args)};
		std::string output {};
		output.resize(stringSize + 1);
		(void)vsnprintf(output.data(), output.size(), message.c_str(), args);


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


#ifdef TIN_DEFER
	#undef TIN_DEFER
#endif