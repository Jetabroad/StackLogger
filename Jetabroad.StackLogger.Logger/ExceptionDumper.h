// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Logger.h"

#include "Jetabroad.StackLogger.Logger_h.h"

class exception_dumper final
{
public: // Nested Types.
	struct config
	{
		unsigned max_frames;

		config() : max_frames(3) {}
	};
public:
	exception_dumper(const config& conf, const CComPtr<IXCLRDataProcess>& dac, DWORD tid, CLogger& logger);
	exception_dumper(const exception_dumper&) = delete;

	exception_dumper& operator=(const exception_dumper&) = delete;

	CComPtr<IStackData> run();
private: // Helpers.
	CComPtr<IUnknown> dump_array(IXCLRDataValue *array);
	CComPtr<IFrameData> dump_frame(IXCLRDataFrame *frame);
	CComPtr<IObjectDescriptor2> dump_method_argument(IXCLRDataFrame *frame, ULONG32 aidx);
	CComPtr<IUnknown> dump_object(IXCLRDataValue *obj);
	CComPtr<IUnknown> dump_object_raw(IXCLRDataValue *obj);
	CComPtr<IUnknown> dump_string(IXCLRDataValue *str);
private: // Private Objects.
	config conf;
	CComPtr<IXCLRDataProcess> dac;
	DWORD tid;
	CLogger& logger;
};
