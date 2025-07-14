#pragma once

#include <exception>
#include <expected>
#include <format>
#include <string_view>
#include <type_traits>
#include <vector>


namespace TestItNow {
	struct TestFailureInfo {
		std::string message;
	};
	using TestResult = std::expected<void, TestFailureInfo>;

	template <typename T>
	concept test_callback = std::invocable<std::remove_cvref_t<T>>
		&& std::same_as<std::invoke_result_t<std::remove_cvref_t<T>>, TestResult>;


	class Test final {
		Test() = delete;
		Test(const Test&) = delete;
		auto operator=(const Test&) -> Test& = delete;

		public:
			using Callback = TestResult(*)();

			inline Test(std::string_view name, std::vector<std::string_view>&& tags, Callback callback) noexcept :
				m_name {name},
				m_tags {std::move(tags)},
				m_callback {std::forward<Callback> (callback)}
			{}
			inline Test(Test&&) noexcept = default;
			inline auto operator=(Test&&) noexcept -> Test& = default;
			~Test() = default;

			inline auto run() const noexcept -> TestResult {
				try {
					return m_callback();
				}
				catch (std::exception &exception) {
					return std::unexpected(TestFailureInfo{std::format("Uncaught exception thrown in test {} : {}",
						m_name,
						exception.what()
					)});
				}
				catch (...) {
					return std::unexpected(TestFailureInfo{
						std::format("Uncaught and unknown exception thrown in test {}", m_name)
					});
				}
			}

			inline auto getName() const noexcept -> std::string_view {return m_name;}
			inline auto getTags() const noexcept -> const std::vector<std::string_view>& {return m_tags;}

		private:
			std::string_view m_name;
			std::vector<std::string_view> m_tags;
			Callback m_callback;
	};


	auto getTestList() noexcept -> std::vector<Test>&;
}
