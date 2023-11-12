#ifndef __PRECOMPILED_INC__
#define __PRECOMPILED_INC__

// ------------------预编译头--------------------

// C/C++ 语言标准库文件
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <clocale>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <pthread.h>

#include <new>

// 添加标准的浮点类型
using flt32_t = float;
using flt64_t = double;
using cstring = const char *;

// 通用宏定义

#define this_file() __builtin_FILE()
#define this_line() __LINE__
#define this_func() __PRETTY_FUNCTION__

// 配置宏
#include <Airkit/airConfig.h>

// POIX 平台相关库
#ifdef Air_Plat_Pthread
#include <pthread.h>
#include <semaphore.h>
#endif

// window 平台相关库
#ifdef Air_Plat_Window
#include <windows.h>
#endif

// linux 平台相关库
#ifdef Air_Plat_Linux

#endif

#endif