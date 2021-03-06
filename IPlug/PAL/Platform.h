/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for more info.

 ==============================================================================
*/


#pragma once

// Required std includes for platform.h
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "IPlugPreprocessor.h"
#include "PlatformCompiler.h"


#ifndef PLATFORM_NAME
	#error "PLATFORM_NAME must be defined. Make sure cmake declared this when generating project."
#endif

// Set non-active platforms to 0
#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_IOS
	#define PLATFORM_IOS 0
#endif

#ifndef PLATFORM_MAC
	#define PLATFORM_MAC 0
#endif

#ifndef PLATFORM_LINUX
	#define PLATFORM_LINUX 0
#endif

#ifndef PLATFORM_WEB
	#define PLATFORM_WEB 0
#endif

#if PLATFORM_WINDOWS + PLATFORM_IOS + PLATFORM_MAC + PLATFORM_LINUX + PLATFORM_WEB != 1
	#error "One and only one platform should be active. Check cmake settings."
#endif

namespace iplug::generic
{
	// Set default types
	// std::uint*_t and int*_t are optional implementations
	// and may not be available on some compilers.
	struct Types
	{
		using byte   = std::byte;
		using uint8  = unsigned char;
		using uint16 = unsigned short;
		using uint32 = unsigned int;
		using uint64 = unsigned long long;
		using int8   = char;
		using int16  = short;
		using int32  = int;
		using int64  = long long;
		using utf8   = unsigned char;
		using utf16  = char16_t;
		using utf32  = char32_t;
		using size_t = std::size_t;
	};
}  // namespace iplug::generic

// Include target platform main header file
#include PLATFORM_PREFIX_HEADER(Platform.h)

// Must be defined after main platform header
#ifndef NDEBUG
	#define DEBUG_ASSERT(expression) ((expression) ? ((void) 0) : (DEBUGBREAK()))
#else
	#define DEBUG_ASSERT(expression) ((void) 0)
#endif

// NULL redefinition for compiler conformance and overload type safety
#undef NULL
#define NULL nullptr

#ifndef BEGIN_INCLUDE_DEPENDENCIES
	#define BEGIN_INCLUDE_DEPENDENCIES
#endif

#ifndef END_INCLUDE_DEPENDENCIES
	#define END_INCLUDE_DEPENDENCIES
#endif

#ifndef PLATFORM_LITTLE_ENDIAN
	#define PLATFORM_LITTLE_ENDIAN 0
#endif

#ifndef PLATFORM_CACHE_LINE_SIZE
	#define PLATFORM_CACHE_LINE_SIZE 64
#endif

#if PLATFORM_CACHE_LINE_SIZE != 16 && PLATFORM_CACHE_LINE_SIZE != 32 && PLATFORM_CACHE_LINE_SIZE != 64 && \
	PLATFORM_CACHE_LINE_SIZE != 128
	#error "Invalid PLATFORM_CACHE_LINE_SIZE set. Must be 16, 32, 64 or 128"
#endif

#ifndef PLATFORM_PTHREADS
	#define PLATFORM_PTHREADS 0
#endif


//-----------------------------------------------------------------------------
// Link types from target platform to iplug namespace

namespace iplug
{
	// Check if we have a valid platform struct
	static_assert(std::is_class_v<type::Platform>, "Platform type definition is wrong type.");
	static_assert(std::is_base_of_v<generic::Types, type::Platform>,
				  "Platform type definition structure inheritance failure.");

	using byte   = type::Platform::byte;    //! 8-bit unsigned enum class type
	using uint8  = type::Platform::uint8;   //! 8-bit unsigned
	using uint16 = type::Platform::uint16;  //! 16-bit unsigned
	using uint32 = type::Platform::uint32;  //! 32-bit unsigned
	using uint64 = type::Platform::uint64;  //! 64-bit unsigned
	using int8   = type::Platform::int8;    //! 8-bit signed
	using int16  = type::Platform::int16;   //! 16-bit signed
	using int32  = type::Platform::int32;   //! 32-bit signed
	using int64  = type::Platform::int64;   //! 64-bit signed
	using utf8   = type::Platform::utf8;    //! 8-bit unsigned
	using utf16  = type::Platform::utf16;   //! 16-bit unsigned
	using utf32  = type::Platform::utf32;   //! 32-bit unsigned
	using size_t = type::Platform::size_t;  //! 32-bit or 64-bit unsigned
	using tfloat = PLUG_TFLOAT_TYPE;        //! defined floating-point type float or double


	//-----------------------------------------------------------------------------
	// Type safety checks. Don't want things to go badonkadonk.

	static_assert(sizeof(void*) == (PLATFORM_64BIT + 1) << 2,
				  "ptr size failed. size does not match target architecture (32bit/64bit).");
	static_assert(sizeof(void*) == sizeof(nullptr),
				  "ptr size failed. void* and nullptr should be equal size. If this fails, the world is doomed.");

	static_assert(std::is_floating_point_v<tfloat> && (std::is_same_v<tfloat, float> || std::is_same_v<tfloat, double>),
				  "tfloat is invalid type. Only float and double are valid");

	static_assert(false == 0, "bool false value failed. false is not 0");
	static_assert(true == 1, "bool true value failed. true is not 1");

