/** 编译发布模式宏
 *
 * debug ： Air_Publish_Debug
 * release ： Air_Publish_Release
 *
 */

/** 平台宏
 *
 * window ：Air_Plat_Window
 * Linux ：Air_Plat_Linux
 * pthread : Air_Plat_Pthread
 */
#define Air_Plat_Window
#define Air_Plat_Pthread

/** 架构宏
 *
 * x86: Air_Arch_X86
 * x64: Air_Arch_X64
 *
 */

/** CPU地址空间大小
 * Air_CPU_Bit32
 * Air_CPU_Bit64
 */
#define Air_CPU_Bit64

// ----------------------其他开关宏-------------------------------

// 跟踪API开启宏
#define Air_Enable_Track
//内存调试开启宏
#define Air_Enable_MemDbg