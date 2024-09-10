﻿tutorial_list = {
        "01",
        "02",
        "03",
        "04",
        "05",
        "06",
        "07",
}

-- add tutorial target one by one
for _, v in pairs(tutorial_list) do
    local target_name = string.format("tutorial-%s", v)
    target(target_name)
        set_kind("binary")
        set_basename(v)
        --set_prefixname("test")
        --config
        add_configfiles("Common/config.h.in",{
                variables = {
                    CURRENT_PATH = os.projectdir():gsub("\\", "/"),
                }
            })
        set_configdir("Common")
        --3rdparty
        add_packages("opencv")
        add_packages("eigen")
        -- framework
        add_includedirs("../cola/")
        add_headerfiles("../cola/**.h")
        add_headerfiles("../cola/**.inl")
        add_files("../cola/**.cpp")
        -- test-common
        add_includedirs("Common/")
        add_headerfiles("Common/**.h")
        add_files("Common/**.cpp")
        -- test-file
        for index, filedir in ipairs(os.filedirs(string.format("%s/**", v))) do
            local s = filedir
            if s:endswith(".cuh") or s:endswith(".h") then
                 add_headerfiles(filedir)
            end
            if s:endswith(".cu") or s:endswith(".cpp") then
                add_files(filedir)
            end
        end
end
