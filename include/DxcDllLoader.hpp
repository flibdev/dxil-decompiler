#pragma once

#include "Wincludes.h"
#include "dxcapi.h"

#include <string>

// Slopped together from bits of DxCompilerDllLoader
class DxcDllLoader {

    HMODULE m_dll = nullptr;
    DxcCreateInstanceProc m_createFn = nullptr;

public:
    DxcDllLoader() = default;
    DxcDllLoader(const DxcDllLoader &) = delete;
    DxcDllLoader(DxcDllLoader &&) = delete;

    HRESULT Initialize(LPCSTR dllPath) {
        if (IsLoaded()) { return S_OK; }

        m_dll = LoadLibraryA(dllPath);
        if (m_dll == nullptr) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        m_createFn = (DxcCreateInstanceProc)GetProcAddress(m_dll, "DxcCreateInstance");
        if (m_createFn == nullptr) {
            HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
            FreeLibrary(m_dll);
            m_dll = nullptr;
            return hr;
        }

        return S_OK;
    }

    bool IsLoaded() { return m_dll != nullptr && m_createFn != nullptr; } 

    template <typename T>
    HRESULT CreateInstance(REFCLSID clsid, T **pResult) {
        static_assert(std::is_base_of<IUnknown, T>::value, "Type must inherit from IUnknown");

        if (!IsLoaded())        { return E_FAIL; }
        if (pResult == nullptr) { return E_POINTER; }

        return m_createFn(clsid, __uuidof(T), (LPVOID*)((IUnknown **)pResult));
    }
};
