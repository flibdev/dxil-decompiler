#pragma once

#include "Wincludes.h"
#include "Error.hpp"
#include "dxcapi.h"

#include <string>

// Slopped together from bits of DxCompilerDllLoader
class DXCompiler {

    HMODULE m_dll = nullptr;
    DxcCreateInstanceProc m_createFn = nullptr;

    CComPtr<IDxcUtils> m_utils;
    CComPtr<IDxcCompiler3> m_compiler;

public:
    DXCompiler() = delete;
    DXCompiler(const DXCompiler &) = delete;
    DXCompiler(DXCompiler &&) = delete;

    DXCompiler(LPCSTR dllPath);
    ~DXCompiler();

    bool IsLoaded() { return m_dll != nullptr && m_createFn != nullptr; }

    CComPtr<IDxcUtils> Utils() const noexcept { return m_utils; }
    CComPtr<IDxcCompiler3> Compiler() const noexcept { return m_compiler; }

    CComPtr<IDxcBlobUtf8> Disassemble(LPCVOID pData, UINT32 size);

    template <typename T>
    HRESULT CreateInstance(REFCLSID clsid, T **pResult) {
        static_assert(std::is_base_of<IUnknown, T>::value, "Type must inherit from IUnknown");

        if (!IsLoaded())        { return E_FAIL; }
        if (pResult == nullptr) { return E_POINTER; }

        return m_createFn(clsid, __uuidof(T), (LPVOID*)((IUnknown **)pResult));
    }
};
