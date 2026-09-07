set_project("backend")
set_languages("c17", "cxx20")
set_warnings("more")
set_defaultmode("releasedbg")
set_allowedmodes("debug", "releasedbg")
set_plat(os.host())
set_arch(os.arch())
add_rules("plugin.compile_commands.autoupdate", "mode.debug", "mode.releasedbg")
add_defines(is_mode("debug") and "_DEBUG" or "NDEBUG", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "U_DISABLE_RENAMING=0", "JSON_MultipleHeaders",
    "ENABLE_SM4_ECB", "ENABLE_SM4_OFB", "ENABLE_SM4_CFB", "ENABLE_SM4_CCM", "ENABLE_SM4_XTS", "ENABLE_SM4_CBC_MAC", "ENABLE_SM2_EXTS",
    "ENABLE_SM3_XMSS", "_WINSOCK_DEPRECATED_NO_WARNINGS")

if is_host("windows") then 
    add_defines("WIN32", "_WIN32")
    set_encodings("utf-8")
    add_linkdirs("C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\ucrt\\x64",
        "C:\\Program Files (x86)\\Windows Kits\\10\\lib\\10.0.22621.0\\um\\x64")
    add_requires("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu",
        "vcpkg::abseil[core,cxx17]", "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp",
        "vcpkg::curl[core,ssl,openssl,brotli,c-ares,non-http,zstd]", "vcpkg::cpr[core,ssl]", "vcpkg::picojson",
        "vcpkg::sentry-native[core,backend,transport]", "vcpkg::jsoncpp", "vcpkg::jwt-cpp", "vcpkg::protobuf",
        "vcpkg::libmariadb[core,openssl,zstd]", "vcpkg::hiredis[core,ssl]", "vcpkg::sqlite3[core,json1,zlib]", "vcpkg::re2",
        "vcpkg::utf8-range", "vcpkg::grpc[core,codegen]", "vcpkg::libpq[core,icu,lz4,openssl,zlib,zstd]", "vcpkg::trantor", "vcpkg::yaml-cpp",
        "vcpkg::drogon[core,orm,mysql,postgres,sqlite3,redis,yaml]", "vcpkg::entt",
        { configs = { shared = true, runtimes = is_mode("debug") and "MDd" or "MD", debug = is_mode("debug") } })
else
    add_requires("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu", "vcpkg::protobuf",
        "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::s2n", "vcpkg::aws-sdk-cpp", "vcpkg::re2", "vcpkg::utf8-range",
        "vcpkg::grpc[core,codegen]", "vcpkg::curl[core,ssl,openssl,brotli,c-ares,non-http,zstd]", "vcpkg::cpr[core,ssl]", "vcpkg::picojson",
        "vcpkg::abseil[core,cxx17]", "vcpkg::sentry-native[core,backend,transport]", "vcpkg::jsoncpp", "vcpkg::jwt-cpp",
        "vcpkg::libmariadb[core,openssl,zstd]", "vcpkg::hiredis[core,ssl]", "vcpkg::sqlite3[core,json1,zlib]",
        "vcpkg::libpq[core,icu,lz4,openssl,zlib,zstd]", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::libuuid",
        "vcpkg::drogon[core,orm,mysql,postgres,sqlite3,redis,yaml]", "vcpkg::entt")
end

function add_depend_thirdparty()
    if is_host("windows") then
        set_toolchains("msvc")
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_syslinks("Advapi32", "ws2_32")
    else
        set_toolchains("clang")
    end
end

