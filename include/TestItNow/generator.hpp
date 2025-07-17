#pragma once

#include <cassert>
#include <ranges>


namespace TestItNow {
	class BasicGeneratorWrapper {
		public:
			virtual constexpr ~BasicGeneratorWrapper() = default;
			virtual auto advance() noexcept -> void = 0;
			virtual auto isEmpty() const noexcept -> bool = 0;
	};

	template <std::ranges::input_range T>
	class GeneratorWrapper final : public BasicGeneratorWrapper {
		public:
			constexpr GeneratorWrapper(auto&&... args) :
				m_range {std::forward<decltype(args)> (args)...},
				m_begin {std::ranges::begin(m_range)}
			{}

			inline auto getValue() noexcept {
				assert(m_begin != std::ranges::end(m_range) && "You can't get value from an empty generator");
				return *m_begin;
			}

			inline auto advance() noexcept -> void override {
				assert(m_begin != std::ranges::end(m_range) && "You can't advance an empty generator");
				++m_begin;
			}

			inline auto isEmpty() const noexcept -> bool override {
				return m_begin == std::ranges::end(m_range);
			}


		private:
			T m_range;
			std::ranges::iterator_t<T> m_begin;
	};
}


#ifdef TestItNow_GENERATE
	#error Macro TestItNow_GENERATE is used by TestItNow. It must not be defined
	#include <stop_compilation>
#endif
#define TestItNow_GENERATE(...) [&TestItNow_state]() { \
		static std::optional<std::size_t> generatorIndex {std::nullopt}; \
		if (!generatorIndex) { \
			generatorIndex = TestItNow_state.internals.generators.size(); \
			TestItNow_state.internals.generators.push_back({}); \
		} \
 \
		auto& generatorPtr {TestItNow_state.internals.generators[*generatorIndex]}; \
		using RangeType = decltype(__VA_ARGS__); \
		using Wrapper = TestItNow::GeneratorWrapper<RangeType>; \
 \
		if (generatorPtr) \
			return static_cast<Wrapper&> (*generatorPtr).getValue(); \
		generatorPtr = std::make_unique<Wrapper> (__VA_ARGS__); \
		return static_cast<Wrapper&> (*generatorPtr).getValue(); \
	} ()
