#pragma once

#include <string_view>



namespace tin {
	enum class Severity {
		eNormal,
		eSuccess,
		eFailure,
		eWarning
	};

	template <tin::Severity severity = tin::Severity::eNormal, typename ...Args>
	void print(std::string_view message, Args&& ...args);

} // namespace tin


#include "TestItNow/output.inl"