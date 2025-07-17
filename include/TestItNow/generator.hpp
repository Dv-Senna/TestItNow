#pragma once

#include <cassert>
#include <concepts>
#include <random>
#include <ranges>


namespace TestItNow {
	class BasicGeneratorWrapper {
		public:
			virtual constexpr ~BasicGeneratorWrapper() = default;
			virtual auto advance() noexcept -> void = 0;
			virtual auto isEmpty() noexcept -> bool = 0;
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

			inline auto isEmpty() noexcept -> bool override {
				return m_begin == std::ranges::end(m_range);
			}


		private:
			T m_range;
			std::ranges::iterator_t<T> m_begin;
	};


	struct TestState;


	namespace generators {
		template <typename Distribution>
		class Random;

		template <typename Distribution>
		class RandomIterator {
			friend class Random<Distribution>;
			using This = RandomIterator<Distribution>;
			public:
				using difference_type = std::ptrdiff_t;
				using value_type = typename Distribution::result_type;

				constexpr RandomIterator() noexcept = default;
				constexpr RandomIterator(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr RandomIterator(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This&) const noexcept -> bool = default;

				inline auto operator++() -> This&;
				inline auto operator++(int) -> This {auto tmp {*this}; ++(*this); return tmp;}

				inline auto operator*() const noexcept -> value_type {return m_value;}


			private:
				inline RandomIterator(Random<Distribution>& randomGenerator) noexcept :
					m_value {},
					m_randomGenerator {&randomGenerator}
				{}

				value_type m_value;
				Random<Distribution>* m_randomGenerator;
		};

		template <typename Distribution>
		class Random {
			public:
				using iterator = RandomIterator<Distribution>;

				inline Random(TestItNow::TestState& state, auto&&... args);
				constexpr Random(std::uint32_t seed, auto&&... args) :
					m_distribution {std::forward<decltype(args)> (args)...},
					m_engine {seed}
				{}

				inline auto generate() {
					return m_distribution(m_engine);
				}

				inline auto begin() noexcept {return iterator{*this};}
				inline auto end() const noexcept {return iterator{};}


			private:
				Distribution m_distribution;
				std::mt19937 m_engine;
		};

		static_assert(std::input_iterator<Random<std::uniform_int_distribution<std::uint32_t>>::iterator>);
		static_assert(std::ranges::input_range<Random<std::uniform_int_distribution<std::uint32_t>>>);

		template <typename Distribution>
		inline auto RandomIterator<Distribution>::operator++() -> This& {
			m_value = m_randomGenerator->generate();
			return *this;
		}


		template <typename Distribution>
		class RandomFactory final {
			public:
				constexpr auto operator()(auto&&... args) const {
					return Random<Distribution> {std::forward<decltype(args)> (args)...};
				}
		};

		class UniformRandomFactory final {
			public:
				constexpr auto operator()(auto&& first, std::integral auto&&... args) const {
					return Random<std::uniform_int_distribution<std::remove_cvref_t<decltype(args...[0])>>> {
						std::forward<decltype(first)> (first), std::forward<decltype(args)> (args)...
					};
				}

				constexpr auto operator()(auto&& first, std::floating_point auto&&... args) const {
					return Random<std::uniform_real_distribution<std::remove_cvref_t<decltype(args...[0])>>> {
						std::forward<decltype(first)> (first), std::forward<decltype(args)> (args)...
					};
				}
		};


		template <typename Distribution>
		constexpr RandomFactory<Random<Distribution>> random {};

		constexpr UniformRandomFactory uniform_random {};
	}
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
		TestItNow_state.internals.generatorStackTop = *generatorIndex; \
		generatorPtr = std::make_unique<Wrapper> (__VA_ARGS__); \
		return static_cast<Wrapper&> (*generatorPtr).getValue(); \
	} ()
