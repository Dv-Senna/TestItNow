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
			std::vector<std::string> errors;
			bool requirementNotFullfilled;
			std::vector<std::unique_ptr<TestItNow::BasicGeneratorWrapper>> generators;
			std::size_t generatorStackTop;
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

			auto run(std::uint32_t randomSeed) const noexcept -> std::expected<std::string, std::string>;

			inline auto getName() const noexcept -> std::string_view {return m_name;}
			inline auto getTags() const noexcept -> const std::vector<std::string_view>& {return m_tags;}

		private:
			auto m_testInstantiation(TestState& state) const
				-> std::expected<std::pair<bool, std::string>, std::string>;

			std::string_view m_name;
			std::vector<std::string_view> m_tags;
			Callback m_callback;
	};


	auto getTestList() noexcept -> std::vector<Test>&;
}
