add_rules("mode.debug", "mode.release")

set_languages("c++17","c17")
add_includedirs("./")
--add_defines("Air_Plat_Pthread")

includes(
    "Airkit/Core"   --核心库
    ,"Example"      --实例代码
    ,"Test"      --测试代码
)


