#ifndef __AIRBITOP__H__
#define __AIRBITOP__H__
#include <Airkit/Precompiled.h>
namespace airkit
{
    // 固定位集合
    template <const size_t bits>
    struct Bitset
    {
        enum : size_t
        {
            BitsCnt = alignup<size_t>(bits, 8), // 位对齐
            ByteCnt = BitsCnt / 8,              // 字节数
        };

        Bitset() { memset(&mByte, 0, ByteCnt); }
        ~Bitset() {}

        void set(size_t index)
        {
            make_ensure(index < BitsCnt);
            const size_t byte = index / 8;
            const size_t bit = index % 8;
            mByte[byte] |= (1 << bit);
        }
        void unset(size_t index)
        {
            make_ensure(index < BitsCnt);
            const size_t byte = index / 8;
            const size_t bit = index % 8;
            mByte[byte] &= ~(1 << bit);
        }
        size_t get(size_t index)
        {
            make_ensure(index < BitsCnt);
            const size_t byte = index / 8;
            const size_t bit = index % 8;
            return mByte[byte] & (1 << bit);
        }

    protected:
        size_t8 mByte[ByteCnt];
    };

    // 位转换
    struct BitsCast32
    {
        union
        {
            int32_t mI32;
            uint32_t mU32;
            flt32_t mF32;
        };
        BitsCast32(int32_t value) : mI32(value) {}
        BitsCast32(uint32_t value) : mU32(value) {}
        BitsCast32(flt32_t value) : mF32(value) {}

        inline flt32_t toFlt32() { return mF32; }

        inline int32_t toInt32() { return mI32; }
        inline uint32_t toUint32() { return mU32; }

        inline flt32_t toFlt32(int32_t value)
        {
            mI32 = value;
            return mF32;
        }
        inline flt32_t toFlt32(uint32_t value)
        {
            mU32 = value;
            return mF32;
        }
        inline int32_t toInt32(flt32_t value)
        {
            mF32 = value;
            return mI32;
        }
        inline uint32_t toUint32(flt32_t value)
        {
            mF32 = value;
            return mU32;
        }
        inline bool sign() const { return (mU32 >> 31) != 0; }
    };
    struct BitsCast64
    {
        union
        {
            int64_t mI64;
            uint64_t mU64;
            flt64_t mF64;
        };

        BitsCast64(int64_t value) : mI64(value) {}
        BitsCast64(uint64_t value) : mU64(value) {}
        BitsCast64(flt64_t value) : mF64(value) {}

        inline flt64_t toFlt64() { return mF64; }
        inline int64_t toInt64() { return mI64; }
        inline uint64_t toUint64() { return mU64; }

        inline flt64_t toFlt64(int64_t value)
        {
            mI64 = value;
            return mF64;
        }
        inline flt64_t toFlt64(uint64_t value)
        {
            mU64 = value;
            return mF64;
        }
        inline int64_t toInt64(flt64_t value)
        {
            mF64 = value;
            return mI64;
        }
        inline uint64_t toUint64(flt64_t value)
        {
            mF64 = value;
            return mU64;
        }
        inline bool sign() const { return (mU64 >> 63) != 0; }
    };
}
#endif //!__AIRBITS__H__