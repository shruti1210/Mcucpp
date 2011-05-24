#pragma once
#include <core_cm3.h>

namespace Atomic
{
	class DisableInterrupts
	{
	public:
		DisableInterrupts()
		{
			_sreg = __get_PRIMASK();
			__disable_irq();
		}
		~DisableInterrupts()
		{
			__set_PRIMASK(_sreg);
		}
		operator bool()
		{return false;}
	private:
		uint32_t _sreg;
	};

#define ATOMIC if(Atomic::DisableInterrupts di = Atomic::DisableInterrupts()){}else

	namespace Private
	{
		inline uint8_t LDREX(uint8_t *addr)
		{
			return __LDREXB(addr);
		}

		inline uint16_t LDREX(uint16_t *addr)
		{
			return __LDREXH(addr);
		}

		inline uint32_t LDREX(uint32_t *addr)
		{
			return __LDREXW(addr);
		}

		inline uint32_t STREX(uint8_t value, uint8_t *addr)
		{
			return __STREXB(value, addr);
		}

		inline uint32_t STREX(uint16_t value, uint16_t *addr)
		{
			return __STREXH(value, addr);
		}

		inline uint32_t STREX(uint32_t value, uint32_t *addr)
		{
			return __STREXW(value, addr);
		}
	}
	
// TODO: reimplement it with LDREX/STREX
#define DECLARE_OP(OPERATION, OP_NAME) \
	template<class T, class T2>\
	T FetchAnd ## OP_NAME (volatile T * ptr, T2 value)   \
	{                                                    \
		T oldValue, newValue;                            \
		do                                               \
		{                                                \
			oldValue = Private::LDREX((T*)ptr);          \
			newValue = oldValue OPERATION value;         \
		}while(Private::STREX(newValue, (T*)ptr));       \
		return oldValue;                                 \
	}                                                    \
	template<class T, class T2>                          \
	T OP_NAME ## AndFetch(volatile T * ptr, T2 value)    \
	{                                                    \
		T oldValue, newValue;                            \
		do                                               \
		{                                                \
			oldValue = Private::LDREX((T*)ptr);          \
			newValue = oldValue OPERATION value;         \
		}while(Private::STREX(newValue, (T*)ptr));       \
		return newValue;                                 \
	}

	DECLARE_OP(+, Add)
	DECLARE_OP(-, Sub)
	DECLARE_OP(|, Or)
	DECLARE_OP(&, And)
	DECLARE_OP(^, Xor)
	
	template<class T, class T2>
	bool CompareExchange(T * ptr, T2 oldValue, T2 newValue)
	{
		if(Private::LDREX(ptr) == oldValue)
		return Private::STREX(newValue, ptr) == 0;
		__CLREX();
		return false;
	}

}