function add_depend()
    if is_host("windows") then
        set_toolchains("msvc")
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_syslinks("Advapi32", "ws2_32")
        add_packages("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu",
            "vcpkg::abseil[core,cxx17]", "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::aws-sdk-cpp",
            "vcpkg::curl[core,ssl,openssl,brotli,c-ares,non-http,zstd]", "vcpkg::cpr[core,ssl]", "vcpkg::picojson",
            "vcpkg::sentry-native[core,backend,transport]", "vcpkg::jsoncpp", "vcpkg::re2", "vcpkg::utf8-range", "vcpkg::protobuf",
            "vcpkg::jwt-cpp", "vcpkg::libmariadb[core,openssl,zstd]", "vcpkg::hiredis[core,ssl]", "vcpkg::sqlite3[core,json1,zlib]",
            "vcpkg::grpc[core,codegen]", "vcpkg::libpq[core,icu,lz4,openssl,zlib,zstd]", "vcpkg::trantor", "vcpkg::yaml-cpp",
            "vcpkg::drogon[core,orm,mysql,postgres,sqlite3,redis,yaml]", "vcpkg::entt")
        if is_mode("debug") then 
            add_links("libprotocd.lib", "libprotobuf-lited.lib", "libprotobufd.lib", "icudtd.lib", "icutud.lib", "icuiod.lib", "icuind.lib",
            "icuucd.lib", "hiredis_ssld.lib", "hiredisd.lib", "libcurl-d.lib", "spdlogd.lib", "fmtd.lib", "yaml-cppd.lib", "lz4d.lib",
            "zlibd.lib")
        else
            add_links("libprotoc.lib", "libprotobuf-lite.lib", "libprotobuf.lib", "icudt.lib", "icutu.lib", "icuio.lib", "icuin.lib",
            "icuuc.lib", "hiredis_ssl.lib", "hiredis.lib", "libcurl.lib", "spdlog.lib", "fmt.lib", "yaml-cpp.lib", "lz4.lib", "zlib.lib")
        end
        add_links(
            "abseil_dll.lib", "absl_decode_rust_punycode.lib", "absl_demangle_rust.lib", "absl_flags_commandlineflag_internal.lib",
            "absl_flags_commandlineflag.lib", "absl_flags_config.lib", "absl_flags_internal.lib", "absl_flags_marshalling.lib",
            "absl_flags_parse.lib", "absl_flags_private_handle_accessor.lib", "absl_flags_program_name.lib", "absl_flags_reflection.lib",
            "absl_flags_usage_internal.lib", "absl_flags_usage.lib", "absl_log_flags.lib", "absl_log_internal_structured_proto.lib",
            "absl_poison.lib", "absl_random_internal_entropy_pool.lib", "absl_tracing_internal.lib", "absl_utf8_for_code_point.lib",
            "address_sorting.lib", "aws-c-auth.lib", "aws-c-cal.lib", "aws-c-common.lib", "aws-c-compression.lib", "aws-c-event-stream.lib",
            "aws-c-http.lib", "aws-c-io.lib", "aws-c-mqtt.lib", "aws-c-s3.lib", "aws-c-sdkutils.lib", "aws-checksums.lib",
            "aws-cpp-sdk-core.lib", "aws-cpp-sdk-dynamodb.lib", "aws-cpp-sdk-kinesis.lib", "aws-cpp-sdk-s3.lib", "aws-crt-cpp.lib",
            "brotlicommon.lib", "brotlidec.lib", "brotlienc.lib", "cares.lib", "cpr.lib", "drogon.lib", "gpr.lib",
            "grpc_authorization_provider.lib", "grpc_plugin_support.lib", "grpc_unsecure.lib", "grpc.lib", "grpc++_alts.lib",
            "grpc++_error_details.lib", "grpc++_reflection.lib", "grpc++_unsecure.lib", "grpc++.lib", "grpcpp_channelz.lib",
            "jsoncpp.lib", "libcrypto.lib", "libecpg_compat.lib", "libecpg.lib", "libmariadb.lib", "libpgcommon.lib", "libpgport.lib",
            "libpgtypes.lib", "libpq.lib", "libssl.lib", "re2.lib", "sentry.lib", "sqlite3.lib", "tinyxml2.lib", "trantor.lib",
            "upb_base_lib.lib", "upb_json_lib.lib", "upb_mem_lib.lib", "upb_message_lib.lib", "upb_mini_descriptor_lib.lib",
            "upb_textformat_lib.lib", "upb_wire_lib.lib", "utf8_range.lib", "utf8_validity.lib", "zstd.lib")
    else
        set_toolchains("clang")
        add_packages("vcpkg::zlib", "vcpkg::lz4", "vcpkg::fmt", "vcpkg::spdlog", "vcpkg::zstd", "vcpkg::openssl", "vcpkg::icu",
            "vcpkg::protobuf", "vcpkg::brotli", "vcpkg::tinyxml2", "vcpkg::c-ares", "vcpkg::s2n", "vcpkg::aws-sdk-cpp", "vcpkg::utf8-range",
            "vcpkg::re2", "vcpkg::abseil[core,cxx17]", "vcpkg::curl[core,ssl,openssl,brotli,c-ares,non-http,zstd]", "vcpkg::cpr[core,ssl]",
            "vcpkg::picojson", "vcpkg::grpc[core,codegen]", "vcpkg::sentry-native[core,backend,transport]", "vcpkg::jsoncpp", "vcpkg::jwt-cpp",
            "vcpkg::libmariadb[core,openssl,zstd]", "vcpkg::hiredis[core,ssl]", "vcpkg::sqlite3[core,json1,zlib]", 
            "vcpkg::libpq[core,icu,lz4,openssl,zlib,zstd]", "vcpkg::trantor", "vcpkg::yaml-cpp", "vcpkg::libuuid", 
            "vcpkg::drogon[core,orm,mysql,postgres,sqlite3,redis,yaml]", "vcpkg::entt")
        if is_mode("debug") then 
            add_links("libprotocd.a", "libprotobuf-lited.a", "libprotobufd.a", "libicudtd.a", "libicutud.a", "libicuiod.a", "libicuind.a",
            "libicuucd.a", "libhiredis_ssld.a", "libhiredisd.a", "libcurl-d.a", "libspdlogd.a", "libfmtd.a", "libyaml-cppd.a", "liblz4d.a",
            "libzlibd.a")
        else
            add_links("libprotoc.a", "libprotobuf-lite.a", "libprotobufd.a", "libicudt.a", "libicutu.a", "libicuio.a", "libicuin.a",
            "libicuuc.a", "libhiredis_ssl.a", "libhiredis.a", "libcurl.a", "libspdlog.a", "libfmt.a", "libyaml-cpp.a", "liblz4.a",
            "libzlib.a")
        end
        add_links(
            "libabsl_decode_rust_punycode.a", "libabsl_demangle_rust.a", "libabsl_flags_commandlineflag_internal.a",
            "libabsl_flags_commandlineflag.a", "libabsl_flags_config.a", "libabsl_flags_internal.a", "libabsl_flags_marshalling.a",
            "libabsl_flags_parse.a", "libabsl_flags_private_handle_accessor.a", "libabsl_flags_program_name.a", "libabsl_flags_reflection.a",
            "libabsl_flags_usage_internal.a", "libabsl_flags_usage.a", "libabsl_log_flags.a", "libabsl_log_internal_structured_proto.a",
            "libabsl_poison.a", "libabsl_random_internal_entropy_pool.a", "libabsl_tracing_internal.a", "libabsl_utf8_for_code_point.a",
            "libaddress_sorting.a", "libaws-c-auth.a", "libaws-c-cal.a", "libaws-c-common.a", "libaws-c-compression.a", "libaws-c-event-stream.a",
            "libaws-c-http.a", "libaws-c-io.a", "libaws-c-mqtt.a", "libaws-c-s3.a", "libaws-c-sdkutils.a", "libaws-checksums.a",
            "libaws-cpp-sdk-core.a", "libaws-cpp-sdk-dynamodb.a", "libaws-cpp-sdk-kinesis.a", "libaws-cpp-sdk-s3.a", "libaws-crt-cpp.a",
            "libbrotlicommon.a", "libbrotlidec.a", "libbrotlienc.a", "libcares.a", "libcpr.a", "libdrogon.a", "libgpr.a",
            "libgrpc_authorization_provider.a", "libgrpc_plugin_support.a", "libgrpc_unsecure.a", "libgrpc.a", "libgrpc++_alts.a",
            "libgrpc++_error_details.a", "libgrpc++_reflection.a", "libgrpc++_unsecure.a", "libgrpc++.a", "libgrpcpp_channelz.a", "libjsoncpp.a",
            "libcrypto.a", "libecpg_compat.a", "libecpg.a", "libmariadb.a", "libpgcommon.a", "libpgport.a", "libpgtypes.a", "libpq.a",
            "libprotobuf.a", "libssl.a", "libre2.a", "libsentry.a", "libsqlite3.a", "libtinyxml2.a", "libtrantor.a", "libupb_base_lib.a",
            "libupb_json_lib.a", "libupb_mem_lib.a", "libupb_message_lib.a", "libupb_mini_descriptor_lib.a", "libupb_textformat_lib.a",
            "libupb_wire_lib.a", "libutf8_range.a", "libutf8_validity.a", "libz.a", "libzstd.a")
    end
    
    add_includedirs("protos/build", "thirdparty/gmssl/include", "thirdparty/nlohmann_json/include", "thirdparty/casbin/include")
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
        os.cp("./scripts/config_debug.json", "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    else
        os.cp("./scripts/config.json", "$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/" .. target:name())
    end

    cprint("${bright green}复制依赖文件完成!")
end

function compile_protobuf()
    local output_dir = path.absolute("./protos/build")
    local protoc = is_host("windows") and os.getenv("VCPKG_ROOT") .. "/installed/x64-windows/tools/protobuf/protoc.exe" or 
        os.getenv("VCPKG_ROOT") .. "/installed/x64-linux/tools/protobuf/protoc"
    local proto_root = path.absolute("./protos")

    -- 检查是否已经编译过，避免重复编译
    local pb_cc_file = output_dir .. (is_host("windows") and "\\user.pb.cc" or "/user.pb.cc")
    local grpc_pb_cc_file = output_dir .. (is_host("windows") and "\\user.grpc.pb.cc" or "/user.grpc.pb.cc")
    
    if os.exists(pb_cc_file) and os.exists(grpc_pb_cc_file) then
        cprint("${bright green}Protobuf文件已存在，跳过编译!")
        return
    end

    local proto_files = {}
    if is_host("windows") then
        local result = os.iorun("powershell -Command \"Get-ChildItem -Path ./protos -Filter *.proto -Recurse | %% { $_.FullName }\"")
        for path in result:gmatch("[^\r\n]+") do
            table.insert(proto_files, path)
        end
    else
        -- 使用相对于proto_root的路径
        table.insert(proto_files, "user.proto")
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
    local cmd_args = table.join(protoc_cmd, proto_files)
    
    -- 过滤掉空字符串参数
    local filtered_args = {}
    for _, arg in ipairs(cmd_args) do
        if arg and #arg > 0 then
            table.insert(filtered_args, arg)
        end
    end
    
    -- 显示调试信息
    cprint("${bright blue}proto_root: " .. proto_root)
    cprint("${bright blue}proto_files: " .. table.concat(proto_files, ", "))
    cprint("${bright blue}执行命令: protoc " .. table.concat(filtered_args, " "))

    local ok, err = os.execv(protoc, filtered_args)
    if not ok then
        cprint("${bright red}Protobuf编译失败!")
        cprint("${yellow}错误信息: " .. (err or "未知错误"))
        raise("Protobuf编译终止!")
    end
    cprint("${bright green}Protobuf编译完成!")
end

target("gmssl")
    set_default(false)
    set_kind("static")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/thirdparty")
    
    add_depend_thirdparty()

    add_includedirs("thirdparty/gmssl/include")
    add_files("thirdparty/gmssl/src/**.c")
    
target("casbin")
    set_default(false)
    set_kind("static")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/thirdparty")
    
    add_depend_thirdparty()

    set_pcxxheader("thirdparty/casbin/include/casbin/pch.h")
    add_cxxflags("/bigobj")

    add_includedirs("thirdparty/nlohmann_json/include", "thirdparty/casbin/include")
    add_files("thirdparty/casbin/casbin/**.cpp")

target("app")
    set_default(false)
    set_kind("binary")
    set_targetdir("$(builddir)/$(host)/x64/" .. (is_mode("debug") and "debug" or "release") .. "/app")

    before_build(compile_protobuf)
    add_depend()
    add_deps("gmssl", "casbin")

    add_includedirs("app/include")
    add_files("app/src/**.cpp")

    if is_host("windows") then 
        add_files("app/**.rc")
    end 

    after_build(copy_depend)

target("all")
    set_default(true)
    set_kind("phony")
    add_deps("gmssl", "casbin", "app", { inherit = false })