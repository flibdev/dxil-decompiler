add_rules("mode.debug", "mode.release")

package("DirectXShaderCompiler")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "deps/DirectXShaderCompiler"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-C " .. path.join(os.scriptdir(), "deps/DirectXShaderCompiler/cmake/caches/PredefinedParams.cmake"))
        table.insert(configs, "-DCMAKE_BUILD_TYPE=Debug")
        table.insert(configs, "-DLLVM_BUILD_TOOLS=OFF")
        table.insert(configs, "-DENABLE_SPIRV_CODEGEN=OFF")
        table.insert(configs, "-DHLSL_BUILD_DXILCONV=OFF")
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("DirectXShaderCompiler")

target("DXILDecompiler")
    set_kind("shared")
    add_files("src/API.cpp")
    add_includedirs("deps/dxil-spirv")
    add_includedirs("deps/DirectXShaderCompiler/include/")
    add_packages("DirectXShaderCompiler")
    
