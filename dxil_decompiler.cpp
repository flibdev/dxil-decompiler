// DXIL_Decompiler.cpp : Defines the entry point for the application.
//

#include "dxil_decompiler.h"

#include <stdio.h>


#define WRITE_ERR(s) snprintf(buffer, size-1, s);

DXIL_DECOMPILER_API void dxd_get_error_string(error_id id, char* buffer, size_t size) {
	switch (id)
	{

	case 0:
	default:
		WRITE_ERR("No error.");
		break;
	}
}

