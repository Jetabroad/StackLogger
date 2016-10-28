#include "PCH.h"
#include "ExceptionDumper.h"

#include "ArrayValue.h"
#include "FrameData.h"
#include "NullValue.h"
#include "ObjectDescriptor2.h"
#include "RawValue.h"
#include "StackData.h"
#include "StringValue.h"
#include "UnknownValue.h"
//dump exception from CLR
exception_dumper::exception_dumper(const config& conf, const CComPtr<IXCLRDataProcess>& dac, DWORD tid, CLogger& logger) :
	conf(conf),
	dac(dac),
	tid(tid),
	logger(logger)
{
}

CComPtr<IStackData> exception_dumper::run()
{
	HRESULT hr;

	// Create StackData.
	logger.WriteLog(L"Begin dumping exception details.");

	//stackData that we store exception
	auto sdata = create_com<StackData>();

	// Get CLR Task for the thread.
	logger.WriteLog(L"Getting IXCLRDataTask for current thread.");

	CComPtr<IXCLRDataTask> task;
	hr = dac->GetTaskByOSThreadID(tid, &task);
	if (FAILED(hr))
		AtlThrow(hr);

	// Create stack walker to walk on CLR stack.
	logger.WriteLog(L"Creating IXCLRDataStackWalk.");

	CComPtr<IXCLRDataStackWalk> walker;
	hr = task->CreateStackWalk(CLRDATA_SIMPFRAME_MANAGED_METHOD, &walker);
	if (FAILED(hr))
		AtlThrow(hr);

	// Walk CLR stack.
	logger.WriteLog(L"Start iterating stack frame.");

	CInterfaceArray<IFrameData> frames;
	frames.SetCount(0, 32); // This will change growing size.

	unsigned i = 0;
	//read nested exception
	do
	{
		if (i >= conf.max_frames)
		{
			logger.WriteLog(L"Reached maximum number allowed frame.");
			break;
		}

		// Query current frame.
		logger.WriteLog(L"Getting frame %u.", i);

		CComPtr<IXCLRDataFrame> frame;
		hr = walker->GetFrame(&frame);
		if (FAILED(hr))
			AtlThrow(hr);

		// Dump frame.
		logger.WriteLog(L"Start dumping frame %u.", i);

		auto fd = dump_frame(frame);
		frames.Add(fd);

		// Moving to next frame.
		logger.WriteLog(L"Moving to next frame.");

		hr = walker->Next();
		if (FAILED(hr))
			AtlThrow(hr);

		i++;
	} while (hr == S_OK);

	logger.WriteLog(L"Finished stack frames dumping. Total frames: %u.", i);
	sdata->SetFrames(frames);

	return sdata;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers.

CComPtr<IUnknown> exception_dumper::dump_array(IXCLRDataValue *array)
{
	HRESULT hr;

	// Create object to hold dumped array.
	logger.WriteLog(L"Creating ArrayValue instance.");

	auto adata = create_com<ArrayValue>();

	// Query array properties.
	// The maximum dimension for array is 32 (https://msdn.microsoft.com/en-us/library/system.array(v=vs.110).aspx).
	logger.WriteLog(L"Getting array properties.");

	ULONG32 rank, nelems; // rank is number of dimension. nelems is number of total elements.
	ULONG32 bounds[32]; // Number of bound for each dimension.
	LONG32 bases[32]; // Lower bounds of each dimension.

	hr = array->GetArrayProperties(&rank, &nelems, _countof(bounds), bounds, _countof(bases), bases);
	if (FAILED(hr))
		AtlThrow(hr);

	// TODO: Support multi-dimensional array and array that lower bound is not zero.
	logger.WriteLog(L"Creating CComSafeArray<LPUNKNOWN> instance.");

	CComSafeArray<LPUNKNOWN> elems;

	if (rank != 1 || bases[0] != 0)
	{
		hr = elems.Create();
		if (FAILED(hr))
			AtlThrow(hr);

		logger.WriteLog(L"Array is multi-dimensional or starting index is not zero. Right now we does not support this kind of array.");
	}
	else
	{
		// Dump Elements.
		hr = elems.Create(nelems);
		if (FAILED(hr))
			AtlThrow(hr);

		logger.WriteLog(L"Start dumping %u elements.", nelems);

		for (ULONG32 i = 0; i < nelems; i++)
		{
			logger.WriteLog(L"Getting element %u.", i);

			LONG32 indices[] = { static_cast<LONG32>(i) };
			CComPtr<IXCLRDataValue> elem;

			hr = array->GetArrayElement(rank, indices, &elem);
			if (FAILED(hr))
				AtlThrow(hr);

			logger.WriteLog(L"Dumping element.");

			auto val = dump_object(elem);

			hr = elems.SetAt(i, val);
			if (FAILED(hr))
				AtlThrow(hr);
		}

		logger.WriteLog(L"Finished dumping %u elements.", nelems);
	}

	adata->SetValues(elems);

	return adata;
}

CComPtr<IComplexValue> exception_dumper::dump_complex_object(IXCLRDataValue *obj)
{
	HRESULT hr;
	CLRDATA_ENUM eh;

	// Enumerate static fields.
	hr = obj->StartEnumFields(CLRDATA_TYPE_ALL_KINDS | CLRDATA_FIELD_IS_INHERITED | CLRDATA_FIELD_FROM_STATIC, nullptr, &eh);
	if (FAILED(hr))
		AtlThrow(hr);

	hr = obj->EndEnumFields(eh);
	if (FAILED(hr))
		AtlThrow(hr);

	// Enumerate instance fields.
	hr = obj->StartEnumFields(CLRDATA_TYPE_ALL_KINDS | CLRDATA_FIELD_IS_INHERITED | CLRDATA_FIELD_FROM_INSTANCE, nullptr, &eh);
	if (FAILED(hr))
		AtlThrow(hr);

	hr = obj->EndEnumFields(eh);
	if (FAILED(hr))
		AtlThrow(hr);
}

CComPtr<IFrameData> exception_dumper::dump_frame(IXCLRDataFrame *frame)
{
	HRESULT hr;

	// Construct object to hold dumped CLR frame.
	logger.WriteLog(L"Creating FrameData instance.");

	auto fdata = create_com<FrameData>();

	// Get IXCLRDataMethodInstance to dump method details.
	logger.WriteLog(L"Getting IXCLRDataMethodInstance.");

	CComPtr<IXCLRDataMethodInstance> method;
	hr = frame->GetMethodInstance(&method);
	if (FAILED(hr))
		AtlThrow(hr);

	// Get method name.
	logger.WriteLog(L"Determine lenght of method name in the current frame.");

	ULONG32 nmethodname;
	hr = method->GetName(0, 0, &nmethodname, nullptr);
	if (FAILED(hr))
		AtlThrow(hr);

	logger.WriteLog(L"Getting name of the method in the current frame.");

	auto mname = std::make_unique<WCHAR[]>(nmethodname);
	hr = method->GetName(0, nmethodname, nullptr, mname.get());
	if (FAILED(hr))
		AtlThrow(hr);

	fdata->SetName(mname.get());

	// Get number of arguments.
	logger.WriteLog(L"Getting number of arguments for the method %s.", mname.get());

	ULONG32 nargs;
	hr = frame->GetNumArguments(&nargs);
	if (FAILED(hr))
		AtlThrow(hr);

	// Dump arguments.
	logger.WriteLog(L"Start dumping %u arguments for the method.", nargs);

	CComSafeArray<LPUNKNOWN> args(nargs);

	for (ULONG32 i = 0; i < nargs; i++)
	{
		logger.WriteLog(L"Dumping argument %u.", i);

		auto arg = dump_method_argument(frame, i);

		hr = args.SetAt(i, arg);
		if (FAILED(hr))
			AtlThrow(hr);
	}

	logger.WriteLog(L"Finished dumping arguments of method %s. Total arguments: %u.", mname.get(), nargs);
	fdata->SetParameters(args);

	return fdata;
}

CComPtr<IObjectDescriptor2> exception_dumper::dump_method_argument(IXCLRDataFrame *frame, ULONG32 aidx)
{
	HRESULT hr;

	// Create object to hold argument details.
	logger.WriteLog(L"Creating ObjectDescriptor2 for argument %u.", aidx);

	auto adata = create_com<ObjectDescriptor2>();

	// Query argument at the specific index.
	logger.WriteLog(L"Getting name and value.");

	WCHAR aname[128];
	CComPtr<IXCLRDataValue> arg;
	hr = frame->GetArgumentByIndex(aidx, &arg, _countof(aname), NULL, aname);
	if (FAILED(hr))
		AtlThrow(hr);

	adata->SetName(aname);

	// Query flags to determine type of object.
	logger.WriteLog(L"Getting value flags.");

	ULONG32 aflags;
	hr = arg->GetFlags(&aflags);
	if (FAILED(hr))
		AtlThrow(hr);

	// Query object's type.
	CComPtr<IXCLRDataTypeInstance> atype;

	if (aflags & CLRDATA_VALUE_IS_REFERENCE)
	{
		logger.WriteLog(L"Dereferencing and getting it type.");

		hr = arg->GetAssociatedType(&atype);
		if (FAILED(hr))
			AtlThrow(hr);
	}
	else
	{
		logger.WriteLog(L"Getting value type.");

		hr = arg->GetType(&atype);
		if (FAILED(hr))
			AtlThrow(hr);
	}

	if (hr == S_OK)
	{
		logger.WriteLog(L"Getting type name for the value.");

		WCHAR tname[128];
		hr = atype->GetName(0, _countof(tname), NULL, tname);
		if (FAILED(hr))
			AtlThrow(hr);

		adata->SetType(tname);

		logger.WriteLog(L"Start dumping %s.", tname);
	}
	else
		logger.WriteLog(L"Start dumping value.");

	// Dump argument's value.
	CComPtr<IUnknown> value;
	try
	{
		value = dump_object(arg);
	}
	catch (CAtlException& e)
	{
		logger.WriteLog(L"Failed to dump value: 0x%X.", e);
		logger.WriteLog(L"Creating UnknownValue.");

		value = create_com<UnknownValue>();
	}

	adata->SetValue(value);

	return adata;
}

CComPtr<IUnknown> exception_dumper::dump_object(IXCLRDataValue *obj)
{
	HRESULT hr;

	// Check for null value.
	logger.WriteLog(L"Getting object size.");

	ULONG64 osize;
	hr = obj->GetSize(&osize);

	if (FAILED(hr))
	{
		if (hr != E_NOINTERFACE) // E_NOINTERFACE mean the object does not have any data. That mean it is null.
			AtlThrow(hr);

		logger.WriteLog(L"Creating NullValue instance.");

		return create_com<NullValue>();
	}

	// Determine how to dump the object.
	logger.WriteLog(L"Size of object is %u. Start getting object flags.", osize);

	ULONG32 oflags;
	hr = obj->GetFlags(&oflags);
	if (FAILED(hr))
		AtlThrow(hr);

	logger.WriteLog(L"Object flags is 0x%X.", oflags);

	if (oflags & CLRDATA_VALUE_IS_REFERENCE)
	{
		logger.WriteLog(L"Dereferencing object.");

		CComPtr<IXCLRDataValue> real;
		hr = obj->GetAssociatedValue(&real);
		if (FAILED(hr))
			AtlThrow(hr);

		return dump_object(real);
	}
	else if (oflags & CLRDATA_VALUE_IS_ARRAY)
		return dump_array(obj);
	else if (oflags & CLRDATA_VALUE_IS_STRING)
		return dump_string(obj);
	else if (oflags & CLRDATA_VALUE_IS_VALUE_TYPE)
		return dump_complex_object(obj);
	else
		return dump_object_raw(obj);
}

CComPtr<IUnknown> exception_dumper::dump_object_raw(IXCLRDataValue *obj)
{
	HRESULT hr;

	// Create object to hold dumped data.
	auto odata = create_com<RawValue>();

	// Query object size to determine required buffer.
	ULONG64 osize;
	hr = obj->GetSize(&osize);
	if (FAILED(hr))
		AtlThrow(hr);

	// Copy object data.
	CComSafeArray<BYTE> raw;

	if (osize > ULONG_MAX)
	{
		// We don't support object that larger than 4,294,967,295 bytes. So we will just return an empty array.
		hr = raw.Create();
		if (FAILED(hr))
			AtlThrow(hr);
	}
	else
	{
		hr = raw.Create(static_cast<ULONG>(osize));
		if (FAILED(hr))
			AtlThrow(hr);

		hr = obj->GetBytes(static_cast<ULONG32>(osize), NULL, reinterpret_cast<BYTE *>(raw.m_psa->pvData));
		if (FAILED(hr))
			AtlThrow(hr);
	}

	odata->SetValue(raw);

	return odata;
}

CComPtr<IUnknown> exception_dumper::dump_string(IXCLRDataValue *str)
{
	HRESULT hr;

	// Create object to hold dumped string.
	auto sdata = create_com<StringValue>();

	// Query the length of string first to determine required buffer.
	ULONG32 slen;
	hr = str->GetString(0, &slen, NULL);
	if (FAILED(hr))
		AtlThrow(hr);

	// Query string.
	auto val = std::make_unique<WCHAR[]>(slen);

	hr = str->GetString(slen, NULL, val.get());
	if (FAILED(hr))
		AtlThrow(hr);

	sdata->SetValue(val.get());

	return sdata;
}
