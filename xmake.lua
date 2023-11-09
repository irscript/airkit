add_rules("mode.debug", "mode.release")

set_languages("c++17","c17")
add_includedirs("./")
--add_defines("Air_Plat_Pthread")
set_pcxxheader("Airkit/PreCompiled.h")
includes(
    "Airkit/Core"   --核心库
    ,"Example"      --实例测试
)


