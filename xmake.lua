set_languages("cxx20")
set_arch("x64")

add_rules("mode.debug", "mode.release")

package("DirectXShaderCompiler")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "deps/DirectXShaderCompiler"))
    on_install(function (package)
        local configs = {
            "-C " .. path.join(os.scriptdir(), "deps/DirectXShaderCompiler/cmake/caches/PredefinedParams.cmake"),
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DLLVM_BUILD_TOOLS=OFF",
            "-DENABLE_SPIRV_CODEGEN=OFF",
            "-DHLSL_BUILD_DXILCONV=OFF"
        }
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("DirectXShaderCompiler")

target("DXILDecompiler")
    set_kind("shared")
    add_files("src/API.cpp")
    add_includedirs("include/")
    add_includedirs("deps/dxil-spirv")
    add_includedirs("deps/DirectXShaderCompiler/include/")
    add_packages("DirectXShaderCompiler")
    after_build(function (target)
        dxsc = target:pkgs()["DirectXShaderCompiler"]
        if dxsc ~= nil then 
            os.cp(
                path.join(dxsc:installdir(), "/bin/dxcompiler.dll"),
                target:targetdir()
            )
        end
    end)
    after_clean(function (target) 
        os.rm(path.join(target:targetdir(), "dxcompiler.dll"))
    end)
    
