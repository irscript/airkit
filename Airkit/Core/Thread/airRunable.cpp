#include "airRunable.h"

namespace airkit
{
    Thread::ID Thread::mMainID = 0;
    void *IRunable::entry(void *userptr)
    {
        IRunable &self = *(IRunable *)userptr;
        auto res = self.entry();
        return (void *)res;
    }
}
