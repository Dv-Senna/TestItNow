#pragma once

#include <format>



namespace tin {
	enum class Severity {
		eNormal,
		eSuccess,
		eFailure,
		eWarning
	};

	template <tin::Severity severity = tin::Severity::eNormal, typename ...Args>
	void print(std::format_string<Args...> message, Args &&...args);

} // namespace tin


#include "TestItNow/output.inl"
