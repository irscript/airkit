
--测试
target("main")
    set_kind("binary")
    add_deps("AirkitCore")
    add_files("**.cpp")