set_project("moon")
set_languages("c17", "cxx20")
set_warnings("more")
set_defaultmode("releasedbg")
set_allowedmodes("debug", "releasedbg")
set_plat(os.host())
set_arch(os.arch())

add_rules("plugin.compile_commands.autoupdate", "mode.debug", "mode.releasedbg")

add_defines(is_mode("debug") and "DEBUG" or "NDEBUG", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX")
set_symbols("debug")
set_optimize(is_mode("debug") and "none" or "fastest")
set_strip(is_mode("debug") and "none" or "all")

if is_host("windows") then 
    set_encodings("utf-8")
end

target("mimalloc")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/mimalloc")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
        add_syslinks("Advapi32")
    else
        set_toolchains("clang")
    end

    add_includedirs("./third/mimalloc/include")
    add_files("./third/mimalloc/src/static.c")

target("lua")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/lua")
    add_defines("MAKE_LIB")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        add_defines("LUA_USE_LINUX")
        set_toolchains("clang")
    end

    add_includedirs("./third/lua")
    add_files("./third/lua/onelua.c")

target("lualib")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/lualib")
    add_defines("SOL_ALL_SAFETIES_ON", "ASIO_STANDALONE", "ASIO_NO_DEPRECATED", "MOON_ENABLE_MIMALLOC")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_includedirs("./src")
    add_includedirs("./src/moon")
    add_includedirs("./src/moon/core")
    add_includedirs("./third/mimalloc/include")
    add_includedirs("./third/recastnavigation/Detour/Include")
    add_includedirs("./third/recastnavigation/DetourCrowd/Include")
    add_includedirs("./third/recastnavigation/DetourTileCache/Include")
    add_includedirs("./third/recastnavigation/Recast/Include")
    add_files("./third/fastlz/**.c")
    add_files("./third/kcp/**.c")
    add_files("./third/recastnavigation/**.cpp")
    add_files("./third/yyjson/**.c")

target("pb")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/pb")
    add_defines("PB_IMPLEMENTATION", "SOL_ALL_SAFETIES_ON", "LUA_LIB")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third/pb")
    add_includedirs("./third/lua")
    add_files("./third/pb/**.c")

target("lcrypt")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/lcrypt")
    add_defines("SOL_ALL_SAFETIES_ON")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_files("./third/lcrypt/**.c")

target("lmongo")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/lmongo")
    add_defines("SOL_ALL_SAFETIES_ON")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
        add_syslinks("Ws2_32")
    else
        add_defines("LUA_USE_LINUX")
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_files("./third/lmongo/**.c")

target("lfmt")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/lfmt")
    add_defines("SOL_ALL_SAFETIES_ON")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
        add_syslinks("Ws2_32")
    else
        add_defines("LUA_USE_LINUX")
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_files("./third/lfmt/**.c")

target("sharetable")
    set_default(false)
    set_kind("static")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/sharetable")
    add_defines("SOL_ALL_SAFETIES_ON")
    add_deps("lua")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_files("./third/sharetable/**.c")

target("moon")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/moon")
    add_defines("ASIO_STANDALONE", "ASIO_NO_DEPRECATED", "MOON_ENABLE_MIMALLOC")
    add_deps("mimalloc", "lua", "lualib", "pb", "lcrypt", "lmongo", "lfmt", "sharetable")

    if is_host("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_cflags("/experimental:c11atomics")
    else
        set_toolchains("clang")
    end

    add_includedirs("./")
    add_includedirs("./third")
    add_includedirs("./third/lua")
    add_includedirs("./src")
    add_includedirs("./src/moon")
    add_includedirs("./src/moon/core")
    add_includedirs("./src/moon/core/network")
    add_includedirs("./src/moon/services")
    add_includedirs("./third/mimalloc/include")
    add_includedirs("./third/recastnavigation/Detour/Include")
    add_includedirs("./third/recastnavigation/DetourCrowd/Include")
    add_includedirs("./third/recastnavigation/DetourTileCache/Include")
    add_includedirs("./third/recastnavigation/Recast/Include")
    add_files("./src/lualib-src/**.cpp")
    add_files("./src/moon/**.cpp")

target("all")
    set_default(true)
    set_kind("phony")
    add_deps("mimalloc", "lua", "lualib", "pb", "lcrypt", "lmongo", "lfmt", "sharetable", "moon", { inherit = false })

    after_build(function (target)
        if is_host("windows") then
            os.cp("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/moon/moon.exe", "./moon.exe")
        else
            os.cp("$(buildir)/$(host)/x64/"..(is_mode("debug") and "debug" or "release").."/moon/moon", "./moon")
        end
    end)