#include "PCH.h"
#include "GlobalConfiguration.h"

CGlobalConfiguration *g_pConfig;

////////////////////////////////////////////////////////////////////////////////
// CGlobalConfigurationValues

CGlobalConfigurationValues::CGlobalConfigurationValues() :
	m_blEnableExceptionDumper(TRUE),
	m_blLogExceptionDumping(FALSE),
	m_uMaxStackDepth(3)
{
}

CGlobalConfigurationValues::~CGlobalConfigurationValues()
{
}

VOID CGlobalConfigurationValues::CopyTo(CGlobalConfigurationValues& dest) const
{
	dest.m_blEnableExceptionDumper = m_blEnableExceptionDumper;
	dest.m_blLogExceptionDumping = m_blLogExceptionDumping;
	dest.m_uMaxStackDepth = m_uMaxStackDepth;
}

VOID CGlobalConfigurationValues::CopyFrom(CGlobalConfigurationValues& src)
{
	m_blEnableExceptionDumper = src.m_blEnableExceptionDumper;
	m_blLogExceptionDumping = src.m_blLogExceptionDumping;
	m_uMaxStackDepth = src.m_uMaxStackDepth;
}

BOOL CGlobalConfigurationValues::GetEnableExceptionDumper() const
{
	return m_blEnableExceptionDumper;
}

VOID CGlobalConfigurationValues::SetEnableExceptionDumper(BOOL blEnable)
{
	m_blEnableExceptionDumper = blEnable;
}

BOOL CGlobalConfigurationValues::GetLogExceptionDumping() const
{
	return m_blLogExceptionDumping;
}

VOID CGlobalConfigurationValues::SetLogExceptionDumping(BOOL blEnable)
{
	m_blLogExceptionDumping = blEnable;
}

UINT CGlobalConfigurationValues::GetMaxStackDepth() const
{
	return m_uMaxStackDepth;
}

VOID CGlobalConfigurationValues::SetMaxStackDepth(UINT uDepth)
{
	m_uMaxStackDepth = uDepth;
}

////////////////////////////////////////////////////////////////////////////////
// CGlobalConfiguration

CGlobalConfiguration::CGlobalConfiguration()
{
}

CGlobalConfiguration::~CGlobalConfiguration()
{
}

VOID CGlobalConfiguration::CopyTo(CGlobalConfigurationValues& dest) const
{
	CObjectLock locker(m_csObject);
	CGlobalConfigurationValues::CopyTo(dest);
}

VOID CGlobalConfiguration::CopyFrom(CGlobalConfigurationValues& src)
{
	CObjectLock locker(m_csObject);
	CGlobalConfigurationValues::CopyFrom(src);
}

BOOL CGlobalConfiguration::GetEnableExceptionDumper() const
{
	CObjectLock locker(m_csObject);
	return CGlobalConfigurationValues::GetEnableExceptionDumper();
}

VOID CGlobalConfiguration::SetEnableExceptionDumper(BOOL blEnable)
{
	CObjectLock locker(m_csObject);
	CGlobalConfigurationValues::SetEnableExceptionDumper(blEnable);
}

BOOL CGlobalConfiguration::GetLogExceptionDumping() const
{
	CObjectLock locker(m_csObject);
	return CGlobalConfigurationValues::GetLogExceptionDumping();
}

VOID CGlobalConfiguration::SetLogExceptionDumping(BOOL blEnable)
{
	CObjectLock locker(m_csObject);
	CGlobalConfigurationValues::SetLogExceptionDumping(blEnable);
}

UINT CGlobalConfiguration::GetMaxStackDepth() const
{
	CObjectLock locker(m_csObject);
	return CGlobalConfigurationValues::GetMaxStackDepth();
}

VOID CGlobalConfiguration::SetMaxStackDepth(UINT uDepth)
{
	CObjectLock locker(m_csObject);
	return CGlobalConfigurationValues::SetMaxStackDepth(uDepth);
}

////////////////////////////////////////////////////////////////////////////////
// GlobalConfiguration

GlobalConfiguration::GlobalConfiguration()
{
	g_pConfig->CopyTo(*this);
}

GlobalConfiguration::~GlobalConfiguration()
{
}

HRESULT GlobalConfiguration::get_EnabledExceptionDumper(VARIANT_BOOL *enabled)
{
	if (!enabled)
		return E_POINTER;

	ObjectLock locker(this);
	*enabled = GetEnableExceptionDumper() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT GlobalConfiguration::put_EnabledExceptionDumper(VARIANT_BOOL enabled)
{
	ObjectLock locker(this);
	SetEnableExceptionDumper(enabled == VARIANT_TRUE ? TRUE : FALSE);
	return S_OK;
}

HRESULT GlobalConfiguration::get_LogExceptionDumping(VARIANT_BOOL *enabled)
{
	if (!enabled)
		return E_POINTER;

	ObjectLock locker(this);
	*enabled = GetLogExceptionDumping() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT GlobalConfiguration::put_LogExceptionDumping(VARIANT_BOOL enabled)
{
	ObjectLock locker(this);
	SetLogExceptionDumping(enabled == VARIANT_TRUE ? TRUE : FALSE);
	return S_OK;
}

HRESULT GlobalConfiguration::get_MaximumStackDepth(long *depth)
{
	if (!depth)
		return E_POINTER;

	ObjectLock locker(this);
	*depth = GetMaxStackDepth();

	return S_OK;
}

HRESULT GlobalConfiguration::put_MaximumStackDepth(long depth)
{
	if (depth < 0)
		return E_INVALIDARG;

	ObjectLock locker(this);
	SetMaxStackDepth(depth);

	return S_OK;
}

HRESULT GlobalConfiguration::Apply()
{
	ObjectLock locker(this);
	g_pConfig->CopyFrom(*this);
	return S_OK;
}
