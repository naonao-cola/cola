tutorial_list = {
        "01",
        "02",
        "03",
}

-- add tutorial target one by one
for _, v in pairs(tutorial_list) do
    target(v)
        set_kind("binary")
        -- framework
        add_includedirs("../src/")
        add_headerfiles("../src/**.h")
        add_headerfiles("../src/**.inl")
        add_files("../src/**.cpp")
        -- test-common
        add_includedirs("Common/")
        add_headerfiles("Common/*.h")
        -- test-file
        for index, filedir in ipairs(os.filedirs(string.format("%s/**", v))) do
            -- print(index)
            -- print(filedir)
            local s = filedir
            if s:endswith(".cuh") or s:endswith(".h") then
                 add_headerfiles(filedir)
            end
            if s:endswith(".cu") or s:endswith(".cpp") then
                add_files(filedir)
            end
        end

end
