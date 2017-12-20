/*
 * Copyright 2010-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#ifndef BX_CPU_H_HEADER_GUARD
#	error "Must be included from bx/cpu.h!"
#endif // BX_CPU_H_HEADER_GUARD

#if BX_COMPILER_MSVC
#	include <emmintrin.h> // _mm_fence
#   include <intrin.h>

extern "C" void _ReadBarrier();
#	pragma intrinsic(_ReadBarrier)

extern "C" void _WriteBarrier();
#	pragma intrinsic(_WriteBarrier)

extern "C" void _ReadWriteBarrier();
#	pragma intrinsic(_ReadWriteBarrier)

extern "C" long _InterlockedExchangeAdd(long volatile* _ptr, long _value);
#	pragma intrinsic(_InterlockedExchangeAdd)

extern "C" int64_t __cdecl _InterlockedExchangeAdd64(int64_t volatile* _ptr, int64_t _value);
//#	pragma intrinsic(_InterlockedExchangeAdd64)

extern "C" long _InterlockedCompareExchange(long volatile* _ptr, long _exchange, long _comparand);
#	pragma intrinsic(_InterlockedCompareExchange)

extern "C" int64_t _InterlockedCompareExchange64(int64_t volatile* _ptr, int64_t _exchange, int64_t _comparand);
#	pragma intrinsic(_InterlockedCompareExchange64)

extern "C" void* _InterlockedExchangePointer(void* volatile* _ptr, void* _value);
#	pragma intrinsic(_InterlockedExchangePointer)

//extern "C" long _InterlockedIncrement(long * lpAddend);
#   pragma intrinsic(_InterlockedIncrement)

//extern "C" __int64 _InterlockedIncrement64(__int64 * lpAddend);
#   pragma intrinsic(_InterlockedIncrement64)

//extern "C" long _InterlockedDecrement(long * lpAddend);
#   pragma intrinsic(_InterlockedDecrement)

//extern "C" __int64 _InterlockedDecrement64(__int64 * lpAddend);
#   pragma intrinsic(_InterlockedDecrement64)

//extern "C" long _InterlockedExchange(long volatile * Target, long Value);
#   pragma intrinsic(_InterlockedExchange)

//extern "C" __int64 _InterlockedExchange64(__int64 volatile * Target, __int64 Value);
#   pragma intrinsic(_InterlockedExchange64)

extern "C" long _InterlockedXor(long volatile* _value, long mask);
#   pragma intrinsic(_InterlockedXor)

extern "C" long _InterlockedAdd(long volatile* _value, long mask);
#   pragma intrinsic(_InterlockedAnd)

extern "C" long _InterlockedOr(long volatile* _value, long mask);
#   pragma intrinsic(_InterlockedOr)

#	if BX_PLATFORM_WINRT
#		define _InterlockedExchangeAdd64 InterlockedExchangeAdd64
#	endif // BX_PLATFORM_WINRT
#endif // BX_COMPILER_MSVC

namespace bx
{
	inline void readBarrier()
	{
#if BX_COMPILER_MSVC
		_ReadBarrier();
#else
		asm volatile("":::"memory");
#endif // BX_COMPILER
	}

	inline void writeBarrier()
	{
#if BX_COMPILER_MSVC
		_WriteBarrier();
#else
		asm volatile("":::"memory");
#endif // BX_COMPILER
	}

	inline void readWriteBarrier()
	{
#if BX_COMPILER_MSVC
		_ReadWriteBarrier();
#else
		asm volatile("":::"memory");
#endif // BX_COMPILER
	}

	inline void memoryBarrier()
	{
#if BX_PLATFORM_WINRT
		MemoryBarrier();
#elif BX_COMPILER_MSVC
		_mm_mfence();
#else
		__sync_synchronize();
#endif // BX_COMPILER
	}

	template<>
	inline int32_t atomicCompareAndSwap<int32_t>(volatile int32_t* _ptr, int32_t _old, int32_t _new)
	{
#if BX_COMPILER_MSVC
		return int32_t(_InterlockedCompareExchange( (volatile long*)(_ptr), long(_new), long(_old) ) );
#else
		return __sync_val_compare_and_swap( (volatile int32_t*)_ptr, _old, _new);
#endif // BX_COMPILER
	}

	template<>
	inline uint32_t atomicCompareAndSwap<uint32_t>(volatile uint32_t* _ptr, uint32_t _old, uint32_t _new)
	{
#if BX_COMPILER_MSVC
		return uint32_t(_InterlockedCompareExchange( (volatile long*)(_ptr), long(_new), long(_old) ) );
#else
		return __sync_val_compare_and_swap( (volatile int32_t*)_ptr, _old, _new);
#endif // BX_COMPILER
	}

	template<>
	inline int64_t atomicCompareAndSwap<int64_t>(volatile int64_t* _ptr, int64_t _old, int64_t _new)
	{
#if BX_COMPILER_MSVC
		return _InterlockedCompareExchange64(_ptr, _new, _old);
#else
		return __sync_val_compare_and_swap( (volatile int64_t*)_ptr, _old, _new);
#endif // BX_COMPILER
	}

	template<>
	inline uint64_t atomicCompareAndSwap<uint64_t>(volatile uint64_t* _ptr, uint64_t _old, uint64_t _new)
	{
#if BX_COMPILER_MSVC
		return uint64_t(_InterlockedCompareExchange64( (volatile int64_t*)(_ptr), int64_t(_new), int64_t(_old) ) );
#else
		return __sync_val_compare_and_swap( (volatile int64_t*)_ptr, _old, _new);
#endif // BX_COMPILER
	}

	template<>
	inline int32_t atomicFetchAndAdd<int32_t>(volatile int32_t* _ptr, int32_t _add)
	{
#if BX_COMPILER_MSVC
		return _InterlockedExchangeAdd( (volatile long*)_ptr, _add);
#else
		return __sync_fetch_and_add(_ptr, _add);
#endif // BX_COMPILER_
	}

	template<>
	inline uint32_t atomicFetchAndAdd<uint32_t>(volatile uint32_t* _ptr, uint32_t _add)
	{
		return uint32_t(atomicFetchAndAdd<int32_t>( (volatile int32_t*)_ptr, int32_t(_add) ) );
	}

	template<>
	inline int64_t atomicFetchAndAdd<int64_t>(volatile int64_t* _ptr, int64_t _add)
	{
#if BX_COMPILER_MSVC
#	if _WIN32_WINNT >= 0x600
		return _InterlockedExchangeAdd64( (volatile int64_t*)_ptr, _add);
#	else
		int64_t oldVal;
		int64_t newVal = *(int64_t volatile*)_ptr;
		do
		{
			oldVal = newVal;
			newVal = atomicCompareAndSwap<int64_t>(_ptr, oldVal, newVal + _add);

		} while (oldVal != newVal);

		return oldVal;
#	endif
#else
		return __sync_fetch_and_add(_ptr, _add);
#endif // BX_COMPILER_
	}

	template<>
	inline uint64_t atomicFetchAndAdd<uint64_t>(volatile uint64_t* _ptr, uint64_t _add)
	{
		return uint64_t(atomicFetchAndAdd<int64_t>( (volatile int64_t*)_ptr, int64_t(_add) ) );
	}

	template<>
	inline int32_t atomicAddAndFetch<int32_t>(volatile int32_t* _ptr, int32_t _add)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, _add) + _add;
#else
		return __sync_add_and_fetch(_ptr, _add);
#endif // BX_COMPILER_
	}

	template<>
	inline int64_t atomicAddAndFetch<int64_t>(volatile int64_t* _ptr, int64_t _add)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, _add) + _add;
#else
		return __sync_add_and_fetch(_ptr, _add);
#endif // BX_COMPILER_
	}

	template<>
	inline uint32_t atomicAddAndFetch<uint32_t>(volatile uint32_t* _ptr, uint32_t _add)
	{
		return uint32_t(atomicAddAndFetch<int32_t>( (volatile int32_t*)_ptr, int32_t(_add) ) );
	}

	template<>
	inline uint64_t atomicAddAndFetch<uint64_t>(volatile uint64_t* _ptr, uint64_t _add)
	{
		return uint64_t(atomicAddAndFetch<int64_t>( (volatile int64_t*)_ptr, int64_t(_add) ) );
	}

	template<>
	inline int32_t atomicFetchAndSub<int32_t>(volatile int32_t* _ptr, int32_t _sub)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, -_sub);
#else
		return __sync_fetch_and_sub(_ptr, _sub);
#endif // BX_COMPILER_
	}

	template<>
	inline int64_t atomicFetchAndSub<int64_t>(volatile int64_t* _ptr, int64_t _sub)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, -_sub);
#else
		return __sync_fetch_and_sub(_ptr, _sub);
#endif // BX_COMPILER_
	}

	template<>
	inline uint32_t atomicFetchAndSub<uint32_t>(volatile uint32_t* _ptr, uint32_t _add)
	{
		return uint32_t(atomicFetchAndSub<int32_t>( (volatile int32_t*)_ptr, int32_t(_add) ) );
	}

	template<>
	inline uint64_t atomicFetchAndSub<uint64_t>(volatile uint64_t* _ptr, uint64_t _add)
	{
		return uint64_t(atomicFetchAndSub<int64_t>( (volatile int64_t*)_ptr, int64_t(_add) ) );
	}

	template<>
	inline int32_t atomicSubAndFetch<int32_t>(volatile int32_t* _ptr, int32_t _sub)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, -_sub) - _sub;
#else
		return __sync_sub_and_fetch(_ptr, _sub);
#endif // BX_COMPILER_
	}

	template<>
	inline int64_t atomicSubAndFetch<int64_t>(volatile int64_t* _ptr, int64_t _sub)
	{
#if BX_COMPILER_MSVC
		return atomicFetchAndAdd(_ptr, -_sub) - _sub;
#else
		return __sync_sub_and_fetch(_ptr, _sub);
#endif // BX_COMPILER_
	}

	template<>
	inline uint32_t atomicSubAndFetch<uint32_t>(volatile uint32_t* _ptr, uint32_t _add)
	{
		return uint32_t(atomicSubAndFetch<int32_t>( (volatile int32_t*)_ptr, int32_t(_add) ) );
	}

	template<>
	inline uint64_t atomicSubAndFetch<uint64_t>(volatile uint64_t* _ptr, uint64_t _add)
	{
		return uint64_t(atomicSubAndFetch<int64_t>( (volatile int64_t*)_ptr, int64_t(_add) ) );
	}

	template<typename Ty>
	inline Ty atomicFetchTestAndAdd(volatile Ty* _ptr, Ty _test, Ty _value)
	{
		Ty oldVal;
		Ty newVal = *_ptr;
		do
		{
			oldVal = newVal;
			newVal = atomicCompareAndSwap<Ty>(_ptr, oldVal, newVal >= _test ? _test : newVal+_value);

		} while (oldVal != newVal);

		return oldVal;
	}

	template<typename Ty>
	inline Ty atomicFetchTestAndSub(volatile Ty* _ptr, Ty _test, Ty _value)
	{
		Ty oldVal;
		Ty newVal = *_ptr;
		do
		{
			oldVal = newVal;
			newVal = atomicCompareAndSwap<Ty>(_ptr, oldVal, newVal <= _test ? _test : newVal-_value);

		} while (oldVal != newVal);

		return oldVal;
	}

	template<typename Ty>
	Ty atomicFetchAndAddsat(volatile Ty* _ptr, Ty _value, Ty _max)
	{
		Ty oldVal;
		Ty newVal = *_ptr;
		do
		{
			oldVal = newVal;
			newVal = atomicCompareAndSwap<Ty>(_ptr, oldVal, newVal >= _max ? _max : min(_max, newVal+_value) );

		} while (oldVal != newVal && oldVal != _max);

		return oldVal;
	}

	template<typename Ty>
	Ty atomicFetchAndSubsat(volatile Ty* _ptr, Ty _value, Ty _min)
	{
		Ty oldVal;
		Ty newVal = *_ptr;
		do
		{
			oldVal = newVal;
			newVal = atomicCompareAndSwap<Ty>(_ptr, oldVal, newVal <= _min ? _min : max(_min, newVal-_value) );

		} while (oldVal != newVal && oldVal != _min);

		return oldVal;
	}

	inline void* atomicExchangePtr(void** _ptr, void* _new)
	{
#if BX_COMPILER_MSVC
		return _InterlockedExchangePointer(_ptr, _new);
#else
		return __sync_lock_test_and_set(_ptr, _new);
#endif // BX_COMPILER
	}

    template<>
    inline int64_t atomicInc<int64_t>(volatile int64_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return _InterlockedIncrement64(_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline int32_t atomicInc<int32_t>(volatile int32_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return (long)_InterlockedIncrement((volatile long*)_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline int16_t atomicInc<int16_t>(volatile int16_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return _InterlockedIncrement16(_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline uint64_t atomicInc<uint64_t>(volatile uint64_t* _ptr)
    {
        return uint64_t(atomicInc<int64_t>((volatile int64_t*)_ptr));
    }

    template<>
    inline uint32_t atomicInc<uint32_t>(volatile uint32_t* _ptr)
    {
        return uint32_t(atomicInc<int32_t>((volatile int32_t*)_ptr));
    }

    template<>
    inline uint16_t atomicInc<uint16_t>(volatile uint16_t* _ptr)
    {
        return uint16_t(atomicInc<int16_t>((volatile int16_t*)_ptr));
    }

    /// Returns the resulting incremented value.
    template<typename Ty>
    inline Ty atomicInc(volatile Ty* _ptr)
    {
        return atomicInc(_ptr);
    }

    template<>
    inline int64_t atomicDec<int64_t>(volatile int64_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return _InterlockedDecrement64(_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline int32_t atomicDec<int32_t>(volatile int32_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return (long)_InterlockedDecrement((volatile long*)_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline int16_t atomicDec<int16_t>(volatile int16_t* _ptr)
    {
#if BX_COMPILER_MSVC
        return _InterlockedDecrement16(_ptr);
#else
        return __sync_add_and_fetch(_ptr, 1);
#endif
    }

    template<>
    inline uint64_t atomicDec<uint64_t>(volatile uint64_t* _ptr)
    {
        return uint64_t(atomicDec<int64_t>((volatile int64_t*)_ptr));
    }

    template<>
    inline uint32_t atomicDec<uint32_t>(volatile uint32_t* _ptr)
    {
        return uint32_t(atomicDec<int32_t>((volatile int32_t*)_ptr));
    }

    template<>
    inline uint16_t atomicDec<uint16_t>(volatile uint16_t* _ptr)
    {
        return uint16_t(atomicDec<int16_t>((volatile int16_t*)_ptr));
    }

    /// Returns the resulting decremented value.
    template<typename Ty>
    inline Ty atomicDec(volatile Ty* _ptr)
    {
        return atomicDec(_ptr);
    }

    template<>
    inline int64_t atomicOR<int64_t>(volatile int64_t* _ptr, int64_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedOr64(_ptr, _value);
#else
        return __sync_fetch_and_or(_ptr, _value);
#endif
    }

    template<>
    inline int32_t atomicOR<int32_t>(volatile int32_t* _ptr, int32_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedOr((volatile long*)_ptr, _value);
#else
        return __sync_fetch_and_or(_ptr, _value);
#endif
    }

    template<>
    inline uint64_t atomicOR<uint64_t>(volatile uint64_t* _ptr, uint64_t _value)
    {
        return uint64_t(atomicOR<int64_t>((volatile int64_t*)_ptr, int64_t(_value)));
    }

    template<>
    inline uint32_t atomicOR<uint32_t>(volatile uint32_t* _ptr, uint32_t _value)
    {
        return uint32_t(atomicOR<int32_t>((volatile int32_t*)_ptr, int32_t(_value)));
    }

    template<typename Ty>
    inline Ty atomicOR(volatile Ty* _ptr, Ty _value)
    {
        return atomicOR(_ptr, _value);
    }

    template<>
    inline int64_t atomicAND<int64_t>(volatile int64_t* _ptr, int64_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedAnd64(_ptr, _value);
#else
        return __sync_fetch_and_and(_ptr, _value);
#endif
    }

    template<>
    inline int32_t atomicAND<int32_t>(volatile int32_t* _ptr, int32_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedAnd((volatile long*)_ptr, _value);
#else
        return __sync_fetch_and_and(_ptr, _value);
#endif
    }

    template<>
    inline uint64_t atomicAND<uint64_t>(volatile uint64_t* _ptr, uint64_t _value)
    {
        return uint64_t(atomicAND<int64_t>((volatile int64_t*)_ptr, int64_t(_value)));
    }

    template<>
    inline uint32_t atomicAND<uint32_t>(volatile uint32_t* _ptr, uint32_t _value)
    {
        return uint32_t(atomicAND<int32_t>((volatile int32_t*)_ptr, int32_t(_value)));
    }

    template<typename Ty>
    inline Ty atomicAND(volatile Ty* _ptr, Ty _value)
    {
        return atomicAND(_ptr, _value);
    }

    template<>
    inline int64_t atomicXOR<int64_t>(volatile int64_t* _ptr, int64_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedXor64(_ptr, _value);
#else
        return __sync_fetch_and_and(_ptr, _value);
#endif
    }

    template<>
    inline int32_t atomicXOR<int32_t>(volatile int32_t* _ptr, int32_t _value)
    {
#if BX_COMPILER_MSVC
        return _InterlockedXor((volatile long*)_ptr, _value);
#else
        return __sync_fetch_and_and(_ptr, _value);
#endif
    }

    template<>
    inline uint64_t atomicXOR<uint64_t>(volatile uint64_t* _ptr, uint64_t _value)
    {
        return uint64_t(atomicXOR<int64_t>((volatile int64_t*)_ptr, int64_t(_value)));
    }

    template<>
    inline uint32_t atomicXOR<uint32_t>(volatile uint32_t* _ptr, uint32_t _value)
    {
        return uint32_t(atomicXOR<int32_t>((volatile int32_t*)_ptr, int32_t(_value)));
    }

    template<typename Ty>
    inline Ty atomicXOR(volatile Ty* _ptr, Ty _value)
    {
        return atomicXOR(_ptr, _value);
    }

    template<>
    inline int64_t atomicExchange<int64_t>(volatile int64_t* _ptr, int64_t _new)
    {
#if BX_COMPILER_MSVC
        return _InterlockedExchange64(_ptr, _new);
#else
        return __sync_lock_test_and_set(_ptr, _new);
#endif
    }

    template<>
    inline int32_t atomicExchange<int32_t>(volatile int32_t* _ptr, int32_t _new)
    {
#if BX_COMPILER_MSVC
        return _InterlockedExchange((volatile long*)_ptr, _new);
#else
        return __sync_lock_test_and_set(_ptr, _new);
#endif
    }

    template<>
    inline int16_t atomicExchange<int16_t>(volatile int16_t* _ptr, int16_t _new)
    {
#if BX_COMPILER_MSVC
        return _InterlockedExchange16(_ptr, _new);
#else
        return __sync_lock_test_and_set(_ptr, _new);
#endif
    }

    template<>
    inline uint64_t atomicExchange<uint64_t>(volatile uint64_t* _ptr, uint64_t _new)
    {
        return uint64_t(atomicExchange<int64_t>((volatile int64_t*)_ptr, int64_t(_new)));
    }

    template<>
    inline uint32_t atomicExchange<uint32_t>(volatile uint32_t* _ptr, uint32_t _new)
    {
        return uint32_t(atomicExchange<int32_t>((volatile int32_t*)_ptr, int32_t(_new)));
    }

    template<>
    inline uint16_t atomicExchange<uint16_t>(volatile uint16_t* _ptr, uint16_t _new)
    {
        return uint16_t(atomicExchange<int16_t>((volatile int16_t*)_ptr, int16_t(_new)));
    }

    template<typename Ty>
    inline Ty atomicExchange(volatile Ty* _ptr, Ty _new)
    {
        return atomicExchange(_ptr, _new);
    }

} // namespace bx