	static_assert(sizeof(uint8) == 1, "uint8 type size failed.");
	static_assert(sizeof(uint16) == 2, "uint16 type size failed.");
	static_assert(sizeof(uint32) == 4, "uint32 type size failed.");
	static_assert(sizeof(uint64) == 8, "uint64 type size failed.");
	static_assert(sizeof(int8) == 1, "int8 type size failed.");
	static_assert(sizeof(int16) == 2, "int16 type size failed.");
	static_assert(sizeof(int32) == 4, "int32 type size failed.");
	static_assert(sizeof(int64) == 8, "int64 type size failed.");
	static_assert(sizeof(utf8) == 1, "utf8 type size failed.");
	static_assert(sizeof(utf16) == 2, "utf16 type size failed.");
	static_assert(sizeof(utf32) == 4, "utf32 type size failed.");
	static_assert(sizeof(size_t) == sizeof(nullptr), "size_t type size failed.");
	static_assert(sizeof(size_t) == (PLATFORM_64BIT + 1) << 2, "size_t type size failed.");
	static_assert(sizeof(wchar_t) >= 2, "wchar_t type size failed.");

	static_assert(uint8(-1) > uint8(0), "uint8 type sign test failed. uint8 is signed.");
	static_assert(uint16(-1) > uint16(0), "uint16 type sign test failed. uint16 is signed.");
	static_assert(uint32(-1) > uint32(0), "uint32 type sign test failed. uint32 is signed.");
	static_assert(uint64(-1) > uint64(0), "uint64 type sign test failed. uint64 is signed.");
	static_assert(int8(-1) < int8(0), "int8 type sign test failed. int8 is unsigned.");
	static_assert(int16(-1) < int16(0), "int16 type sign test failed. int16 is unsigned.");
	static_assert(int32(-1) < int32(0), "int32 type sign test failed. int32 is unsigned.");
	static_assert(int64(-1) < int64(0), "int64 type sign test failed. int64 is unsigned.");
	static_assert(utf8(-1) > utf8(0), "utf8 type sign test failed. utf8 is signed.");
	static_assert(utf16(-1) > utf16(0), "utf16 type sign test failed. utf16 is signed.");
	static_assert(utf32(-1) > utf32(0), "utf32 type sign test failed. utf32 is signed.");
	static_assert(size_t(-1) > size_t(0), "size_t type sign test failed. size is signed.");
	static_assert(wchar_t(-1) > wchar_t(0), "wchar_t type sign test failed. wchar_t is signed.");


	//-----------------------------------------------------------------------------
	// templates

	// Return number of elements in an array at compile time.
	// TODO: temporary, move to Array class later
	template <class T>
	constexpr auto ArrayCount(T&& array)
	{
		return sizeof(uint8(&)[sizeof(array) / sizeof(array[0])]);
	}

	//-----------------------------------------------------------------------------
	// Enum class helpers with compile-time information
	// Can be used to for replacing #ifdef/#endif with 'if constexpr(ENUM::Native == ENUM::Value){...}'

	enum class EBuildType
	{   // clang-format off
		Debug,
		Release,
		ReleaseWithDebugInformation,
		Distributed,
		Native =
			#if DISTRIBUTED
				Distributed
			#elif RELWITHDEBINFO
				ReleaseWithDebugInformation
			#elif RELEASE
				Release
			#elif _DEBUG
				Debug
			#endif
		// clang-format on
	};

	enum class ECompiler
	{   // clang-format off
		Emscripten,
		MSVC,
		Clang,
		AppleClang,
		GCC,
		Native  = (PLATFORM_COMPILER_EMSCRIPTEN) ? Emscripten
				: (PLATFORM_COMPILER_MSVC)       ? MSVC
				: (PLATFORM_COMPILER_GCC)        ? GCC
				: (PLATFORM_COMPILER_CLANG)      ? Clang : AppleClang
		// clang-format on
	};

	enum class EArch
	{
		_32bit,
		_64bit,
		Native = (PLATFORM_64BIT == 1) ? _64bit : _32bit
	};

	enum class EPlatform
	{   // clang-format off
		Windows = 0x10,
		MacOS   = 0x20,
		iOS     = 0x30,
		Linux   = 0x40,
		Web     = 0x50,
		Native  = (PLATFORM_WINDOWS) ? Windows
				: (PLATFORM_LINUX)   ? Linux
				: (PLATFORM_MAC)     ? MacOS
				: (PLATFORM_IOS)     ? iOS : Web
		// clang-format on
	};

	enum class EEndian
	{
		Little = 0,
		Big    = 1,
		Native = (PLATFORM_LITTLE_ENDIAN == 1) ? Little : Big
	};

	enum class ECacheLineSize
	{   // clang-format off
		_16,
		_32,
		_64,
		_128,
		Native = (PLATFORM_CACHE_LINE_SIZE == 16) ? _16
			   : (PLATFORM_CACHE_LINE_SIZE == 32) ? _32
			   : (PLATFORM_CACHE_LINE_SIZE == 64) ? _64 : _128
		// clang-format on
	};

	enum class EPlugApi
	{   // clang-format off
		APP,
		VST2,
		VST3,
		AAX,
		AUv2,
		AUv3,
		WAM,
		WEB,
		Native = 
			#if APP_API
				APP
			#elif VST2_API
				VST2
			#elif VST3_API
				VST3
			#elif AAX_API
				AAX
			#elif AUV2_API
				AUv2
			#elif AUV3_API
				AUv3
			#elif WAM_API
				WAM
			#elif WEB_API
				WEB
			#endif
		// clang-format on
	};

	enum class EPlugApiState
	{   // clang-format off
		None,
		Controller,
		Processor,
		Native = 
			#if VST3C_API || WEB_API
				Controller
			#elif VST3P_API || WAM_API
				Processor
			#else
				None
			#endif
		// clang-format on
	};

	// Result codes returned by various functions
	enum class EResult : int32
	{
		SUCCESS = 0x00000000,  // Success

		WARNING = (1 << 30) | 0x00000001,  // Warnings

		FAILED = (1 << 31) | 0x00000001,  // Errors
	};


}  // namespace iplug
