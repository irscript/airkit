#ifndef __AIRPRINT_INC__
#define __AIRPRINT_INC__

// 控制台打印
#include <Airkit/Precompiled.h>
#include <Airkit/Core/Plat/AirThread.h>
namespace airkit
{
    struct Print
    {
        // 打印颜色
        enum
        {
            Black,       // 黑色
            Blue,        // 蓝色
            Green,       // 绿色
            Wathet,      // 浅蓝色
            Red,         // 红色
            Purple,      // 紫色
            Yellow,      // 黄色
            White,       // 白色
            Grey,        // 灰色
            LightBlue,   // 淡蓝色
            LightGreen,  // 淡绿色
            Aqua,        // 淡浅绿色
            LightRed,    // 淡红色
            Lavender,    // 淡紫色
            FaintYellow, // 淡黄色
            BrightWhite, // 亮白色
        };
        // 控制台锁定
        inline static void lock() { mSync.lock(); }
        // 控制台解锁
        inline static void unlock() { mSync.unlock(); }
        // 不带锁打印
        static void print(int32_t clr, cstring fmt, ...);

        // ----------------通用带锁函数---------------

        // 普通信息打印
        template <typename... Args>
        inline static void info(Args... args)
        {
            Spinlocker lck(mSync);
            print(White, args...);
        }
        // 警告信息打印
        template <typename... Args>
        inline static void warning(Args... args)
        {
            Spinlocker lck(mSync);
            print(FaintYellow, args...);
        }
        // 错误信息打印
        template <typename... Args>
        inline static void error(Args... args)
        {
            Spinlocker lck(mSync);
            print(LightRed, args...);
        }

        // 错误信息打印
        template <typename... Args>
        inline static void track(Args... args)
        {
#ifdef Air_Enable_Track
            Spinlocker lck(mSync);
            print(Aqua, args...);
#endif
        }

    private:
        static Spinlock mSync;     // 同步锁
        static char mBuffer[2048]; // 日志缓存
    };

#ifdef Air_Plat_Window
#include <Airkit/Core/PlatWin/airPrint.inl>
#else
#error "控制台打印类未实现！"
#endif

}
#endif // __AIRPRINT_INC__