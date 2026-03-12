#include "API.h"
#include "Error.hpp"
#include "DXCompiler.hpp"
#include "Wincludes.h"

#include <iostream>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <memory>

#include <stdio.h>

#include <dxc/dxcapi.h>
#include <dxil_spirv_c.h>


typedef struct dxd_api_handle {
	DXCompiler *dxcompiler = nullptr;

	bool initialized = false;

	dxd_api_handle() {
		initialized = true;
	}
	~dxd_api_handle() {
		if (dxcompiler != nullptr) {
			delete dxcompiler;
			dxcompiler = nullptr;
		}
	}
} dxd_api_handle;

// Helper method to write a char buffer to file while ensuring the full path exists
void WriteToFile(const char *filename, const char *data, size_t size) {
	try {
		// Ensure full path to parent exists
		auto path = std::filesystem::path(filename);
		std::filesystem::create_directories(path.parent_path());

		std::ofstream outFile(path);
		outFile.write(data, size);
		outFile.close();
	}
	catch (...) {
		throw ErrorCodes::DXD_CannotWrite;
	}
}


inline dxd_api_handle* ConvertDXDHandle(dxd_handle handle) {
	auto instance = reinterpret_cast<dxd_api_handle*>(handle);
	if (!instance->initialized) {
		throw ErrorCodes::DXD_InvalidHandle;
	}
	return instance;
}



DXIL_DECOMPILER_API void dxd_get_error_string(dxd_error id, char* buffer, size_t size) {
	if (size >= DXD_MIN_ERROR_BUFFER_SIZE) {
		snprintf(buffer, size, "%s", GetErrorString((ErrorCodes)id).c_str());
	}
}

DXD_API dxd_handle dxd_api_create()
{
	auto handle = new dxd_api_handle();
	return reinterpret_cast<dxd_handle>(handle);
}


DXD_API dxd_error dxd_api_destroy(dxd_handle handle)
{
	try {
		auto instance = ConvertDXDHandle(handle);

		delete instance;
		instance = nullptr;

		return ErrorCodes::Success;
	}
	catch (ErrorCodes &e) {
		return e;
	}
	catch (...) {
		return ErrorCodes::UnhandledException;
	}
}

DXD_API dxd_error dxd_dxc_initialize(dxd_handle handle, const char* dllPath)
{
	try {
		auto instance = ConvertDXDHandle(handle);

		instance->dxcompiler = new DXCompiler(dllPath);

		return ErrorCodes::Success;
	}
	catch (ErrorCodes &e) {
		return e;
	}
	catch (...) {
		return ErrorCodes::UnhandledException;
	}
}

DXD_API dxd_error dxd_dxc_export_disassembled(dxd_handle handle, const void* data, size_t size, const char* filename)
{
	try {
		auto instance = ConvertDXDHandle(handle);

		auto result = instance->dxcompiler->Disassemble(data, size);

		WriteToFile(filename, result->GetStringPointer(), result->GetStringLength());

		return ErrorCodes::Success;
	}
	catch (ErrorCodes &e) {
		return e;
	}
	catch (...) {
		return ErrorCodes::UnhandledException;
	}
}


/*
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
*/
