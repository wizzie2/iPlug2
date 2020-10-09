#pragma once

/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/


namespace iplug::type  // move everything to iplug namespace?
{
	struct _internal
	{
		template <class>
		static constexpr bool Always_false = false;

		template <class T>
		struct InvalidType
		{
			static_assert(Always_false<T>, "Invalid type");
		};
	};

	// Copy of MSVC internal std::_Is_any_of_v implementation
	template <class T, class... Types>
	inline constexpr bool IsAnyOf = std::disjunction_v<std::is_same<T, Types>...>;

	// No bool, char, wchar_t, char16_t or char32_t
	template <class T>
	inline constexpr bool IsIntegral = IsAnyOf<std::remove_cv_t<T>,  // remove 'const' and 'volatile' qualifiers
											   uint8,
											   uint16,
											   uint32,
											   uint64,
											   int8,
											   int16,
											   int32,
											   int64,
											   signed char,
											   unsigned char,
											   short,
											   unsigned short,
											   int,
											   unsigned int,
											   long,
											   unsigned long,
											   long long,
											   unsigned long long>;

	template <class Tx, class Ty>
	inline constexpr bool IsSame = std::is_same_v<std::remove_cv_t<Tx>, Ty>;

	template <class T>
	inline constexpr bool IsFloatingPoint = std::is_floating_point_v<T>;

	template <class T>
	inline constexpr bool IsSigned = std::is_signed_v<T>;

	template <class T>
	inline constexpr bool IsUnsigned = std::is_unsigned_v<T>;

	template <class T>
	inline constexpr bool IsFloat = IsSame<T, float>;

	template <class T>
	inline constexpr bool IsDouble = IsSame<T, double>;

	// Using modified is_integral_v without bool or char types
	template <class T>
	inline constexpr bool IsArithmetic = IsIntegral<T> || IsFloatingPoint<T>;

	template <class T>
	inline constexpr bool IsEnumClass = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	template <class T>
	using EnableWhenFloatingtPoint = std::enable_if_t<IsFloatingPoint<T>, int>;

	template <class T>
	using EnableWhenIntegral = std::enable_if_t<IsIntegral<T>, int>;

	template <class T>
	using EnableWhenArethmatic = std::enable_if_t<IsArithmetic<T>, int>;

	// clang-format off

	// Returns uint8, uint16, uint32 or uint64 based on the size of T
	template <class T>
	using ConditionalUIntSize = std::conditional_t<sizeof(T) == 1, uint8,
		                        std::conditional_t<sizeof(T) == 2, uint16,
		                        std::conditional_t<sizeof(T) == 4, uint32,
		                        std::conditional_t<sizeof(T) == 8, uint64,
		                        _internal::InvalidType<T>>>>>;

	// Returns int8, int16, int32 or int64 based on the size of T
	template <class T>
	using ConditionalIntSize =  std::conditional_t<sizeof(T) == 1, int8,
		                        std::conditional_t<sizeof(T) == 2, int16,
		                        std::conditional_t<sizeof(T) == 4, int32,
		                        std::conditional_t<sizeof(T) == 8, int64,
		                        _internal::InvalidType<T>>>>>;

	// Emulate C++20 bit_cast, or if available use the real bit_cast
	#if !defined __cpp_lib_bit_cast || defined DOXYGEN_GENERATING_OUTPUT
		template <class To,
				  class From,
				  std::enable_if_t<std::conjunction_v<std::bool_constant<sizeof(To) == sizeof(From)>,
													  std::is_trivially_copyable<To>,
													  std::is_trivially_copyable<From>>,
								   int> = 0>
		NODISCARD inline constexpr To bit_cast(const From& type) noexcept
		{
			To result;
			memcpy(&result, &type, sizeof(To));
			return result;
		}
	#else
		template <class To, class From>
		using bit_cast = ::std::bit_cast<To, From>;
	#endif
	// clang-format on

	struct EnumHash
	{
		template <class T>
		inline constexpr auto operator()(const T& index) const
			-> std::enable_if_t<std::is_enum_v<T>, std::underlying_type_t<T>>
		{
			return std::hash<std::underlying_type_t<T>>()(static_cast<std::underlying_type_t<T>>(index));
		}
	};

	template <class T>
	using EnumStringMap = std::unordered_map<T, const char*, EnumHash>;

}  // namespace iplug::type


namespace iplug
{
	/**
	 * Shortcut to cast enum class members to their underlying type. Prefix "+" to do an automatic
	 * "static_cast<underlying_type>(Enum)". Default underlying type for 'enum class' is int if not specified.
	 *
	 * @param value Enum member to cast.
	 * @return value of Enum members casted to an underlying type.
	 */
	template <class T>
	inline constexpr auto operator+(const T& value) -> std::enable_if_t<type::IsEnumClass<T>, std::underlying_type_t<T>>
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

}  // namespace iplug
