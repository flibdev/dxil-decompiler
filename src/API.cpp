#include "API.h"
#include "Wincludes.h"
#include "DxcDllLoader.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <stdio.h>

#include "dxc/dxcapi.h"
#include "../deps/dxil-spirv/dxil_spirv_c.h"


enum ErrorCodes
{
	Success = 0,
	UnhandledException,
	DXC_LoadDLL,
	DXC_CreateUtils,
	DXC_CreateCompiler,
	DXC_CreateBlob,
	DXC_DisassembleError,
	DXC_DisassembleEmpty
};

#define WRITE_ERR(...) snprintf(buffer, size-1, __VA_ARGS__);

DXIL_DECOMPILER_API void dxd_get_error_string(dxd_error id, char* buffer, size_t size) {
	switch (id)
	{
		// dxil-spirv error codes are negative
		case DXIL_SPV_ERROR_OUT_OF_MEMORY:
			WRITE_ERR("dxil-spirv: Ran out of memory");
			break;
		case DXIL_SPV_ERROR_GENERIC:
			WRITE_ERR("dxil-spirv: Unhelpful generic error");
			break;
		case DXIL_SPV_ERROR_UNSUPPORTED_FEATURE:
			WRITE_ERR("dxil-spirv: Unsupported feature");
			break;
		case DXIL_SPV_ERROR_PARSER:
			WRITE_ERR("dxil-spirv: Unable to parse blob");
			break;
		case DXIL_SPV_ERROR_FAILED_VALIDATION:
			WRITE_ERR("dxil-spirv: Failed validation");
			break;
		case DXIL_SPV_ERROR_INVALID_ARGUMENT:
			WRITE_ERR("dxil-spirv: Invalid argument");
			break;
		case DXIL_SPV_ERROR_NO_DATA:
			WRITE_ERR("dxil-spirv: No data");
			break;

		case ErrorCodes::DXC_LoadDLL:
			WRITE_ERR("DXC: Cannot load dxcompiler.dll");
			break;
		case ErrorCodes::DXC_CreateUtils:
			WRITE_ERR("DXC: Cannot create instance of IDxcUtils");
			break;
		case ErrorCodes::DXC_CreateCompiler:
			WRITE_ERR("DXC: Cannot create instance of IDxcCompiler3");
			break;
		case ErrorCodes::DXC_CreateBlob:
			WRITE_ERR("DXC: Cannot create IDxcBlob from input data");
			break;
		case ErrorCodes::DXC_DisassembleError:
			WRITE_ERR("DXC: Cannot disassemble input blob");
			break;
		case ErrorCodes::DXC_DisassembleEmpty:
			WRITE_ERR("DXC: Disassembled output is empty");
			break;

		case ErrorCodes::UnhandledException:
			WRITE_ERR("Unhandled exception, yell at flib");
			break;
			
		case ErrorCodes::Success:
			WRITE_ERR("Success");
			break;
		default:
			WRITE_ERR("Invalid error code: %i", id);
			break;
	}
}

#define COM_IID_ARGS(x) __uuidof(*x), reinterpret_cast<void**>(&x)

#define COM_IFT(x, e)                                                          \
{                                                                              \
	HRESULT hr = (x);                                                          \
	if (hr < 0) { return e; }                                                  \
}



DXD_API dxd_error dxd_export_disassembled(const void* data, size_t size, const char* filename)
{
	try
	{
		// Todo: move this into an init function
		DxcDllLoader loader;
		COM_IFT(loader.Initialize("lib/dxcompiler.dll"), ErrorCodes::DXC_LoadDLL);
		
		CComPtr<IDxcUtils> utils;
		COM_IFT(loader.CreateInstance(CLSID_DxcUtils, &utils), ErrorCodes::DXC_CreateUtils);

		CComPtr<IDxcCompiler3> compiler;
		COM_IFT(loader.CreateInstance(CLSID_DxcCompiler, &compiler), ErrorCodes::DXC_CreateCompiler);

		CComPtr<IDxcBlobEncoding> compiledBlob;
		COM_IFT(utils->CreateBlob(data, size, NULL, &compiledBlob), ErrorCodes::DXC_CreateBlob);

		DxcBuffer compiledBuffer = {
			compiledBlob->GetBufferPointer(),
			compiledBlob->GetBufferSize(),
			0
		};

		CComPtr<IDxcResult> result;
		COM_IFT(
			compiler->Disassemble(&compiledBuffer, COM_IID_ARGS(result)),
			ErrorCodes::DXC_DisassembleError
		);

		if (!result->HasOutput(DXC_OUT_KIND::DXC_OUT_DISASSEMBLY)) {
			return ErrorCodes::DXC_DisassembleEmpty;
		}

		CComPtr<IDxcBlobUtf8> outputBlob;
		CComPtr<IDxcBlobWide> outputName;
		COM_IFT(
			result->GetOutput(DXC_OUT_KIND::DXC_OUT_DISASSEMBLY, COM_IID_ARGS(outputBlob), &outputName),
			ErrorCodes::DXC_DisassembleEmpty
		);

		auto path = std::filesystem::path(filename);
		// Ensure full path exists
		std::filesystem::create_directories(path.parent_path());

		std::ofstream outFile(path);
		outFile.write(outputBlob->GetStringPointer(), outputBlob->GetStringLength());
		outFile.close();

		return ErrorCodes::Success;
	}
	catch(const std::exception &ex)
	{
		return ErrorCodes::UnhandledException;
	}
}



BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

