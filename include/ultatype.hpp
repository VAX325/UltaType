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
#if __cplusplus >= 201703L
#include <string_view>
#endif
#include <string.h>
#include <cstddef>
#include <map>

namespace ULT
{
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
#if __cplusplus >= 201703L
	static inline constexpr std::string_view demangle(const char* const name) { return name; }
#else
	static inline const std::string demangle(const char* const name) { return name; }
#endif
#endif
#endif

	namespace ULTReflection
	{
		enum class BaseTypes_t : int
		{
			TYPE_CHAR,
			TYPE_SHORT,
			TYPE_INT,
			TYPE_LONG,
			TYPE_LONGLONG,

			TYPE_UCHAR,
			TYPE_USHORT,
			TYPE_UINT,
			TYPE_ULONG,
			TYPE_ULONGLONG,

			TYPE_FLOAT,
			TYPE_DOUBLE,
			TYPE_LONGDOUBLE,

			TYPE_STDSTRING,
		};

#define __ULTATYPE_TYPE_ID_DEFINE(x, y)                                                                                \
	{                                                                                                                  \
		typeid(x).hash_code(), (int)y                                                                                  \
	}

		std::map<size_t, int> g_TypesIdentify = {
			__ULTATYPE_TYPE_ID_DEFINE(char, BaseTypes_t::TYPE_CHAR),
			__ULTATYPE_TYPE_ID_DEFINE(short, BaseTypes_t::TYPE_SHORT),
			__ULTATYPE_TYPE_ID_DEFINE(int, BaseTypes_t::TYPE_INT),
			__ULTATYPE_TYPE_ID_DEFINE(long, BaseTypes_t::TYPE_LONG),
			__ULTATYPE_TYPE_ID_DEFINE(long long, BaseTypes_t::TYPE_LONGLONG),

			__ULTATYPE_TYPE_ID_DEFINE(unsigned char, BaseTypes_t::TYPE_UCHAR),
			__ULTATYPE_TYPE_ID_DEFINE(unsigned short, BaseTypes_t::TYPE_USHORT),
			__ULTATYPE_TYPE_ID_DEFINE(unsigned int, BaseTypes_t::TYPE_UINT),
			__ULTATYPE_TYPE_ID_DEFINE(unsigned long, BaseTypes_t::TYPE_ULONG),
			__ULTATYPE_TYPE_ID_DEFINE(unsigned long long, BaseTypes_t::TYPE_ULONGLONG),

			__ULTATYPE_TYPE_ID_DEFINE(float, BaseTypes_t::TYPE_FLOAT),
			__ULTATYPE_TYPE_ID_DEFINE(double, BaseTypes_t::TYPE_DOUBLE),
			__ULTATYPE_TYPE_ID_DEFINE(long double, BaseTypes_t::TYPE_LONGDOUBLE),

			__ULTATYPE_TYPE_ID_DEFINE(std::string, BaseTypes_t::TYPE_STDSTRING),
		};

#undef __ULTATYPE_TYPE_ID_DEFINE
	} // namespace ULTReflection

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

			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, char, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, short, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, int, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long long, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned char, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned short, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned int, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned long, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, unsigned long long, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, float, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, double, std::string),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T(__ultToStringConverter, long double, std::string),

			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, char),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, short),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, int),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long long),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned char),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned short),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned int),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned long),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, unsigned long long),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, float),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, double),
			__ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S(__ultFromStringConverter, std::string, long double),
		};

#undef __ULTATYPE_BASE_CONVERTER_DEFINE
#undef __ULTATYPE_BASE_CONVERTER_DEFINE_BUNCH
#undef __ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_T
#undef __ULTATYPE_CUSTOM_CONVERTER_DEFINE_NO_S
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

