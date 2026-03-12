#pragma once

#ifndef NOMINMAX
  #define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <ObjIdl.h>
#include <atlbase.h> // atlbase.h needs to come before strsafe.h
#include <intsafe.h>
#include <strsafe.h>
#include <unknwn.h>
#include <windows.h>


// COM Helper macros

// Outputs the common (type uuid, void pointer) args pair
#define COM_IID_ARGS(x) __uuidof(*x), reinterpret_cast<void**>(&x)

// Do x, if it returns a negative HRESULT, throw e
#define COM_IFT(x, e)                                                          \
{                                                                              \
  HRESULT hr = (x);                                                            \
  if (hr < 0) { throw e; }                                                     \
}