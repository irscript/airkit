#include <Airkit/Core/Thread/airThdPools.h>
#include <Airkit/Core/Plat/airPrint.h>
using namespace airkit;

struct TaskA : public ITPTask
{

    TaskA(int id) : ITPTask(id), mID(id) {}
    int mID;
    int mCount = 0;
    virtual Result entry() override
    {
        if (mCount < 100)
        {
            Print::error("%d : count = %d , priority = %u\n", mID, mCount, mTrue);
            mCount++;
            return Result::Continue;
        }

        return Result::Finished;
    }
};
struct TaskB : public ITPTask
{

    TaskB(int id) : ITPTask(id), mID(id) {}
    int mID;
    int mCount = 0;
    virtual Result entry() override
    {
        if (mCount < 100)
        {
            Print::warning("%d : count = %d , priority = %u\n", mID, mCount, mTrue);
            mCount++;
            return Result::Continue;
        }

        return Result::Finished;
    }
};
// 内存分页测试
int main(int argc, char **argv)
{
    TPScheduler domain;

    domain.init(16, 8);
    for (int i = 0; i < 20; i++)
    {
        i % 2 == 0 ? domain.addTask(new TaskA(i) ): domain.addTask(new TaskB(i));
    }
    domain.wait();
    return 0;
}
