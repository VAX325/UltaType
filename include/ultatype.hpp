/*
============================================
- File: ultatype.hpp
- Author: Vadim "VAX325"
- Creation date: 26.05.2023 / 15:10
- Last edit date: 27.05.2023 / 14:34
- Description: UltaType is wrapper class for
  storing almost any type of data.
- MIT Licensed. (See LICENSE for more info)
=============================================
*/

#pragma once
#ifndef ULTATYPE_H
#define ULTATYPE_H

#include <memory>
#include <typeinfo>
#include <string>
#include <string.h>
#include <cstddef>
#include <map>

#ifdef ULTATYPE_SAVE_TYPENAME
#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
static inline const std::string demangle(const char* const name)
{
	int status = -4;
	std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
	return (status == 0) ? res.get() : name;
}
#else
static inline const std::string demangle(const char* const name) { return name; }
#endif
#endif

namespace ULT
{
	namespace ULTConvert
	{
		struct FromString_t
		{
		private:
			const std::string& source;

		public:
			FromString_t(const std::string& src) : source(src) {}

			inline operator char() const { return (char)std::stoi(source); }
			inline operator short() const { return (short)std::stoi(source); }
			inline operator int() const { return std::stoi(source); }
			inline operator long() const { return std::stol(source); }
			inline operator long long() const { return std::stoll(source); }

			inline operator unsigned char() const { return (unsigned char)std::stoul(source); }
			inline operator unsigned short() const { return (unsigned short)std::stoul(source); }
			inline operator unsigned int() const { return (unsigned int)std::stoul(source); }
			inline operator unsigned long() const { return std::stoul(source); }
			inline operator unsigned long long() const { return std::stoull(source); }

			inline operator float() const { return std::stof(source); }
			inline operator double() const { return std::stod(source); }
			inline operator long double() const { return std::stold(source); }
		};

		template <typename S, typename T>
		inline void __utBaseConverter(const void* a, const void* dst)
		{
			const S& val = *(S*)a;
			*(T*)dst = static_cast<T>(val);
		}

#define __ULTATYPE_BASE_CONVERTER_DEFINE(s, t)                                                                         \
	{                                                                                                                  \
		{typeid(s).hash_code(), typeid(t).hash_code()}, ULT::ULTConvert::__utBaseConverter<s, t>                       \
	}
#define __ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(x)                                                                      \
	__ULTATYPE_BASE_CONVERTER_DEFINE(x, char), __ULTATYPE_BASE_CONVERTER_DEFINE(x, short),                             \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, int), __ULTATYPE_BASE_CONVERTER_DEFINE(x, long),                           \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, long long), __ULTATYPE_BASE_CONVERTER_DEFINE(x, unsigned char),            \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, unsigned short), __ULTATYPE_BASE_CONVERTER_DEFINE(x, unsigned int),        \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, unsigned long), __ULTATYPE_BASE_CONVERTER_DEFINE(x, unsigned long long),   \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, float), __ULTATYPE_BASE_CONVERTER_DEFINE(x, double),                       \
		__ULTATYPE_BASE_CONVERTER_DEFINE(x, long double)

		using Converter_t = void (*)(const void* src, const void* dst);
		std::map<std::tuple<size_t, size_t>, Converter_t> g_UTConverters = {
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(char),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(short),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(int),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(long),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(long long),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(unsigned char),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(unsigned short),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(unsigned int),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(unsigned long),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(unsigned long long),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(float),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(double),
			__ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH(long double),
		};

#define __ULTATYPE_CUSTOM_CONVERTER_DEFINE(c, s, t)                                                                    \
	{                                                                                                                  \
		{typeid(s).hash_code(), typeid(t).hash_code()}, c<s, t>                                                        \
	}

#define __ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(c, s, t)                                                               \
	{                                                                                                                  \
		{typeid(s).hash_code(), typeid(t).hash_code()}, c<s>                                                           \
	}

