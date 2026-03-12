#pragma once

#include <exception>
#include <format>
#include <string>

#include "../deps/dxil-spirv/dxil_spirv_c.h"

enum ErrorCodes
{
    Success = 0,
    UnhandledException,
    
    DXD_InvalidHandle,
    DXD_CannotWrite,

    DXC_NotInitialized,
    DXC_LoadDLL,
    DXC_CreateUtils,
    DXC_CreateCompiler,
    DXC_CreateBlob,
    DXC_DisassembleError,
    DXC_DisassembleEmpty,
    
    DXIL_SPV_OutOfMemory        = dxil_spv_result::DXIL_SPV_ERROR_OUT_OF_MEMORY,
    DXIL_SPV_Generic            = dxil_spv_result::DXIL_SPV_ERROR_GENERIC,
    DXIL_SPV_UnsupportedFeature = dxil_spv_result::DXIL_SPV_ERROR_UNSUPPORTED_FEATURE,
    DXIL_SPV_BlobParser         = dxil_spv_result::DXIL_SPV_ERROR_PARSER,
    DXIL_SPV_FailedValidation   = dxil_spv_result::DXIL_SPV_ERROR_FAILED_VALIDATION,
    DXIL_SPV_InvalidArgument    = dxil_spv_result::DXIL_SPV_ERROR_INVALID_ARGUMENT,
    DXIL_SPV_NoData             = dxil_spv_result::DXIL_SPV_ERROR_NO_DATA,
};

constexpr std::string GetErrorString(ErrorCodes code) noexcept {
    switch (code)
    {
        case ErrorCodes::Success:
            return "Success";
        case ErrorCodes::UnhandledException:
            return "Unhandled Exception";

        case ErrorCodes::DXD_InvalidHandle:
            return "DXD: Passed invalid handle";
        case ErrorCodes::DXD_CannotWrite:
            return "DXD: Cannot write to file";

        case ErrorCodes::DXC_NotInitialized:
            return "DXC: Not initialized";
        case ErrorCodes::DXC_LoadDLL:
            return "DXC: Cannot load dxcompiler.dll";
        case ErrorCodes::DXC_CreateUtils:
            return "DXC: Cannot create instance of IDxcUtils";
        case ErrorCodes::DXC_CreateCompiler:
            return "DXC: Cannot create instance of IDxcCompiler3";
        case ErrorCodes::DXC_CreateBlob:
            return "DXC: Cannot create IDxcBlob from input data";
        case ErrorCodes::DXC_DisassembleError:
            return "DXC: Cannot disassemble input blob";
        case ErrorCodes::DXC_DisassembleEmpty:
            return "DXC: Disassembled output is empty";

        case ErrorCodes::DXIL_SPV_OutOfMemory:
            return "dxil-spirv: Ran out of memory";
        case ErrorCodes::DXIL_SPV_Generic:
            return "dxil-spirv: Unhelpful generic error";
        case ErrorCodes::DXIL_SPV_UnsupportedFeature:
            return "dxil-spirv: Unsupported feature";
        case ErrorCodes::DXIL_SPV_BlobParser:
            return "dxil-spirv: Unable to parse blob";
        case ErrorCodes::DXIL_SPV_FailedValidation:
            return "dxil-spirv: Failed validation";
        case ErrorCodes::DXIL_SPV_InvalidArgument:
            return "dxil-spirv: Invalid argument";
        case ErrorCodes::DXIL_SPV_NoData:
            return "dxil-spirv: No data";
    }
    // Outside of `default:` so the compiler yells at me for missing cases
    return std::format("Invalid error code: {}", (int)code);
}