#undef __ULTATYPE_BASE_COMPARER_DEFINE
#undef __ULTATYPE_BASE_COMPARER_DEFINE_BUNCH
	} // namespace ULTCompare

	namespace ULTOperations
	{
		struct AbstractOperator_t
		{
			virtual void Plus(const void* a, const void* b, void* result) = 0;
			virtual void Minus(const void* a, const void* b, void* result) = 0;
			virtual void Multiply(const void* a, const void* b, void* result) = 0;
			virtual void Divide(const void* a, const void* b, void* result) = 0;
		};

#pragma warning(push)
#pragma warning(disable : 4244)
		template <typename A, typename B>
		struct __ULTBaseTOperator_t : public AbstractOperator_t
		{
			virtual void Plus(const void* /*A*/ a, const void* /*B*/ b, void* /*A*/ result)
			{
				const A& aA = *(A*)a;
				const B& bB = *(B*)b;
				*(A*)result = aA + bB;
			}

			virtual void Minus(const void* /*A*/ a, const void* /*B*/ b, void* /*A*/ result)
			{
				const A& aA = *(A*)a;
				const B& bB = *(B*)b;
				*(A*)result = aA - bB;
			}

			virtual void Multiply(const void* /*A*/ a, const void* /*B*/ b, void* /*A*/ result)
			{
				const A& aA = *(A*)a;
				const B& bB = *(B*)b;
				*(A*)result = aA * bB;
			}

			virtual void Divide(const void* /*A*/ a, const void* /*B*/ b, void* /*A*/ result)
			{
				const A& aA = *(A*)a;
				const B& bB = *(B*)b;
				*(A*)result = aA / bB;
			}
		};
#pragma warning(pop)

#define __ULTATYPE_BASE_OPERATOR_DEFINE(x, y)                                                                          \
	{                                                                                                                  \
		{typeid(x).hash_code(), typeid(y).hash_code()}, new __ULTBaseTOperator_t<x, y>()                               \
	}

#define __ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(x)                                                                       \
	__ULTATYPE_BASE_OPERATOR_DEFINE(x, char), __ULTATYPE_BASE_OPERATOR_DEFINE(x, short),                               \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, int), __ULTATYPE_BASE_OPERATOR_DEFINE(x, long),                             \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, long long), __ULTATYPE_BASE_OPERATOR_DEFINE(x, unsigned char),              \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, unsigned short), __ULTATYPE_BASE_OPERATOR_DEFINE(x, unsigned int),          \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, unsigned long), __ULTATYPE_BASE_OPERATOR_DEFINE(x, unsigned long long),     \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, float), __ULTATYPE_BASE_OPERATOR_DEFINE(x, double),                         \
		__ULTATYPE_BASE_OPERATOR_DEFINE(x, long double)

		std::map<std::tuple<size_t, size_t>, AbstractOperator_t*> g_Operators = {
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(char),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(short),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(int),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(long),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(long long),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(unsigned char),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(unsigned short),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(unsigned int),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(unsigned long),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(unsigned long long),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(float),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(double),
			__ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH(long double),
		};

#undef __ULTATYPE_BASE_OPERATOR_DEFINE_BUNCH
#undef __ULTATYPE_BASE_OPERATOR_DEFINE
	} // namespace ULTOperations

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

		inline void Copy(const UltaType& other)
		{
			m_ziTypeHash = other.m_ziTypeHash;
			m_ziTypeSize = other.m_ziTypeSize;
			m_pPtr = std::make_unique<UltaTypeByte_t[]>(m_ziTypeSize);
#ifdef ULTATYPE_SAVE_TYPENAME
			m_strTypeName = other.m_strTypeName;
#endif
			memcpy(m_pPtr.get(), other.m_pPtr.get(), m_ziTypeSize);
		}

		inline UltaType(const UltaType& other) : m_ziTypeHash(0), m_ziTypeSize(0), m_pPtr(nullptr) { Copy(other); }
		inline UltaType& operator=(const UltaType& other)
		{
			Copy(other);
			return *this;
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
				memcpy(&data, &value, m_ziTypeSize);
			}
			else
			{
				T& data = *(T*)m_pPtr.get();
				memcpy(&data, &value, m_ziTypeSize);
			}
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
		inline operator T&() const
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

		UltaType operator+(const UltaType& other) const throw()
		{
			using namespace ULTOperations;
			UltaType res = *this;
			{
				const auto& it = g_Operators.find({m_ziTypeHash, other.m_ziTypeHash});
				if (it == g_Operators.end()) return res;
				it->second->Plus(res.m_pPtr.get(), other.m_pPtr.get(), res.m_pPtr.get());
			}
			return res;
		}

		UltaType operator+=(const UltaType& other) throw()
		{
			*this = *this + other;
			return *this;
		}

		UltaType operator-(const UltaType& other) const throw()
		{
			using namespace ULTOperations;
			UltaType res = *this;
			{
				const auto& it = g_Operators.find({m_ziTypeHash, other.m_ziTypeHash});
				if (it == g_Operators.end()) return res;
				it->second->Minus(res.m_pPtr.get(), other.m_pPtr.get(), res.m_pPtr.get());
			}
			return res;
		}

		UltaType operator-=(const UltaType& other) throw()
		{
			*this = *this - other;
			return *this;
		}

		UltaType operator*(const UltaType& other) const throw()
		{
			using namespace ULTOperations;
			UltaType res = *this;
			{
				const auto& it = g_Operators.find({m_ziTypeHash, other.m_ziTypeHash});
				if (it == g_Operators.end()) return res;
				it->second->Multiply(res.m_pPtr.get(), other.m_pPtr.get(), res.m_pPtr.get());
			}
			return res;
		}

		UltaType operator*=(const UltaType& other) throw()
		{
			*this = *this * other;
			return *this;
		}

		UltaType operator/(const UltaType& other) const throw()
		{
			using namespace ULTOperations;
			UltaType res = *this;
			{
				const auto& it = g_Operators.find({m_ziTypeHash, other.m_ziTypeHash});
				if (it == g_Operators.end()) return res;
				it->second->Divide(res.m_pPtr.get(), other.m_pPtr.get(), res.m_pPtr.get());
			}
			return res;
		}

		UltaType operator/=(const UltaType& other) throw()
		{
			*this = *this * other;
			return *this;
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
