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
#include <spirv-tools/libspirv.hpp>

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

void WriteStringToFile(const char *filename, std::string str) {
	WriteToFile(filename, str.data(), str.size());
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
		delete handle;
		handle = nullptr;

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

#define SPV_IFT(x)                                                             \
{                                                                              \
  dxil_spv_result res = (x);                                                   \
  if (res != DXIL_SPV_SUCCESS) { throw (ErrorCodes)res; }                      \
}


DXD_API dxd_error dxd_spv_export_spirv(dxd_handle handle, const void* data, size_t size, const char* filename)
{
	try {
		auto instance = ConvertDXDHandle(handle);

		dxil_spv_parsed_blob blob;
		dxil_spv_parsed_blob reflection_blob;
		dxil_spv_converter converter;
		dxil_spv_compiled_spirv compiled;

		SPV_IFT(dxil_spv_parse_dxil_blob(data, size, &blob));
		//SPV_IFT(dxil_spv_parse_reflection_dxil_blob(data, size, &reflection_blob));
		SPV_IFT(dxil_spv_create_converter(blob, &converter));
		SPV_IFT(dxil_spv_converter_run(converter));
		SPV_IFT(dxil_spv_converter_get_compiled_spirv(converter, &compiled));

		std::stringstream stream;

		unsigned heuristic_min_wave_size = 0;
		unsigned heuristic_max_wave_size = 0;
		unsigned wave_size_min = 0;
		unsigned wave_size_max = 0;
		unsigned wave_size_preferred = 0;
		dxil_spv_converter_get_compute_wave_size_range(converter, &wave_size_min, &wave_size_max, &wave_size_preferred);
		dxil_spv_converter_get_compute_heuristic_min_wave_size(converter, &heuristic_min_wave_size);
		dxil_spv_converter_get_compute_heuristic_max_wave_size(converter, &heuristic_max_wave_size);

		if (wave_size_min) {
			stream << "// WaveSize(" << wave_size_min;
			if (wave_size_max || wave_size_preferred) {
				stream << ","  << (wave_size_max ? wave_size_max : wave_size_min);
			}
			if (wave_size_preferred) {
				stream << "," << wave_size_preferred;
			}
			stream << std::endl;
		}
		if (heuristic_min_wave_size) {
			stream << "// HeuristicWaveSizeMin(" << heuristic_min_wave_size << ")" << std::endl;
		}
		if (heuristic_max_wave_size) {
			stream << "// HeuristicWaveSize(" << heuristic_max_wave_size << ")" << std::endl;
		}

		//stream << convert_to_asm(compiled.data, compiled.size);
		spvtools::SpirvTools tools(SPV_ENV_VULKAN_1_3);
		std::string disassemStr;
		if (tools.Disassemble(
				static_cast<const uint32_t *>(compiled.data),
				compiled.size / sizeof(uint32_t),
				&disassemStr, 0)) {
			stream << disassemStr;
		}

		//WriteToFile(filename, (const char*)compiled.data, compiled.size);
		WriteStringToFile(filename, stream.str());

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
