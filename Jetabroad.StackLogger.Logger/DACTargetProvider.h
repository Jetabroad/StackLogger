// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Logger.h"

// This class used to provide information that will be ask by DAC.
class ATL_NO_VTABLE DACTargetProvider :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<DACTargetProvider>,
	public ICLRDataTarget2
{
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(DACTargetProvider)
		COM_INTERFACE_ENTRY(ICLRDataTarget)
		COM_INTERFACE_ENTRY(ICLRDataTarget2)
	END_COM_MAP()
public:
	DACTargetProvider(const DACTargetProvider&) = delete;
	DACTargetProvider& operator=(const DACTargetProvider&) = delete;

	VOID SetException(const EXCEPTION_POINTERS *pException);
	VOID SetLogger(CLogger *pLogger);

	HRESULT STDMETHODCALLTYPE GetMachineType(ULONG32 *machineType) override;
	HRESULT STDMETHODCALLTYPE GetPointerSize(ULONG32 *pointerSize) override;
	HRESULT STDMETHODCALLTYPE GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS *baseAddress) override;
	HRESULT STDMETHODCALLTYPE ReadVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesRead) override;
	HRESULT STDMETHODCALLTYPE WriteVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesWritten) override;
	HRESULT STDMETHODCALLTYPE GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS *value) override;
	HRESULT STDMETHODCALLTYPE SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS value) override;
	HRESULT STDMETHODCALLTYPE GetCurrentThreadID(ULONG32 *threadID) override;
	HRESULT STDMETHODCALLTYPE GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE *context) override;
	HRESULT STDMETHODCALLTYPE SetThreadContext(ULONG32 threadID, ULONG32 contextSize, BYTE *context) override;
	HRESULT STDMETHODCALLTYPE Request(ULONG32 reqCode, ULONG32 inBufferSize, BYTE *inBuffer, ULONG32 outBufferSize, BYTE *outBuffer) override;
	HRESULT STDMETHODCALLTYPE AllocVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags, ULONG32 protectFlags, CLRDATA_ADDRESS *virt) override;
	HRESULT STDMETHODCALLTYPE FreeVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags) override;
protected:
	DACTargetProvider();
	~DACTargetProvider();
private:
	CLogger *logger;
	const EXCEPTION_POINTERS *ex;
};
