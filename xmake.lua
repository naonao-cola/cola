set_project("Cola")

set_version("0.0.1")
set_languages("c++20")
add_rules("mode.debug", "mode.release","mode.releasedbg")

-- set features on different platform
if is_plat("macosx") then
    add_defines("_ENABLE_LIKELY_")
elseif is_plat("linux") then
    add_defines("_ENABLE_LIKELY_")
    add_syslinks("pthread")
end

--自动更新vs解决方案结构
add_rules("plugin.vsxmake.autoupdate")
set_policy("build.cuda.devlink", true)
set_encodings("source:utf-8")

includes(
    "tutorial/xmake.lua"
)