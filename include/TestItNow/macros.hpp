#pragma once

#include <expected>
#include <format>
#include <print>
#include <sstream>
#include <string>
#include <string_view>


namespace TestItNow::internals {
	template <typename T>
	concept formattable = requires(T value) {
		typename std::formatter<std::remove_cvref_t<T>>;
	};

	template <typename T>
	concept ostream_pipable = requires(std::ostream str, T value) {
		str << value;
	};

	template <typename T>
	struct Stringifier {
		constexpr auto operator()(const T& value) const {
			if constexpr (formattable<T>)
				return std::format("{}", value);
			else if constexpr (ostream_pipable<T>) {
				std::ostringstream stream {};
				stream << value;
				return stream.str();
			}
			else
				return std::to_string(value);
		}
	};

	struct ExpressionInfos {
		std::string_view expression;
		std::string_view file;
		std::uint32_t line;
	};

#ifdef TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL
	#error Macro TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL is used by TestItNow. You must not define it
	#include <stop_compilation>
#endif
#define TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(func, operator, inverseOperator) \
	constexpr auto func(auto&& rhs) const noexcept -> std::expected<void, std::string> { \
		decltype(rhs) value {::std::forward<decltype(rhs)> (rhs)}; \
		if (m_value operator value) \
			return {}; \
		::TestItNow::internals::Stringifier<::std::remove_reference_t<T>> lhsStringifier {}; \
		::TestItNow::internals::Stringifier<::std::remove_reference_t<decltype(rhs)>> rhsStringifier {}; \
		return ::std::unexpected{::std::format("Requirement '{}' ({}:{}) failed : {} " #inverseOperator " {}", \
			m_infos.expression, m_infos.file, m_infos.line, \
			lhsStringifier(m_value), rhsStringifier(value) \
		)}; \
	}

	template <typename T>
	class ExpressionLeftHandSide {
		public:
			constexpr ExpressionLeftHandSide(const ExpressionInfos& infos, T&& value) noexcept :
				m_infos {infos},
				m_value {std::forward<T> (value)}
			{}

			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator==, ==, !=)
			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator!=, !=, ==)
			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator<, <, >=)
			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator<=, <=, >)
			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator>, >, <=)
			TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL(operator>=, >=, <)
 
		private:
			ExpressionInfos m_infos;
			T m_value;
	};

#undef TestItNow_EXPR_LHS_CMP_OPERATOR_IMPL

	class ExpressionDestructor {
		public:
			constexpr ExpressionDestructor(ExpressionInfos&& infos) noexcept : m_infos {std::move(infos)} {}
			constexpr auto operator<(auto&& lhs) const noexcept {
				return ExpressionLeftHandSide<decltype(lhs)> {m_infos, std::forward<decltype(lhs)> (lhs)};
			}

		private:
			ExpressionInfos m_infos;
	};
}


#ifdef TestItNow_DESTRUCT_EXPR
	#error Macro TestItNow_DESTRUCT_EXPR is used by TestItNow. It must not be defined
	#include <stop_compilation>
#endif
#define TestItNow_DESTRUCT_EXPR(...) (::TestItNow::internals::ExpressionDestructor{ \
		::TestItNow::internals::ExpressionInfos{#__VA_ARGS__, __FILE__, __LINE__} \
	} < __VA_ARGS__)

#ifdef TestItNow_REQUIRES
	#error Macro TestItNow_REQUIRES is used by TestItNow. It must not be defined
	#include <stop_compilation>
#endif
#define TestItNow_REQUIRES(...) do { \
		++TestItNow_state.internals.testCount; \
		auto TestItNow_exprResult_##__LINE__ {TestItNow_DESTRUCT_EXPR(__VA_ARGS__)}; \
		if (!TestItNow_exprResult_##__LINE__) { \
			TestItNow_state.internals.errors.push_back(TestItNow_exprResult_##__LINE__.error()); \
			TestItNow_state.internals.requirementNotFullfilled = true; \
			return; \
		} \
		++TestItNow_state.internals.successCount; \
	} while (false)

#ifdef TestItNow_ASSERT
	#error Macro TestItNow_ASSERT is used by TestItNow. It must not be defined
	#include <stop_compilation>
#endif
#define TestItNow_ASSERT(...) do { \
		++TestItNow_state.internals.testCount; \
		auto TestItNow_exprResult_##__LINE__ {TestItNow_DESTRUCT_EXPR(__VA_ARGS__)}; \
		if (!TestItNow_exprResult_##__LINE__) \
			TestItNow_state.internals.errors.push_back(TestItNow_exprResult_##__LINE__.error()); \
		else \
			++TestItNow_state.internals.successCount; \
	} while (false)
