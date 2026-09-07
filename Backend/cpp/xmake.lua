set_project("backend")
set_languages("c17", "cxx20")
set_warnings("more")
set_defaultmode("releasedbg")
set_allowedmodes("debug", "releasedbg")
set_plat(os.host())
set_arch(os.arch())
add_rules("plugin.compile_commands.autoupdate", "mode.debug", "mode.releasedbg")
add_defines(is_mode("debug") and "_DEBUG" or "NDEBUG", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", 
    (is_host("windows") and is_mode("releasedbg")) and "U_DISABLE_RENAMING=1" or "U_DISABLE_RENAMING=0")

if is_host("windows") then
    set_encodings("utf-8")
    add_linkdirs("C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\ucrt\\x64",
        "C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\um\\x64")
    add_requires("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu", 
        "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp", "vcpkg::grpc", "vcpkg::curl", "vcpkg::cpr", 
        "vcpkg::picojson", "vcpkg::sentry-native", "vcpkg::jsoncpp", "vcpkg::jwt-cpp", "vcpkg::libmariadb", "vcpkg::hiredis", 
        "vcpkg::sqlite3", "vcpkg::libpq", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::drogon", 
        {configs = {shared = true, runtimes = is_mode("debug") and "MDd" or "MD", debug = is_mode("debug")}})
