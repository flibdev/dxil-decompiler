#include "DXCompiler.hpp"

DXCompiler::DXCompiler(LPCSTR dllPath) {
    try {
        m_dll = LoadLibraryA(dllPath);
        if (m_dll == nullptr) { throw ErrorCodes::DXC_LoadDLL; }

        m_createFn = (DxcCreateInstanceProc)GetProcAddress(m_dll, "DxcCreateInstance");
        if (m_createFn == nullptr) { throw ErrorCodes::DXC_LoadDLL; }

        HRESULT hr = CreateInstance(CLSID_DxcUtils, &m_utils);
        if (hr < 0) { throw ErrorCodes::DXC_CreateUtils; }

        hr = CreateInstance(CLSID_DxcCompiler, &m_compiler);
        if (hr < 0) { throw ErrorCodes::DXC_CreateCompiler; }
    }
    catch (ErrorCodes &e) {
        if (m_dll != nullptr) {
            FreeLibrary(m_dll);
        }
        m_dll = nullptr;
        m_createFn = nullptr;

        throw e;
    }
}

DXCompiler::~DXCompiler() {
    if (m_dll != nullptr) {
        FreeLibrary(m_dll);
    }
    m_dll = nullptr;
    m_createFn = nullptr;
}

CComPtr<IDxcBlobUtf8> DXCompiler::Disassemble(LPCVOID pData, UINT32 size) {
    CComPtr<IDxcBlobEncoding> compiledBlob;
    CComPtr<IDxcResult> result;
    CComPtr<IDxcBlobUtf8> outputBlob;
    
    // Create a blob of the compiled bitcode
    COM_IFT(
        m_utils->CreateBlob(pData, size, NULL, &compiledBlob),
        ErrorCodes::DXC_CreateBlob
    );

    // Wrap it in a DxcBuffer because the compiler doesn't accept blobs directly
    // (which is very dumb)
    DxcBuffer compiledBuffer = {
        compiledBlob->GetBufferPointer(),
        compiledBlob->GetBufferSize(),
        0
    };

    COM_IFT(
        m_compiler->Disassemble(&compiledBuffer, COM_IID_ARGS(result)),
        ErrorCodes::DXC_DisassembleError
    );

    if (!result->HasOutput(DXC_OUT_KIND::DXC_OUT_DISASSEMBLY)) {
        throw ErrorCodes::DXC_DisassembleEmpty;
    }

    COM_IFT(
        result->GetOutput(DXC_OUT_KIND::DXC_OUT_DISASSEMBLY, COM_IID_ARGS(outputBlob), nullptr),
        ErrorCodes::DXC_DisassembleEmpty
    );

    return outputBlob;
}
