#pragma once

// Represents the value to be stored in CTLS.
class CTlsValue
{
public:
	virtual ~CTlsValue();
protected:
	CTlsValue();
};

// For general value that does not need special handling, such as int or object that are copyable.
template<class T>
class CGeneralTlsValue : public CTlsValue
{
public:
	CGeneralTlsValue(const T& val) : m_val(val)
	{
	}
public:
	const T& GetValue() const
	{
		return m_val;
	}
private:
	T m_val;
};

// For value that need to free by "delete" operator.
template<typename T>
class CAutoPtrTlsValue : public CTlsValue
{
public:
	CAutoPtrTlsValue(T *ptr) : m_ptr(ptr)
	{
	}

	virtual ~CAutoPtrTlsValue()
	{
		delete m_ptr;
	}

	T * GetValue() const
	{
		return m_ptr;
	}
private:
	T *m_ptr;
};

// For COM object. It will automatic release reference on the value.
template<class T>
class CInterfaceTlsValue : public CTlsValue
{
public:
	CInterfaceTlsValue(const CComPtr<T>& ptr) : m_ptr(ptr)
	{
	}
public:
	const CComPtr<T>& GetValue() const
	{
		return m_ptr;
	}
private:
	CComPtr<T> m_ptr;
};
