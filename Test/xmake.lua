
--测试
for _, file in ipairs(os.files("*.cpp")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        add_includedirs("../")
        add_deps("AirkitCore")
        add_files(name .. ".cpp")
        add_tests("default")
end