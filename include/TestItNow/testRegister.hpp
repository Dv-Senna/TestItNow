#pragma once

#include <meta>
#include <ranges>
#include <type_traits>
#include <utility>

#include "TestItNow/test.hpp"


namespace TestItNow {
	namespace internals {
		template <typename ...Ts>
		struct StructuralTuple;

		template <typename First, typename ...Tails>
		struct StructuralTuple<First, Tails...> : public StructuralTuple<Tails...> {
			First data;
		};

		template <>
		struct StructuralTuple<> {};


		template <std::size_t I, typename ...Ts>
		requires (I < sizeof...(Ts))
		constexpr auto getFromStructuralTuple(StructuralTuple<Ts...>& tuple) noexcept {
			constexpr auto implementation {[] <std::size_t J = 0, typename First, typename ...Tails> (
				this const auto& implementation,
				StructuralTuple<First, Tails...>& tuple
			) {
				if constexpr (J == I)
					return tuple.StructuralTuple<First, Tails...>::data;
				else
					return implementation.template operator() <J + 1, Tails...> (tuple);
			}};
			return implementation(tuple);
		}


		template <typename T>
		struct extract_fixed_size_string_size;

		template <std::integral auto N>
		struct extract_fixed_size_string_size<const char[N]> {
			static constexpr auto value {static_cast<std::size_t> (N)};
		};

		template <std::integral auto N>
		struct extract_fixed_size_string_size<const char(&)[N]> {
			static constexpr auto value {static_cast<std::size_t> (N)};
		};


		template <std::size_t... Ns>
		struct StringAnnotation {
			StructuralTuple<char[Ns]...> strings;
		};

		template <std::size_t N>
		struct StringAnnotation<N> {
			char string[N];
		};

		template <>
		struct StringAnnotation<> {};

		template <>
		struct StringAnnotation<0uz> {};

		template <template <std::size_t...> typename ToBuild, bool allowMultiple, bool allowNone>
		struct StringAnnotationProxy {
			consteval auto operator()() const noexcept requires (allowNone) {
				return ToBuild<0uz> {};
			}

			template <std::size_t N>
			consteval auto operator()(const char (&value)[N]) const noexcept {
				ToBuild<N> result {};
				for (const auto i : std::views::iota(0uz, N))
					result.string[i] = value[i];
				return result;
			}

			consteval auto operator()(auto&... values) const noexcept requires (allowMultiple) {
				ToBuild<extract_fixed_size_string_size<decltype(values)>::value...> result {};
				template for (constexpr auto i : std::define_static_array(
					std::views::iota(0uz, sizeof...(values))
				)) {
					for (const auto j : std::views::iota(
						0uz, std::get<i> (std::tuple{extract_fixed_size_string_size<decltype(values)>::value...})
					))
						getFromStructuralTuple<i> (result.strings)[j] = (values...[i])[j];
				}
				return result;
			}
		};

		static_assert(std::same_as<
			decltype(std::declval<StringAnnotationProxy<StringAnnotation, true, true>> ()("hi", "hoo", "hum?")),
			StringAnnotation<3uz, 4uz, 5uz>
		>);
	}

	namespace annotations {
		template <std::size_t N>
		struct TestType : internals::StringAnnotation<N> {};
		struct TestTypeProxy : internals::StringAnnotationProxy<TestType, false, true> {};

		template <std::size_t ...Ns>
		struct TagsType : internals::StringAnnotation<Ns...> {};
		struct TagsTypeProxy : internals::StringAnnotationProxy<TagsType, true, true> {};
	}

	constexpr annotations::TestTypeProxy Test {};
	constexpr annotations::TagsTypeProxy Tags {};
}
