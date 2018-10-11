// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// OperationLog contain logging message for exception dumping operation.
// It expose directly to managed world via IExceptionDumpingMessage.
class ATL_NO_VTABLE OperationLog :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<OperationLog>,
	public IOperationLog
{
public:
	BEGIN_COM_MAP(OperationLog)
		COM_INTERFACE_ENTRY(IOperationLog)
	END_COM_MAP()
public:
	OperationLog(const OperationLog&) = delete;
	OperationLog& operator=(const OperationLog&) = delete;

	VOID SetTime(DATE time);
	VOID SetMessage(PCWSTR pszMessage);
	
	HRESULT STDMETHODCALLTYPE get_Time(DATE *time) override;
	HRESULT STDMETHODCALLTYPE get_Message(BSTR *msg) override;
protected:
	OperationLog();
private:
	COleDateTime time;
	CComBSTR msg;
};
