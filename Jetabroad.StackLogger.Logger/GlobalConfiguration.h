#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// Value container for global configurations.
class CGlobalConfigurationValues
{
public:
	CGlobalConfigurationValues();
	virtual ~CGlobalConfigurationValues();

	virtual VOID CopyTo(CGlobalConfigurationValues& dest) const;
	virtual VOID CopyFrom(CGlobalConfigurationValues& src);

	virtual BOOL GetEnableExceptionDumper() const;
	virtual VOID SetEnableExceptionDumper(BOOL blEnable);
	virtual BOOL GetLogExceptionDumping() const;
	virtual VOID SetLogExceptionDumping(BOOL blEnable);
	virtual UINT GetMaxStackDepth() const;
	virtual VOID SetMaxStackDepth(UINT uDepth);
private:
	BOOL m_blEnableExceptionDumper;
	BOOL m_blLogExceptionDumping;
	UINT m_uMaxStackDepth;
};

// Global configurations. We cannot make this class to be COM class since it
// will make DllCanUnloadNow() alway return S_FALSE.
class CGlobalConfiguration : public CGlobalConfigurationValues
{
	typedef CComCritSecLock<CComAutoCriticalSection> CObjectLock;
public:
	CGlobalConfiguration();
	CGlobalConfiguration(const CGlobalConfiguration&) = delete;
	virtual ~CGlobalConfiguration();

	CGlobalConfiguration& operator=(const CGlobalConfiguration&) = delete;

	virtual VOID CopyTo(CGlobalConfigurationValues& dest) const;
	virtual VOID CopyFrom(CGlobalConfigurationValues& src);

	virtual BOOL GetEnableExceptionDumper() const;
	virtual VOID SetEnableExceptionDumper(BOOL blEnable);
	virtual BOOL GetLogExceptionDumping() const;
	virtual VOID SetLogExceptionDumping(BOOL blEnable);
	virtual UINT GetMaxStackDepth() const;
	virtual VOID SetMaxStackDepth(UINT uDepth);
private:
	mutable CComAutoCriticalSection m_csObject;
};

// Interface for managed world to control global configurations.
// It expose directly via IGlobalConfiguration.
class ATL_NO_VTABLE GlobalConfiguration :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<GlobalConfiguration, &CLSID_GlobalConfiguration>,
	public CGlobalConfigurationValues,
	public IGlobalConfiguration
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(GlobalConfiguration)
		COM_INTERFACE_ENTRY(IGlobalConfiguration)
	END_COM_MAP()
public:
	GlobalConfiguration(const GlobalConfiguration&) = delete;
	GlobalConfiguration& operator=(const GlobalConfiguration&) = delete;

	virtual HRESULT STDMETHODCALLTYPE get_EnabledExceptionDumper(VARIANT_BOOL *enabled);
	virtual HRESULT STDMETHODCALLTYPE put_EnabledExceptionDumper(VARIANT_BOOL enabled);
	virtual HRESULT STDMETHODCALLTYPE get_LogExceptionDumping(VARIANT_BOOL *enabled);
	virtual HRESULT STDMETHODCALLTYPE put_LogExceptionDumping(VARIANT_BOOL enabled);
	virtual HRESULT STDMETHODCALLTYPE get_MaximumStackDepth(long *depth);
	virtual HRESULT STDMETHODCALLTYPE put_MaximumStackDepth(long depth);
	virtual HRESULT STDMETHODCALLTYPE Apply();
protected:
	GlobalConfiguration();
	virtual ~GlobalConfiguration();
};

extern CGlobalConfiguration *g_pConfig;
