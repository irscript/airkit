#ifndef __AIRATOIMC__H__
#define __AIRATOIMC__H__
#include <Airkit/Precompiled.h>
#include <atomic>
namespace airkit
{
    // 原子操作类型
    using aint8_t = std::atomic_int8_t;
    using aint16_t = std::atomic_int16_t;
    using aint32_t = std::atomic_int32_t;
    using aint64_t = std::atomic_int64_t;

    using auint8_t = std::atomic_uint8_t;
    using auint16_t = std::atomic_uint16_t;
    using auint32_t = std::atomic_uint32_t;
    using auint64_t = std::atomic_uint64_t;

    using aintptr_t = std::atomic_intptr_t;
    using auintptr_t = std::atomic_uintptr_t;
    using asize_t = std::atomic_size_t;

}
#endif //!__AIRATOIMC__H__