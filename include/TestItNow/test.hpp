#pragma once

#include <exception>
#include <expected>
#include <format>
#include <string_view>
#include <type_traits>
#include <vector>


namespace TestItNow {
	struct TestFailureInfos {
		std::string message;
	};
	using TestResult = std::expected<void, TestFailureInfos>;

	struct TestState {
		const std::uint32_t randomSeed;
		struct {
			std::uint32_t successCount;
			TestResult result {};
		} internals;
	};

	template <typename T>
	concept test_callback = std::invocable<std::remove_cvref_t<T>>
		&& std::same_as<std::invoke_result_t<std::remove_cvref_t<T>>, TestResult>;


	class Test final {
		Test() = delete;
		Test(const Test&) = delete;
		auto operator=(const Test&) -> Test& = delete;

		public:
			using Callback = void(*)(TestState&);

			inline Test(std::string_view name, std::vector<std::string_view>&& tags, Callback callback) noexcept :
				m_name {name},
				m_tags {std::move(tags)},
				m_callback {std::forward<Callback> (callback)}
			{}
			inline Test(Test&&) noexcept = default;
			inline auto operator=(Test&&) noexcept -> Test& = default;
			~Test() = default;

			inline auto run(std::uint32_t randomSeed) const noexcept -> std::expected<std::string, std::string> {
				try {
					TestState state {
						.randomSeed = randomSeed,
						.internals = {}
					};
					m_callback(state);
					if (!state.internals.result)
						return std::unexpected{state.internals.result.error().message};
					if (state.internals.successCount == 0)
						return std::format("\033[33mYour test seems to be empty\033[m");
					std::string output {std::format("\033[32m{} tests succeeded\033[m", state.internals.successCount)};
					return output;
				}
				catch (std::exception &exception) {
					return std::unexpected(std::format("Uncaught exception thrown in test {} : {}",
						m_name,
						exception.what()
					));
				}
				catch (...) {
					return std::unexpected(std::format("Uncaught and unknown exception thrown in test {}", m_name));
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
