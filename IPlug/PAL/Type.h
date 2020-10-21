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

	// No long double
	template <class T>
	inline constexpr bool IsFloatingPoint = IsAnyOf<std::remove_cv_t<T>,  // remove 'const' and 'volatile' qualifiers
													float,
													double>;

	template <class Tx, class Ty>
	inline constexpr bool IsSame = std::is_same_v<std::remove_cv_t<Tx>, std::remove_cv_t<Ty>>;

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
	using EnableWhenArithmetic = std::enable_if_t<IsArithmetic<T>, int>;

	// clang-format off

	// Returns a type of uint8, uint16, uint32 or uint64 depending on the size of T
	template <class T>
	using uint_sizeof = std::conditional_t<sizeof(T) == 1, uint8,
						std::conditional_t<sizeof(T) == 2, uint16,
						std::conditional_t<sizeof(T) == 4, uint32,
						std::conditional_t<sizeof(T) == 8, uint64,
						_internal::InvalidType<T>>>>>;

	// Returns a type of int8, int16, int32 or int64 based on the size of T
	template <class T>
	using int_sizeof = std::conditional_t<sizeof(T) == 1, int8,
					   std::conditional_t<sizeof(T) == 2, int16,
					   std::conditional_t<sizeof(T) == 4, int32,
					   std::conditional_t<sizeof(T) == 8, int64,
					   _internal::InvalidType<T>>>>>;

	// Returns a type of int8, int16, int32 ,int64, uint8, uint16, uint32 or uint64 based on the size and signess of T
	template <class T>
	using type_sizeof = std::conditional_t<std::is_signed_v<T>, type::int_sizeof<T>, type::uint_sizeof<T> >;


	template <class To, class From,
			  std::enable_if_t<std::conjunction_v<std::bool_constant<sizeof(To) == sizeof(From)>,
												  std::is_trivially_copyable<To>,
												  std::is_trivially_copyable<From>>, int> = 0>
	NODISCARD constexpr To bit_cast(const From& type) noexcept
	{
		// Use std::bit_cast if available, otherwise emulate it
		#ifdef __cpp_lib_bit_cast
			return std::bit_cast<To>(type);
		#else
			To result;
			memcpy(&result, &type, sizeof(To));
			return result;
		#endif
	}

	// clang-format on

	struct EnumHash
	{
		template <class T>
		constexpr auto operator()(const T& index) const
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
	// enum class operators

	/**
	 * Shortcut to cast enum class members to their underlying type. Prefix "+" to do an automatic
	 * "static_cast<underlying_type>(Enum)". Default underlying type for 'enum class' is int if not specified.
	 *
	 * @param value Enum member to cast.
	 * @return value of Enum members casted to an underlying type.
	 */
	template <class T>
	inline constexpr auto operator+(const T& e) -> std::enable_if_t<type::IsEnumClass<T>, std::underlying_type_t<T>>
	{
		return static_cast<std::underlying_type_t<T>>(e);
	}


	// Equal To operator for EnumClass/Integer or Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator==(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>),
							bool>
	{
		if constexpr (type::IsEnumClass<Ta>)
			return +lhs == rhs;
		else
			return lhs == +rhs;
	}


	// Not Equal To operator for EnumClass/Integer or Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator!=(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(std::is_integral_v<Ta> && type::IsEnumClass<Tb>),
							bool>
	{
		return !(lhs == rhs);
	}


	// Bitwise NOT for EnumClass
	template <class Ta>
	inline constexpr auto operator~(const Ta& e) -> std::enable_if_t<type::IsEnumClass<Ta>, Ta>
	{
		return static_cast<Ta>(~+e);
	}


	// Logical NOT for EnumClass
	template <class Ta>
	inline constexpr auto operator!(const Ta& e) -> std::enable_if_t<type::IsEnumClass<Ta>, bool>
	{
		return 0 == e;
	}


	// Bitwise AND for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator&(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		if constexpr (type::IsEnumClass<Ta> && type::IsEnumClass<Tb>)
			return static_cast<Ta>(+lhs & +rhs);
		else if constexpr (type::IsEnumClass<Ta>)
			return static_cast<Ta>(+lhs & rhs);
		else
			return lhs & +rhs;
	}


	// Bitwise OR for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator|(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		if constexpr (type::IsEnumClass<Ta> && type::IsEnumClass<Tb>)
			return static_cast<Ta>(+lhs | +rhs);
		else if constexpr (type::IsEnumClass<Ta>)
			return static_cast<Ta>(+lhs | rhs);
		else
			return lhs | +rhs;
	}


	// Bitwise shift left for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator<<(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		if constexpr (type::IsEnumClass<Ta> && type::IsEnumClass<Tb>)
			return static_cast<Ta>(+lhs << +rhs);
		else if constexpr (type::IsEnumClass<Ta>)
			return static_cast<Ta>(+lhs << rhs);
		else
			return lhs << +rhs;
	}


	// Bitwise shift right for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator>>(const Ta& lhs, const Tb& rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		if constexpr (type::IsEnumClass<Ta> && type::IsEnumClass<Tb>)
			return static_cast<Ta>(+lhs >> +rhs);
		else if constexpr (type::IsEnumClass<Ta>)
			return static_cast<Ta>(+lhs >> rhs);
		else
			return lhs >> +rhs;
	}

	// Bitwise OR assignment for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator|=(Ta& lhs, const Tb rhs)
		-> std::enable_if_t<((std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
							 type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		return lhs = lhs | rhs;
	}

	// Bitwise AND assignment for EnumClass/EnumClass, EnumClass/Integer and Integer/EnumClass
	template <class Ta, class Tb>
	inline constexpr auto operator&=(Ta& lhs, const Tb rhs)
		-> std::enable_if_t<(std::is_integral_v<Ta> && type::IsEnumClass<Tb>) ||
								(type::IsEnumClass<Ta> && std::is_integral_v<Tb>) ||
								(type::IsEnumClass<Ta> && type::IsEnumClass<Tb>),
							Ta>
	{
		return lhs = lhs & rhs;
	}

}  // namespace iplug