else
    add_requires("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu", 
        "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp", "vcpkg::grpc", "vcpkg::curl", "vcpkg::cpr", 
        "vcpkg::picojson", "vcpkg::sentry-native", "vcpkg::jsoncpp", "vcpkg::jwt-cpp", "vcpkg::libmariadb", "vcpkg::hiredis", 
        "vcpkg::sqlite3", "vcpkg::libpq", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::drogon")
end

function add_depend()
    if is_host("windows") then
        set_toolchains("msvc")
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_syslinks("Advapi32", "ws2_32")
        add_packages("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu", 
            "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp", "vcpkg::grpc", "vcpkg::curl", "vcpkg::cpr", 
            "vcpkg::picojson", "vcpkg::sentry-native", "vcpkg::jsoncpp", "vcpkg::jwt-cpp", "vcpkg::libmariadb", "vcpkg::hiredis", 
            "vcpkg::sqlite3", "vcpkg::libpq", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::drogon", "vcpkg::abseil", "vcpkg::aws-c-auth", 
            "vcpkg::aws-c-cal", "vcpkg::aws-c-common", "vcpkg::aws-c-compression", "vcpkg::aws-c-event-stream", "vcpkg::aws-c-http", 
            "vcpkg::aws-c-io", "vcpkg::aws-c-mqtt", "vcpkg::aws-c-s3", "vcpkg::aws-c-sdkutils", "vcpkg::aws-checksums", "vcpkg::aws-crt-cpp", 
            "vcpkg::protobuf", "vcpkg::re2", "vcpkg::utf8-range")
    else
        set_toolchains("clang")
        -- if is_host("windows") and is_mode("debug") then 
        --     add_links("liblz4d.a", "libfmtd.a", "libyaml-cppd.a", "libhiredisd.a", "libhiredis_ssld.a", "libspdlogd.a")
        -- end
        -- add_links("libaws-cpp-sdk-dynamodb.a", "libaws-cpp-sdk-kinesis.a", "libaws-cpp-sdk-s3.a", "libaws-cpp-sdk-core.a", "libaws-crt-cpp.a", 
        --     "libaws-c-s3.a", "libaws-c-mqtt.a", "libaws-c-event-stream.a", "libaws-c-auth.a", "libaws-c-sdkutils.a", "libaws-c-http.a", 
        --     "libaws-c-cal.a", "libaws-checksums.a", "libaws-c-compression.a", "libaws-c-io.a", "libaws-c-common.a", "libcurl.a", "libcpr.a", 
        --     "libcares.a", "libdrogon.a", "libtrantor.a", "libhiredis.a", "libhiredis_ssl.a", "libmariadb.a", "libmariadbclient.a", "libsqlite3.a", 
        --     "libpq.a", "libecpg.a", "libecpg_compat.a", "libpgtypes.a", "libpgcommon.a", "libpgport.a", "libssl.a", "libcrypto.a", "libz.a", 
        --     "libzstd.a", "liblz4.a", "libuuid.a", "libyaml-cpp.a", "libtinyxml2.a", "libfmt.a", "libspdlog.a", "libjsoncpp.a", "libbrotlicommon.a", 
        --     "libbrotlidec.a", "libbrotlienc.a", "libsentry.a", "libcrashpad_client.a", "libcrashpad_compat.a", "libcrashpad_util.a", "libgpr.a", 
        --     "libgrpc.a", "libgrpc_authorization_provider.a", "libgrpc_plugin_support.a", "libgrpc_unsecure.a", "libgrpc++.a", "libgrpc++_alts.a", 
        --     "libgrpc++_error_details.a", "libgrpc++_reflection.a", "libgrpc++_unsecure.a", "libgrpcpp_channelz.a", "libcrashpad_snapshot.a", 
        --     "libmini_chromium.a", "libicuio.a", "libicutu.a", "libicui18n.a", "libicuuc.a", "libicudata.a")
        add_packages("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu", 
            "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp", "vcpkg::grpc", "vcpkg::curl", "vcpkg::cpr", 
            "vcpkg::picojson", "vcpkg::sentry-native", "vcpkg::jsoncpp", "vcpkg::jwt-cpp", "vcpkg::libmariadb", "vcpkg::hiredis", 
            "vcpkg::sqlite3", "vcpkg::libpq", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::drogon", "vcpkg::abseil", "vcpkg::aws-c-auth", 
            "vcpkg::aws-c-cal", "vcpkg::aws-c-common", "vcpkg::aws-c-compression", "vcpkg::aws-c-event-stream", "vcpkg::aws-c-http", 
            "vcpkg::aws-c-io", "vcpkg::aws-c-mqtt", "vcpkg::aws-c-s3", "vcpkg::aws-c-sdkutils", "vcpkg::aws-checksums", "vcpkg::aws-crt-cpp", 
            "vcpkg::protobuf", "vcpkg::re2", "vcpkg::utf8-range")
    end
    
    add_includedirs("common")
    add_files("common/src/*.cpp")
end

function copy_depend(target)
    cprint("${bright blue}正在复制依赖文件...")

    if is_host("windows") then
        os.cp(os.getenv("VCPKG_ROOT") .. "/installed/x64-windows/" .. (is_mode("debug") and "debug/bin/" or "bin/") .. "*.dll",
            "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
        os.cp(os.getenv("VCPKG_ROOT") .. "/installed/x64-windows/tools/sentry-native/crashpad_handler.exe",
            "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    else
        os.cp(os.getenv("VCPKG_ROOT") .. "/installed/x64-linux/tools/sentry-native/crashpad_handler",
            "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    end

    if is_mode("debug") then 
        os.cp("./config_debug.json", "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    end

    os.cp("./config.json", "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    os.cp("./usercenter.pem", "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())

    cprint("${bright green}复制依赖文件完成!")
end

function compile_protobuf()
    local output_dir = path.absolute("./protos/build")
    local protoc = is_host("windows") and os.getenv("VCPKG_ROOT") .. "/installed/x64-windows/tools/protobuf/protoc.exe" or 
        os.getenv("VCPKG_ROOT") .. "/installed/x64-linux/tools/protobuf/protoc"
    local proto_root = path.absolute("./protos")

    local proto_files = {}
    if is_host("windows") then
        local result = os.iorun("powershell -Command \"Get-ChildItem -Path ./protos -Filter *.proto -Recurse | %% { $_.FullName }\"")
        for path in result:gmatch("[^\r\n]+") do
            table.insert(proto_files, path)
        end
    else
        local result = os.iorun("find ./protos -name '*.proto'")
        for path in result:gmatch("[^\r\n]+") do
            table.insert(proto_files, path)
        end
    end

    local protoc_cmd = is_host("windows") and {
        "--proto_path=" .. proto_root,
        "--cpp_out=" .. output_dir,
        "--grpc_out=" .. output_dir,
        "--plugin=protoc-gen-grpc=" .. os.getenv("VCPKG_ROOT") .. "/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe"
    } or {
        "--proto_path=" .. proto_root,
        "--cpp_out=" .. output_dir,
        "--grpc_out=" .. output_dir,
        "--plugin=protoc-gen-grpc=" .. os.getenv("VCPKG_ROOT") .. "/installed/x64-linux/tools/grpc/grpc_cpp_plugin"
    }

    if is_host("windows") then
        protoc_cmd[#protoc_cmd] = protoc_cmd[#protoc_cmd]:gsub("/", "\\")
        for i, path in ipairs(proto_files) do
            proto_files[i] = path:gsub("/", "\\")
        end
    end

    cprint("${bright blue}正在编译Protobuf文件...")
    local ok, err = os.execv(protoc, table.join(protoc_cmd, proto_files))
    if not ok then
        cprint("${bright red}Protobuf编译失败!")
        cprint("${yellow}执行命令: protoc " .. table.concat(table.join(protoc_cmd, proto_files), " "))
        cprint("${yellow}错误信息: " .. (err or "未知错误"))
        raise("Protobuf编译终止!")
    end
    cprint("${bright green}Protobuf编译完成!")
end

target("hik")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/hik")
    set_pcheader("hik/include/pch.h")
    set_pcxxheader("hik/include/pch.h")

    add_depend()

    add_includedirs("hik/include")
    add_files("hik/src/*.cpp")

    after_build(copy_depend)

target("cbis")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/cbis")
    set_pcheader("cbis/include/pch.h")
    set_pcxxheader("cbis/include/pch.h")

    add_depend()

    add_includedirs("cbis/include")
    add_files("cbis/src/*.cpp")

    after_build(copy_depend)

target("clean")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/clean")
    set_pcheader("clean/include/pch.h")
    set_pcxxheader("clean/include/pch.h")

    add_depend()

    add_includedirs("clean/include")
    add_files("clean/src/*.cpp")

    after_build(copy_depend)

target("funeral")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/funeral")
    set_pcheader("funeral/include/pch.h")
    set_pcxxheader("funeral/include/pch.h")

    add_depend()

    add_includedirs("funeral/include")
    add_files("funeral/src/*.cpp")

    after_build(copy_depend)

target("gateway")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/gateway")
    set_pcheader("gateway/include/pch.h")
    set_pcxxheader("gateway/include/pch.h")

    add_depend()

    add_includedirs("gateway/include")
    add_files("gateway/src/*.cpp")

    after_build(copy_depend)

target("notify")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/notify")
    set_pcheader("notify/include/pch.h")
    set_pcxxheader("notify/include/pch.h")

    add_depend()

    add_includedirs("notify/include")
    add_files("notify/src/*.cpp")

    after_build(copy_depend)

target("pay")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/pay")
    set_pcheader("pay/include/pch.h")
    set_pcxxheader("pay/include/pch.h")

    add_depend()

    add_includedirs("pay/include")
    add_files("pay/src/*.cpp")

    after_build(copy_depend)

target("repair")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/repair")
    set_pcheader("repair/include/pch.h")
    set_pcxxheader("repair/include/pch.h")

    add_depend()

    add_includedirs("repair/include")
    add_files("repair/src/*.cpp")

    after_build(copy_depend)

target("qdzy")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/qdzy")
    set_pcheader("qdzy/include/pch.h")
    set_pcxxheader("qdzy/include/pch.h")

    add_depend()

    add_includedirs("qdzy/include")
    add_files("qdzy/src/*.cpp")

    after_build(copy_depend)

target("tow")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/tow")
    set_pcheader("tow/include/pch.h")
    set_pcxxheader("tow/include/pch.h")

    add_depend()

    add_includedirs("tow/include")
    add_files("tow/src/*.cpp")

    after_build(copy_depend)

target("daemon")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/daemon")
    set_pcheader("daemon/include/pch.h")
    set_pcxxheader("daemon/include/pch.h")

    add_depend()

    add_includedirs("daemon/include")
    add_files("daemon/src/*.cpp")

    after_build(copy_depend)

target("dingtalk")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/dingtalk")
    set_pcheader("dingtalk/include/pch.h")
    set_pcxxheader("dingtalk/include/pch.h")

    add_depend()

    add_includedirs("dingtalk/include")
    add_files("dingtalk/src/*.cpp")

    after_build(copy_depend)

target("chat")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/chat")
    set_pcheader("chat/include/pch.h")
    set_pcxxheader("chat/include/pch.h")

    add_depend()

    add_includedirs("chat/include")
    add_files("chat/src/*.cpp")

    after_build(copy_depend)

target("all")
    set_default(true)
    set_kind("phony")
    add_deps("hik", "cbis", "clean", "funeral", "gateway", "notify", "pay", "repair", "tow", "daemon", "dingtalk", "qdzy", "chat",
        { inherit = false })

task("vcpkg_install")
    set_menu {
        usage = "xmake vcpkg_install",
        description = "Install all vcpkg dependencies for the project"
    }
    on_run(function ()
        local cmd = "vcpkg install zlib lz4 fmt spdlog zstd openssl icu brotli tinyxml2 c-ares aws-sdk-cpp grpc[core,codegen] " ..
            "curl[core,ssl,openssl,brotli,c-ares,non-http,zstd] cpr[core,ssl] picojson sentry-native[core,backend,transport] jsoncpp " ..
            "jwt-cpp libmariadb[core,openssl,zstd] hiredis[core,ssl] sqlite3[core,json1,zlib] libpq[core,icu,lz4,openssl,zlib,zstd] " ..
            "trantor yaml-cpp drogon[core,orm,mysql,postgres,sqlite3,redis,yaml]"
        os.exec(cmd)
    end)
