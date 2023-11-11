#ifndef __AIRRTIMATE__H__
#define __AIRRTIMATE__H__
#include <Airkit/Precompiled.h>

namespace airkit
{
    // 运行时信息系统

    // 运行时信息枚举
    enum class RTIEnum : uint32_t
    {
        Unknown,

        // 类型类别
        Void,    // 空类型
        Builtin, // 内建类型
        Enum,    // 枚举类型
        Struct,  // 结构体类
        Class,   // 类类型
    };
    struct IRTIMate
    {
        RTIEnum mKind;  // 类型的标记
        uint32_t mSize; // 类型大小
        cstring mName;  // 类型名称

        IRTIMate(RTIEnum kind, uint32_t size, cstring name)
            : mKind(kind), mSize(size), mName(name) {}

        // 是否空类型
        inline bool isvoid() const { return this == &VoidMate; }
        // 是否是内建类型
        inline bool isBuiltin() const { return mKind == RTIEnum::Builtin; }
        // 是否是枚举类型
        inline bool isEnum() const { return mKind == RTIEnum::Enum; }
        // 是否是结构体类
        inline bool isStruct() const { return mKind == RTIEnum::Struct; }
        // 是否是类类型
        inline bool isClass() const { return mKind == RTIEnum::Class; }

    protected:
        static IRTIMate VoidMate;
    };
    // 内建类型元信息
    struct BuiltinRTIMate : public IRTIMate
    {
        BuiltinRTIMate(uint32_t size, cstring name)
            : IRTIMate(RTIEnum::Builtin, size, name) {}
    };
    // 枚举类型元信息
    struct EnumRTIMate : public IRTIMate
    {
        IRTIMate &mParent; // 父类元信息

        EnumRTIMate(IRTIMate &parent, uint32_t size, cstring name)
            : IRTIMate(RTIEnum::Enum, size, name),
              mParent(parent) {}
    };

    // 结构体类型元信息
    struct StructRTIMate : public IRTIMate
    {
        IRTIMate &mParent; // 父类元信息

        StructRTIMate(IRTIMate &parent, uint32_t size, cstring name)
            : IRTIMate(RTIEnum::Struct, size, name),
              mParent(parent) {}
    };
    // 类类型元信息
    struct ClassRTIMate : public IRTIMate
    {
        IRTIMate &mParent; // 父类元信息

        ClassRTIMate(IRTIMate &parent, uint32_t size, cstring name)
            : IRTIMate(RTIEnum::Class, size, name),
              mParent(parent) {}
    };

}
#endif //!__AIRRTIMATE__H__