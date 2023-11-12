#include "airThdPools.h"
#include <Airkit/Core/Memory/airMemSys.h>
namespace airkit
{
    bool TPScheduler::addTask(ITPTask *task)
    {
        if (0 == mWaited)
        {
            // 锁定任务池
            mCond.lock();
            // 插入任务
            ITPTask *item = mTasks.back();
            for (; item != mTasks.root();
                 item = item->getPrev())
            {
                if (item->mTrue > task->mInit)
                    break;
            }
            mTasks.insert(item, task);
            // 判断是否可以添加线程
            if (mPools.mIdled == 0 && mPools.mHave != mPools.mMax)
                mPools.add(*this);
            // 解锁任务池
            mCond.unlock();
            // 通知线程可以处理
            mCond.broadcast();
            return true;
        }
        return false;
    }

    bool TPScheduler::getTask(ITPTask *&task)
    {
        if (0 == mWaited)
        {
            // 锁定任务池
            mCond.lock();
            if (true == mTasks.isEmpty())
            {
                mPools.mIdled.fetch_add(1);
                mCond.wait();
                mPools.mIdled.fetch_sub(1);
            }

            // 获取任务
            task = mTasks.entry();
            mTasks.remove(task);
            if (task == mTasks.root())
                task = nullptr;

            // 调整任务优先级
            for (ITPTask *item = mTasks.entry();
                 item != mTasks.root();
                 item = item->getNext())
                task->mTrue++;

            // 解锁任务池
            mCond.unlock();

            return task != nullptr;
        }
        return false;
    }
    void TPScheduler::wait()
    {
        while (true)
        {
            // 判断任务池是否为空
            if (mTasks.isEmpty())
            {
                mWaited = 1;
            }

            // 判断线程释放是否完成
            if (mPools.mHave == mPools.mFinished)
                break;

            // 通知线程处理程序
            mCond.broadcast();
        }
    }

    uintptr_t TPTaskRunable::entry()
    {
        ITPTask *task;
        while (true)
        {
            // 初始
            task = nullptr;
            // 获取任务
            if (mDomian.getTask(task) == true)
            {
                // 处理任务
                auto result = task->entry();
                // 任务完成
                if (ITPTask::Result::Finished == result)
                {
                } // 未完成，继续进入池内等待调度
                else
                {
                    mDomian.addTask(task);
                }
            }
            // 判断线程池是否销毁
            if (mDomian.isQuit())
                break;
        }
        mDomian.onQuit();
        return 0;
    }
    TPTRControler::~TPTRControler()
    {
        if (mRun)
        {
            for (auto i = 0; i < mHave; ++i)
            {
                MemSys::objfree(mRun[i]);
            }

            MemSys::dealloc(mRun, sizeof(TPTaskRunable **) * mMax);
        }
    }
    void TPTRControler::setArgs(TPScheduler &sch, uint16_t max, uint16_t init)
    {
        mMax = max;
        mRun = (TPTaskRunable **)MemSys::alloc(sizeof(TPTaskRunable **) * mMax);
        for (int i = 0; i < init; ++i)
        {
            mRun[i] = MemSys::objnew<TPTaskRunable>(&sch);
        }
        mHave = init;
    }
    inline void TPTRControler::add(TPScheduler &sch)
    {
        if (mHave != mMax)
        {
            mRun[mHave] = MemSys::objnew<TPTaskRunable>(&sch);
            ++mHave;
        }
    }
}
