
--核心库
target("AirkitCore")
    set_pcxxheader("../PreCompiled.h")
    set_kind("shared")
    add_files("**.cpp")