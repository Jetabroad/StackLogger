// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

class ATL_NO_VTABLE CLogger
{
public:
	virtual VOID WriteLog(LPCWSTR pszFormat, ...) = 0;
protected:
	CLogger();
	virtual ~CLogger();
};