#define __ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(c, s, t)                                                               \
	{                                                                                                                  \
		{typeid(s).hash_code(), typeid(t).hash_code()}, c<t>                                                           \
	}

		template <typename S>
		inline void __ultToStringConverter(const void* a, const void* dst /*std::string*/)
		{
			const S val = *(S*)a;
			*(std::string*)dst = std::to_string(val);
		}

		template <typename T>
		inline void __ultFromStringConverter(const void* a /*std::string*/, const void* dst)
		{
			const std::string& val = *(std::string*)a;
			*(T*)dst = static_cast<T>(FromString_t(val));
		}

		const volatile char g_ConvertInit = ([]() -> char { 
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, char, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, short, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, int, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long long, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned char, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned short, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned int, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned long, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned long long, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, float, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, double, std::string));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long double , std::string));

			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, char));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, short));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, int));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long long));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned char));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned short));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned int));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned long));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned long long));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, float));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, double));
			g_UTConverters.insert(__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long double));
			return 1;
		})();
	} // namespace ULTConvert

	namespace ULTCompare
	{
		template <typename T1, typename T2>
		inline const char __ultBaseComparer(const void* a, const void* b)
		{
#pragma warning(push)
#pragma warning(disable : 4018)
			const T1& aT = *(T1*)a;
			const T2& bT = *(T2*)b;
			if (aT < bT)
				return -1;
			else if (aT == bT)
				return 0;
			else /*if (aT > bT)*/
				return 1;
#pragma warning(pop)
		}

#define __ULTATYPE_BASE_COMPARER_DEFINE(a, b)                                                                          \
	{                                                                                                                  \
		{typeid(a).hash_code(), typeid(b).hash_code()}, ULT::ULTCompare::__ultBaseComparer<a, b>                       \
	}
#define __ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(x)                                                                       \
	__ULTATYPE_BASE_COMPARER_DEFINE(x, char), __ULTATYPE_BASE_COMPARER_DEFINE(x, short),                               \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, int), __ULTATYPE_BASE_COMPARER_DEFINE(x, long),                             \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, long long), __ULTATYPE_BASE_COMPARER_DEFINE(x, unsigned char),              \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, unsigned short), __ULTATYPE_BASE_COMPARER_DEFINE(x, unsigned int),          \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, unsigned long), __ULTATYPE_BASE_COMPARER_DEFINE(x, unsigned long long),     \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, float), __ULTATYPE_BASE_COMPARER_DEFINE(x, double),                         \
		__ULTATYPE_BASE_COMPARER_DEFINE(x, long double)

		using Comparer_t = const char (*)(const void* a, const void* b);
		std::map<std::tuple<size_t, size_t>, Comparer_t> g_UTComparers = {
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(char),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(short),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(int),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(long),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(long long),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(unsigned char),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(unsigned short),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(unsigned int),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(unsigned long),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(unsigned long long),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(float),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(double),
			__ULTATYPE_BASE_COMPARER_DEFINE_BUNCH(long double),

			__ULTATYPE_BASE_COMPARER_DEFINE(std::string, std::string),
		};

		const volatile char g_CompareInit = ([]() -> char { return 1; })();
	} // namespace ULTCompare

	class UltaType final
	{
	private:
#ifndef ULTATYPE_BYTE_TYPE
#if __cplusplus >= 201703L
		using UltaTypeByte_t = std::byte;
#else
		using UltaTypeByte_t = unsigned char;
#endif
#else
		using UltaTypeByte_t = ULTATYPE_BYTE_TYPE;
#endif

#ifdef ULTATYPE_SAVE_TYPENAME
#if __cplusplus >= 201703L
		using UltaTypeName_t = std::string_view;
#else
		using UltaTypeName_t = std::string;
#endif
#endif

		static_assert(sizeof(UltaTypeByte_t) == 1,
					  "Can't recognize byte type! Please, define it manualy with ULTATYPE_BYTE_TYPE=<byteType> macro");

	public:
		inline UltaType() : m_ziTypeHash(0), m_ziTypeSize(0), m_pPtr(nullptr) {}
		template <typename T>
		inline UltaType(const T& value) : m_ziTypeHash(0), m_ziTypeSize(0), m_pPtr(nullptr)
		{
			SetValue<T>(value);
		}
		inline UltaType(const UltaType& other) : m_ziTypeHash(0), m_ziTypeSize(0), m_pPtr(nullptr)
		{
			m_ziTypeHash = other.m_ziTypeHash;
			m_ziTypeSize = other.m_ziTypeSize;
			m_pPtr = std::make_unique<UltaTypeByte_t[]>(m_ziTypeSize);
#ifdef ULTATYPE_SAVE_TYPENAME
			m_strTypeName = other.m_strTypeName;
#endif
			memcpy(m_pPtr.get(), other.m_pPtr.get(), m_ziTypeSize);
		}
		inline ~UltaType() { m_pPtr.release(); }

		template <typename T>
		void SetValue(const T& value)
		{
			const size_t newHash = typeid(value).hash_code();
			if (newHash != m_ziTypeHash)
			{
				m_ziTypeHash = typeid(value).hash_code();
#ifdef ULTATYPE_SAVE_TYPENAME
				m_strTypeName = demangle(typeid(value).name());
#endif
				m_ziTypeSize = sizeof(value);
				m_pPtr.release();
				m_pPtr.reset(new UltaTypeByte_t[m_ziTypeSize]);
				T& data = *(T*)m_pPtr.get();
				memcpy(&data, &value, sizeof(data));
			}
			else
			{
				T& data = *(T*)m_pPtr.get();
				memcpy(&data, &value, sizeof(data));
			}
		}

		inline UltaType& operator=(const UltaType& other)
		{
			m_ziTypeHash = other.m_ziTypeHash;
			m_ziTypeSize = other.m_ziTypeSize;
			m_pPtr = std::make_unique<UltaTypeByte_t[]>(m_ziTypeSize);
#ifdef ULTATYPE_SAVE_TYPENAME
			m_strTypeName = other.m_strTypeName;
#endif
			memcpy(m_pPtr.get(), other.m_pPtr.get(), m_ziTypeSize);
			return *this;
		}

		template <typename T>
		inline UltaType& operator=(const T& value)
		{
			SetValue<T>(value);
			return *this;
		}

		template <typename T>
		inline T* GetPointer() const noexcept
		{
			return reinterpret_cast<T*>(m_pPtr.get());
		}

		template <typename T>
		inline T& GetValueHard() const noexcept
		{
			T& ref = *reinterpret_cast<T*>(m_pPtr.get());
			return ref;
		}

		template <typename T>
		T& GetValue() const
		{
			using namespace ULTConvert;
			static thread_local T g_THLocalTmp;

			if (m_ziTypeHash == typeid(T).hash_code()) return GetValueHard<T>();

			const auto& conv = g_UTConverters.find(std::tuple<size_t, size_t>(m_ziTypeHash, typeid(T).hash_code()));
			if (conv != g_UTConverters.end())
			{
				T& ref = g_THLocalTmp;
				conv->second(m_pPtr.get(), (void*)&ref);
				return ref;
			}

			return GetValueHard<T>();
		}

		template <typename T>
		inline operator T&()
		{
			return GetValue<T>();
		}

		bool operator==(const UltaType& other) const
		{
			using namespace ULTCompare;
			const auto& comp = g_UTComparers.find({m_ziTypeHash, other.m_ziTypeHash});
			if (comp != g_UTComparers.end())
				return !comp->second(m_pPtr.get(), other.m_pPtr.get()); // ! 'cause 0 - equal
			return false;
		}

		template <typename T>
		inline bool operator==(const T& other) const
		{
			return GetValue<T>() == other;
		}

		bool operator<(const UltaType& other) const
		{
			using namespace ULTCompare;
			const auto& comp = g_UTComparers.find({m_ziTypeHash, other.m_ziTypeHash});
			if (comp != g_UTComparers.end()) return comp->second(m_pPtr.get(), other.m_pPtr.get()) == -1;
			return false;
		}

		template <typename T>
		inline bool operator<(const T& other) const
		{
			return GetValue<T>() < other;
		}

		bool operator<=(const UltaType& other) const
		{
			using namespace ULTCompare;
			const auto& comp = g_UTComparers.find({m_ziTypeHash, other.m_ziTypeHash});
			if (comp != g_UTComparers.end())
			{
				const auto res = comp->second(m_pPtr.get(), other.m_pPtr.get());
				return res == -1 || res == 0;
			}
			return false;
		}

		template <typename T>
		inline bool operator<=(const T& other) const
		{
			return GetValue<T>() <= other;
		}

		bool operator>(const UltaType& other) const
		{
			using namespace ULTCompare;
			const auto& comp = g_UTComparers.find({m_ziTypeHash, other.m_ziTypeHash});
			if (comp != g_UTComparers.end()) return comp->second(m_pPtr.get(), other.m_pPtr.get()) == 1;
			return false;
		}

		template <typename T>
		inline bool operator>(const T& other) const
		{
			return GetValue<T>() > other;
		}

		bool operator>=(const UltaType& other) const
		{
			using namespace ULTCompare;
			const auto& comp = g_UTComparers.find({m_ziTypeHash, other.m_ziTypeHash});
			if (comp != g_UTComparers.end())
			{
				const auto res = comp->second(m_pPtr.get(), other.m_pPtr.get());
				return res == 1 || res == 0;
			}
			return false;
		}

		template <typename T>
		inline bool operator>=(const T& other) const
		{
			return GetValue<T>() >= other;
		}

		inline const size_t GetSize() const noexcept { return m_ziTypeSize; }

		inline const size_t GetTypeHash() const noexcept { return m_ziTypeHash; }

#ifdef ULTATYPE_SAVE_TYPENAME

	public:
		UltaTypeName_t GetTypeName() const { return m_strTypeName.c_str(); }

	private:
		std::string m_strTypeName;
#endif

	private:
		size_t m_ziTypeHash;
		size_t m_ziTypeSize;
		std::unique_ptr<UltaTypeByte_t[]> m_pPtr;
	};
} // namespace ULT

#endif
