#pragma once

#include <meta>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

#include "TestItNow/test.hpp"


namespace TestItNow {
	namespace internals {
		template <typename T>
		struct StructuralSpan {
			T* m_begin;
			T* m_end;

			using This = StructuralSpan<T>;
			constexpr StructuralSpan() noexcept = default;
			constexpr StructuralSpan(const This&) noexcept = default;
			constexpr auto operator=(const This&) noexcept -> This& = default;
			constexpr StructuralSpan(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;

			constexpr StructuralSpan(T* begin, T* end) noexcept :
				m_begin {begin},
				m_end {end}
			{}
			constexpr StructuralSpan(std::span<T> span) noexcept :
				m_begin {span.data()},
				m_end {span.data() + span.size()}
			{}

			explicit constexpr operator std::span<T> () const noexcept {
				return std::span{m_begin, m_end};
			}
		};


		template <std::meta::info r, typename T>
		consteval auto hasAnnotation() -> bool {
			std::size_t count {0};
			template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
				if constexpr (type_of(a) == ^^T)
					++count;
			}
			if (count > 1)
				throw "Can't have multiple instance of the same annotation";
			return count == 1;
		}

		template <std::meta::info r, typename T>
		consteval auto getAnnotation() {
			template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
				using AnnotationType = [:type_of(a):];
				if constexpr (type_of(a) == ^^T)
					return extract<AnnotationType> (a);
			}
			throw "Can't get annotation that is not present on reflection";
		}
	}


	namespace annotations {
		struct TestBase {
			bool hasValue;
			const char* value;

			template <std::size_t N>
			consteval auto operator()(const char (&value)[N]) const noexcept {
				return TestBase{
					.hasValue = true,
					.value = std::define_static_string(value)
				};
			}
		};


		struct TagsBase {
			internals::StructuralSpan<const char* const> values;
		};

		struct TagsBaseProxy {
			consteval auto operator()(auto& ...values) const noexcept {
				std::vector staticValues {std::define_static_string(values)...};
				return TagsBase{.values = std::define_static_array(staticValues)};
			};
		};
	}

	constexpr annotations::TestBase Test {.hasValue = false, .value = nullptr};
	constexpr annotations::TagsBaseProxy Tags {};



	namespace internals {
		struct TestInfos {
			std::string_view name;
			std::span<const char* const> tags;
			//std::span<const char* const> tags;
		};

		template <std::meta::info f>
		consteval auto getTestInfos() -> TestInfos {
			if constexpr (!is_function(f))
				throw "Can't apply Test annotation to non-function";

			TestInfos result {};
			constexpr auto test {getAnnotation<f, annotations::TestBase> ()};
			if constexpr (!test.hasValue)
				result.name = std::string_view{identifier_of(f)};
			else
				result.name = std::string_view{test.value};

			if constexpr (hasAnnotation<f, annotations::TagsBase> ()) {
				static constexpr auto tags {getAnnotation<f, annotations::TagsBase> ()};
				result.tags = static_cast<std::span<const char* const>> (tags.values);
			}

			return result;
		}
	}


	template <std::meta::info Namespace>
	struct TestRegister {
		inline TestRegister(std::string_view file) {
			constexpr auto ctx {std::meta::access_context::current()};
			template for (constexpr auto member : std::define_static_array(members_of(Namespace, ctx))) {
				if constexpr (internals::hasAnnotation<member, annotations::TestBase> ()) {
					std::println("member {} in {}", display_string_of(member), file);
					constexpr internals::TestInfos testInfos {internals::getTestInfos<member> ()};
					std::println("Test infos of {} in {} : {} {}", identifier_of(member), file, testInfos.name, testInfos.tags);
				}
			}
		}
	};
}
