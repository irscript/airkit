#ifndef __AIRTHDPOOLS__H__
#define __AIRTHDPOOLS__H__
#include <Airkit/Precompiled.h>
#include <Airkit/Core/Thread/airRunable.h>
#include <Airkit/Core/STL/airIList.h>
#include <Airkit/Core/Thread/airAtoimc.h>
// 线程池封装
namespace airkit
{

    // 线程池任务
    struct ITPTask : public IListNode<ITPTask>
    {
        ITPTask(uint16_t priority) : mInit(priority), mTrue(priority) {}
        // 任务处理结果
        enum class Result : uint32_t
        {
            Unknown, // 未知状态

            Initial,  // 任务开始
            Continue, // 任务继续
            Delayed,  // 任务延时
            Finished, // 任务完成
            Max
        };
        // 任务处理入口函数
        virtual Result entry() = 0;

    protected:
        friend struct TPScheduler;
        uint16_t mInit; // 初始优先级
        uint16_t mTrue; // 真实优先级
    };
    // 线程池调度器
    struct TPScheduler;
    // 任务处理线程
    struct TPTaskRunable;
    // 线程池控制器
    struct TPTRControler
    {
        TPTRControler()
            : mMax(0), mHave(0),
              mIdled(0), mFinished(0),
              mRun(nullptr) {}
        ~TPTRControler();

    private:
        // 设置线程的数量
        void setArgs(TPScheduler &sch, uint16_t max, uint16_t init);
        // 添加一个线程
        inline void add(TPScheduler &sch);

        friend struct TPScheduler;
        friend struct TPTaskRunable;
        uint16_t mMax;       // 最大线程数量
        uint16_t mHave;      // 当前线程数量
        auint16_t mIdled;    // 空闲线程数量
        auint16_t mFinished; // 释放线程数量

        TPTaskRunable **mRun; // 任务线程
    };
    // 线程池调度器
    struct TPScheduler
    {

        void init(uint16_t max, uint16_t init) { mPools.setArgs(*this, max, init); }

        // 添加任务
        bool addTask(ITPTask *task);
        // 获取任务
        bool getTask(ITPTask *&task);
        // 等待所有任务完成
        void wait();
        // 是否等待线程退出
        inline bool isQuit() const { return mWaited == 1; }
        inline void onQuit() { mPools.mFinished.fetch_add(1); }

    protected:
        friend struct TaskRunable;
        TPTRControler mPools;  // 线程池
        IList<ITPTask> mTasks; // 任务池
        Condition mCond;       // 竞争的条件
        uint16_t mWaited=0;      // 等待销毁,不准在添加任务
    };
    // 任务处理线程
    struct TPTaskRunable : public IRunable
    {
        TPTaskRunable(TPScheduler *sch) : mDomian(*sch)
        {
            start();
            detch();
        }

        virtual uintptr_t entry() override;

    private:
        TPScheduler &mDomian; // 调度器
    };
}
#endif //!__AIRTHDPOOLS__H__