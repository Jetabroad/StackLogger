#pragma once

// This class used to provide information that will be ask by CLR about our process.
class ClrDataTarget :
	public CComObjectRootEx<CComMultiThreadModel>,
	public ICLRDataTarget2
{
public:
	BEGIN_COM_MAP(ClrDataTarget)
		COM_INTERFACE_ENTRY(ICLRDataTarget)
		COM_INTERFACE_ENTRY(ICLRDataTarget2)
	END_COM_MAP()
public:
	virtual HRESULT STDMETHODCALLTYPE GetMachineType(ULONG32 *machineType);
	virtual HRESULT STDMETHODCALLTYPE GetPointerSize(ULONG32 *pointerSize);
	virtual HRESULT STDMETHODCALLTYPE GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS *baseAddress);
	virtual HRESULT STDMETHODCALLTYPE ReadVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesRead);
	virtual HRESULT STDMETHODCALLTYPE WriteVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesWritten);
	virtual HRESULT STDMETHODCALLTYPE GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS *value);
	virtual HRESULT STDMETHODCALLTYPE SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS value);
	virtual HRESULT STDMETHODCALLTYPE GetCurrentThreadID(ULONG32 *threadID);
	virtual HRESULT STDMETHODCALLTYPE GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE *context);
	virtual HRESULT STDMETHODCALLTYPE SetThreadContext(ULONG32 threadID, ULONG32 contextSize, BYTE *context);
	virtual HRESULT STDMETHODCALLTYPE Request(ULONG32 reqCode, ULONG32 inBufferSize, BYTE *inBuffer, ULONG32 outBufferSize, BYTE *outBuffer);
	virtual HRESULT STDMETHODCALLTYPE AllocVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags, ULONG32 protectFlags, CLRDATA_ADDRESS *virt);
	virtual HRESULT STDMETHODCALLTYPE FreeVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags);
protected:
	ClrDataTarget();
	virtual ~ClrDataTarget();
};
