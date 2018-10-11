// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "DACTargetProvider.h"
//implementation of ICLRDataTarget (we need to implement to access DAC) 
DACTargetProvider::DACTargetProvider() :
	logger(nullptr),
	ex(nullptr)
{
}

DACTargetProvider::~DACTargetProvider()
{
}

VOID DACTargetProvider::SetException(const EXCEPTION_POINTERS *pException)
{
	ex = pException;
}

VOID DACTargetProvider::SetLogger(CLogger *pLogger)
{
	logger = pLogger;
}

HRESULT DACTargetProvider::GetMachineType(ULONG32 *machineType)
{
	logger->WriteLog(L"DAC is asking for machine type.");

	if (!machineType)
		return E_POINTER;

#if defined(_M_IX86)
	*machineType = IMAGE_FILE_MACHINE_I386;
#elif defined(_M_AMD64)
	*machineType = IMAGE_FILE_MACHINE_AMD64;
#else
#error Target machine is not supported.
#endif

	return S_OK;
}

HRESULT DACTargetProvider::GetPointerSize(ULONG32 *pointerSize)
{
	logger->WriteLog(L"DAC is asking for the size of pointer.");

	if (!pointerSize)
		return E_POINTER;

	*pointerSize = sizeof(PVOID);

	return S_OK;
}

HRESULT DACTargetProvider::GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS *baseAddress)
{
	logger->WriteLog(L"DAC is asking for image base of the module %s.", imagePath);

	if (!imagePath || !baseAddress)
		return E_POINTER;

	auto mod = GetModuleHandle(imagePath);
	if (!mod)
	{
		auto hr = AtlHresultFromLastError();
		logger->WriteLog(L"Failed to get image base for module %s: 0x%X.", imagePath, hr);
		return hr;
	}

	*baseAddress = reinterpret_cast<CLRDATA_ADDRESS>(mod);

	return S_OK;
}

HRESULT DACTargetProvider::ReadVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesRead)
{
	// Don't logging this method since it will be a lot of entries.
	if (!buffer || !bytesRead)
		return E_POINTER;

	__try
	{
		memcpy(buffer, reinterpret_cast<const void *>(address), bytesRequested);
		*bytesRead = bytesRequested;
	}
	__except (GetExceptionCode() == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return E_ACCESSDENIED;
	}

	return S_OK;
}

HRESULT DACTargetProvider::WriteVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesWritten)
{
	logger->WriteLog(L"DAC is asking to write %u bytes at 0x%IX.", bytesRequested, address);

	if (!buffer || !bytesWritten)
		return E_POINTER;

	__try
	{
		// We need to write bytesWritten before do the actual write since after we do the actual write, writing to bytesWritten is not allow to fail.
		*bytesWritten = bytesRequested;
		memcpy(reinterpret_cast<void *>(address), buffer, bytesRequested);
	}
	__except (GetExceptionCode() == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return E_ACCESSDENIED;
	}

	return S_OK;
}

HRESULT DACTargetProvider::GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS * /* value */)
{
	logger->WriteLog(L"DAC is asking for TLS value from index %u for thread %u.", index, threadID);
	return E_NOTIMPL;
}

HRESULT DACTargetProvider::SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS /* value */)
{
	logger->WriteLog(L"DAC is asking to set TLS value to index %u for thread %u.", index, threadID);
	return E_NOTIMPL;
}

HRESULT DACTargetProvider::GetCurrentThreadID(ULONG32 * /* threadID */)
{
	logger->WriteLog(L"DAC is asking for current thread's identifier.");
	return E_NOTIMPL;
}

HRESULT DACTargetProvider::GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE *context)
{
	logger->WriteLog(L"DAC is asking for context of the thread %u.", threadID);

	if (contextSize != sizeof(CONTEXT))
		return E_INVALIDARG;

	auto pctx = reinterpret_cast<PCONTEXT>(context);

	memcpy(pctx, ex->ContextRecord, contextSize);
	pctx->ContextFlags = contextFlags;

	return S_OK;
}

HRESULT DACTargetProvider::SetThreadContext(ULONG32 threadID, ULONG32 /* contextSize */, BYTE * /* context */)
{
	logger->WriteLog(L"DAC is asking for setting context for thread %u.", threadID);
	return E_NOTIMPL;
}

HRESULT DACTargetProvider::Request(ULONG32 reqCode, ULONG32 /* inBufferSize */, BYTE * /* inBuffer */, ULONG32 /* outBufferSize */, BYTE * /* outBuffer */)
{
	logger->WriteLog(L"DAC is asking for a service number %u.", reqCode);
	return E_NOTIMPL;
}

HRESULT DACTargetProvider::AllocVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags, ULONG32 protectFlags, CLRDATA_ADDRESS *virt)
{
	logger->WriteLog(L"DAC is asking to allocate %u bytes starting at 0x%IX.", size, addr);

	if (!virt)
		return E_POINTER;

	auto mem = VirtualAlloc(reinterpret_cast<LPVOID>(addr), size, typeFlags, protectFlags);
	if (!mem)
	{
		auto hr = AtlHresultFromLastError();
		logger->WriteLog(L"Failed to allocate %u bytes on virtual memory starting at 0x%IX", size, addr);
		return hr;
	}

	*virt = reinterpret_cast<CLRDATA_ADDRESS>(mem);

	return S_OK;
}

HRESULT DACTargetProvider::FreeVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags)
{
	logger->WriteLog(L"DAC is asking to free %u bytes at 0x%IX.", size, addr);

#pragma warning(push)
#pragma warning(disable:28160) // Passing MEM_RELEASE and a non - zero dwSize parameter to VirtualFree is not allowed.
	// We need to ignore this warning since we need to passthrough original behavior back to CLR.
	if (!VirtualFree(reinterpret_cast<LPVOID>(addr), size, typeFlags))
		return AtlHresultFromLastError();
#pragma warning(pop)

	return S_OK;
}
