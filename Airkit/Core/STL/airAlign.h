#ifndef __AIRALIGN__H__
#define __AIRALIGN__H__

#include <Airkit/Precompiled.h>
namespace airkit
{
    // 数值对齐相关的算法
    struct Align
    {

        /**
         * @brief 向上大小对齐
         * @tparam T 计算的数据类型
         * @param num 要对齐的值
         * @param base 对齐的基，必须是2的幂值
         * @return 对齐后的值
         */
        template <typename T>
        inline static constexpr T up(T num, T base) { return (num + base - 1) & (~(base - 1)); }

        /**
         * @brief 向下大小对齐
         * @tparam T 计算的数据类型
         * @param num 要对齐的值
         * @param base 对齐的基，必须是2的幂值
         * @return 对齐后的值
         */
        template <typename T>
        inline static constexpr T down(T num, T base) { return (num & ~(base - 1)); }
    };
}

#endif //!__AIRALIGN__H__