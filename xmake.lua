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

package("dxil_spirv")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "deps/dxil-spirv"))
    on_install(function (package)
        local configs = {
            "-DCMAKE_BUILD_TYPE=Release",
        }
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("DirectXShaderCompiler", "dxil_spirv")
add_requires("spirv-tools")

target("DXILDecompiler")
    set_kind("shared")
    add_files("src/API.cpp", "src/DXCompiler.cpp")
    add_includedirs("include/")
    add_includedirs("deps/dxil-spirv")
    add_includedirs("deps/DirectXShaderCompiler/include/")
    add_packages("DirectXShaderCompiler", "dxil_spirv", "spirv-tools")
    after_build(function (target)
        os.cp(
            path.join(target:pkgs()["DirectXShaderCompiler"]:installdir(), "/bin/dxcompiler.dll"),
            target:targetdir()
        )
        os.cp(
            path.join(target:pkgs()["dxil_spirv"]:installdir(), "/bin/dxil-spirv-c-shared.dll"),
            target:targetdir()
        )
    end)
    after_clean(function (target) 
        os.rm(path.join(target:targetdir(), "dxcompiler.dll"))
        os.rm(path.join(target:targetdir(), "dxil-spirv-c-shared.dll"))
    end)
    
