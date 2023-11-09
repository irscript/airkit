
#include <Airkit/Core/Plat/airPrint.h>
#include <Airkit/Core/Plat/AirThread.h>

namespace airkit
{
    Spinlock Print::mSync;     // 同步锁
    char Print::mBuffer[2048]; // 日志缓存
}