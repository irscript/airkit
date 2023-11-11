#ifndef __AIRENTRUST__H__
#define __AIRENTRUST__H__

#include <Airkit/Precompiled.h>

namespace airkit
{

    // 委托代理基类
    template <typename Ret, typename... Args>
    class IEntrust
    {
    public:
        virtual ~IEntrust() {}
        // 通过函数指针传参调用
        virtual Ret emit(Args... args) = 0;
    };

    //------------------------单播委托--------------
    // 静态函数委托回调
    template <typename Ret, typename... Args>
    class EntrustStaticImp : public IEntrust<Ret, Args...>
    {
    public:
        using Callback = Ret (*)(Args...);

        EntrustStaticImp(Callback func = nullptr) : mFunc(func) {}
        virtual ~EntrustStaticImp() {}
        virtual Ret emit(Args... args) override { return mFunc(args...); }

    protected:
        // 静态函数指针
        Callback mFunc;
    };

    // 用户指针静态函数委托回调
    template <typename Ret, typename... Args>
    class UserEntrustStaticImp : public IEntrust<Ret, Args...>
    {
    public:
        using Callback = Ret (*)(uintptr, Args...);

        UserEntrustStaticImp(uintptr user = nullptr, Callback func = nullptr) : mUser(user), mFunc(func) {}
        virtual ~UserEntrustStaticImp() {}
        virtual Ret emit(Args... args) override { return mFunc(mUser, args...); }

    protected:
        uintptr_t mUser; // 用户指针
        Callback mFunc;  // 静态函数指针
    };

    // 成员函数委托回调
    template <typename Type, typename Ret, typename... Args>
    class EntrustImp : public IEntrust<Ret, Args...>
    {
    public:
        using Callback = Ret (Type::*)(Args...);
        EntrustImp(Type *obj, Callback method) : mObject(obj), mMethod(method) {}
        virtual ~EntrustImp() {}

        virtual Ret emit(Args... args) override { return (mObject->*mMethod)(args...); }

    protected:
        Type *mObject;    // 对象指针
        Callback mMethod; // 成员函数
    };
    // 可以实现两种回调的模板类
    template <typename Ret, typename... Args>
    class Entrust
    {
    public:
        Entrust() { memset(this, 0, sizeof(Entrust)); }
        Entrust(const Entrust &rhs)
        {
            if (nullptr == rhs.mEmit)
            {
                mEmit = nullptr;
            }
            else
            {
                memcpy(mBuffer, rhs.mBuffer, sizeof(mBuffer));
                mEmit = (IEntrust<Ret, Args...>)(mBuffer);
            }
        }
        virtual ~Entrust() {}
        Entrust &operator=(const Entrust &rhs)
        {
            if (nullptr == rhs.mEmit)
            {
                mEmit = nullptr;
            }
            else
            {
                memcpy(mBuffer, rhs.mBuffer, sizeof(mBuffer));
                mEmit = (IEntrust<Ret, Args...>)(mBuffer);
            }
            return *this;
        }

        // 绑定成员函数回调
        template <typename Type>
        void bind(Type *object, Ret (Type::*method)(Args...))
        {
            int nSize = sizeof(method);
            if (nullptr != object && nullptr != method)
                mEmit = new ((uintptr)mBuffer) EntrustImp<Type, Ret, Args...>(object, method);
            else
                mEmit = nullptr;
        }
        // 绑定静态函数回调
        void bind(Ret (*method)(Args...))
        {
            if (nullptr != method)
                mEmit = new ((uintptr)mBuffer) EntrustStaticImp<Ret, Args...>(method);
            else
                mEmit = nullptr;
        }
        // 绑定用户指针静态函数回调
        void bind(uintptr_t user, Ret (*method)(uintptr, Args...))
        {
            if (nullptr != method)
                mEmit = new ((uintptr)mBuffer) UserEntrustStaticImp<Ret, Args...>(user, method);
            else
                mEmit = nullptr;
        }

        // 函数调用
        Ret emit(Args... args)
        {
            if (nullptr != mEmit)
                return mEmit->emit(args...);
            else
                return Ret(0);
        }
        // 函数调用
        inline Ret call(Args... args) { return emit(args...); }

        // 仿函数调用
        inline Ret operator()(Args... args) { return emit(args...); }

    protected:
        // 指向回调类型的指针
        IEntrust<Ret, Args...> *mEmit;
        // 从mBuffer处构造回调类
        uint8_t mBuffer[32];
    };
}
#endif //!__AIRENTRUST__H__