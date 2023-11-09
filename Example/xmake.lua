
--测试
target("test")
    set_kind("binary")
    add_deps("AirkitCore")
    add_files("**.cpp")