#pragma once

#include <exception>
#include <expected>
#include <format>
#include <memory>
#include <sstream>
#include <string_view>
#include <vector>

#include "TestItNow/generator.hpp"


namespace TestItNow {
	struct TestState {
		const std::uint32_t randomSeed;
		struct {
			std::uint32_t currentRandomSeed;
			std::uint32_t successCount;
			std::uint32_t testCount;
			std::vector<std::string> errors {};
			bool requirementNotFullfilled;
			std::vector<std::unique_ptr<TestItNow::BasicGeneratorWrapper>> generators;
		} internals;
	};


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
						.internals = {
							.currentRandomSeed = randomSeed,
							.successCount = 0,
							.testCount = 0,
							.errors = {},
							.requirementNotFullfilled = false,
							.generators = {}
						}
					};

					while (true) {
						m_callback(state);

						if (state.internals.testCount == 0)
							return std::format("\033[33mYour test seems to be empty\033[m");

						if (state.internals.testCount != state.internals.successCount) {
							std::ostringstream stream {};
							for (std::string_view delim {""}; const auto& str : state.internals.errors) {
								stream << delim << str;
								delim = "\n\t";
							}
							if (!state.internals.requirementNotFullfilled) {
								stream << std::format("\n{}/{} tests succeeded",
									state.internals.successCount,
									state.internals.testCount
								);
							}
							return std::unexpected{stream.str()};
						}

						if (state.internals.generators.empty())
							break;

						state.internals.generators[0]->advance();
						if (state.internals.generators[0]->isEmpty())
							break;
					}

					std::string output {std::format("\033[32m{}/{} tests succeeded\033[m",
						state.internals.successCount,
						state.internals.testCount
					)};
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
