#ifndef __AIRRUNABLE__H__
#define __AIRRUNABLE__H__

#include <Airkit/Precompiled.h>
#include <Airkit/Core/Plat/airThread.h>
namespace airkit
{
    // 线程运行类封装
    struct IRunable : public Thread
    {
        // 需要进行重载的任务函数
        virtual uintptr_t entry() = 0;

        // 启动函数
        inline void start() { Thread::run(entry, this); }

    private:
        static void *entry(void *userptr);
    };
}
#endif //!__AIRRUNABLE__H__