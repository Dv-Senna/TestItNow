#pragma once

#include <string>



namespace tin {
	enum class Severity {
		eNormal,
		eSuccess,
		eFailure,
		eWarning
	};

	template <tin::Severity severity = tin::Severity::eNormal>
	void print(const std::string &message, ...);

} // namespace tin


#include "TestItNow/output.inl"