/* DXIL Decompiler
 * Copyright 2025 Reece Sheppard (flibdev)
 * 
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef DXIL_DECOMPILER_API
#define DXIL_DECOMPILER_API

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__GNUC__)
#define DXD_API __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define DXD_API __declspec(dllexport)
#else
#define DXD_API
#endif

// API Functions return this, 0 means success, and other value is an error
typedef uint32_t dxd_error;

// Gets the error reason for a given id
// Minimum required buffer size is 40 bytes
DXD_API void dxd_get_error_string(dxd_error id, char* buffer, size_t size);


// Disassemble DXIL bitcode to its LLVM-based IR
// (Horrible to read but 1:1 with compiled bitcode)
DXD_API dxd_error dxd_export_disassembled(const void* data, size_t size, const char* filename);


/*typedef struct dxd_api* dxd_handle;


DXD_API dxd_handle dxd_api_create();
DXD_API dxd_error dxd_api_destroy(dxd_handle instance);
*/

#ifdef __cplusplus
}
#endif

#endif
