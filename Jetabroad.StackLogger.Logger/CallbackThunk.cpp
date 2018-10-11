// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "CallbackThunk.h"

template<typename T>
static binary instruction_with_param(std::initializer_list<uint8_t> op, T param)
{
	binary inst = op;
	inst.append(reinterpret_cast<uint8_t *>(&param), sizeof(param));
	return inst;
}

#if defined(_M_AMD64)
// x64 have only one calling convention.
// Documents for it can be found here: https://msdn.microsoft.com/en-us/library/9b372w95.aspx
//
// This function will generate the following code on the fly:
//
//    push		rbp
//    mov		rbp,rsp
// [] mov		qword ptr [rbp-0x10],rcx|xmm0
// [] mov		qword ptr [rbp-0x18],rdx|xmm1
// [] mov		qword ptr [rbp-0x20],r8|xmm2
// [] mov		qword ptr [rbp-0x28],r9|xmm3
// [] push		qword ptr [rbp-0x30]
// [] push		qword ptr [rbp-0x28]
//    sub		rsp,0x20
//    movabs	rcx,pObject
// [] mov		rdx|xmm1,qword ptr [rbp-0x10]
// [] mov		r8|xmm2,qword ptr [rbp-0x18]
// [] mov		r9|xmm3,qword ptr [rbp-0x20]
//    movabs	rax,pCallback
//    call		rax
//    mov		rsp,rbp
//    pop		rbp
//    ret
//
// Instructions that have [] prefix is optional. It depends on some conditions to generate.
static void * create_x64_thunk(void *obj, void *callback, std::uint8_t nargs, const argument_description args[])
{
	// Initialise Prolog.
	binary inst =
	{
		0x55,				// push	rbp
		0x48, 0x89, 0xE5,	// mov	rbp,rsp
	};

	// Copy the first 4 arguments to shadow space to make it easy to work on later.
	if (nargs >= 1)
	{
		switch (args[0].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	qword ptr [rbp-0x10],rcx
			inst.append({ 0x48, 0x89, 0x4d, 0xF0 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 1.");
		}
	}

	if (nargs >= 2)
	{
		switch (args[1].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	qword ptr [rbp-0x18],rdx
			inst.append({ 0x48, 0x89, 0x55, 0xE8 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 2.");
		}
	}

	if (nargs >= 3)
	{
		switch (args[2].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	qword ptr [rbp-0x20],r8
			inst.append({ 0x4C, 0x89, 0x45, 0xE0 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 3.");
		}
	}

	if (nargs >= 4)
	{
		switch (args[3].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	qword ptr [rbp-0x28],r9
			inst.append({ 0x4C, 0x89, 0x4D, 0xD8 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 4.");
		}
	}

	// Push arguments that will be passed via stack.
	// We can pick only 3 arguments to pass via registers since we need the first one (rcx) for "obj".
	// That mean we need to pass the 4th argument or more via stack.
	//
	// The following is the layout of memory relative to base pointer:
	// -00: value need to restore to stack pointer upon return from the function.
	// -08: address to return from the function.
	// -16: shadow space for 1st argument.
	// -24: shadow space for 2nd argument.
	// -32: shadow space for 3rd argument.
	// -40: shadow space for 4th argument.
	// -48: argument 5.
	// -56: argument 6.
	// ...: argument ..
	std::uint8_t viastack = (nargs > 3) ? nargs - 3 : 0;
	int argpos = -32 - viastack * 8; // -32 is the position of the third argument. We will starting to push from the last argument.

	for (std::uint8_t i = 0; i < viastack; i++)
	{
		// push	qword ptr [rbp+argpos]
		inst.append(instruction_with_param({ 0xFF, 0xB5 }, argpos));
		argpos += 8;
	}

	// Allocate shadow space for the first 4 arguments.
	inst.append({ 0x48, 0x83, 0xEC, 0x20 }); // sub	rsp,0x20

	// Set the first 4 arguments.
	inst.append(instruction_with_param({ 0x48, 0xB9 }, obj)); // movabs	rcx,pObject

	if (nargs >= 1)
	{
		switch (args[0].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	rdx,qword ptr [rbp-0x10]
			inst.append({ 0x48, 0x8B, 0x55, 0xF0 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 1.");
		}
	}

	if (nargs >= 2)
	{
		switch (args[1].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	r8,qword ptr [rbp-0x18]
			inst.append({ 0x4C, 0x8B, 0x45, 0xE8 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 2.");
		}
	}

	if (nargs >= 3)
	{
		switch (args[2].type)
		{
		case argument_type::pointer:
		case argument_type::integer:
			// mov	r9,qword ptr [rbp-0x20]
			inst.append({ 0x4C, 0x8B, 0x4D, 0xE0 });
			break;
		default:
			throw std::invalid_argument("Invalid type on argument 3.");
		}
	}

	// Call Callback.
	inst.append(instruction_with_param({ 0x48, 0xB8 }, callback));	// movabs	rax,pCallback
	inst.append({ 0xFF, 0xD0 });									// call		rax
	inst.append({ 0x48, 0x89, 0xEC });								// mov		rsp,rbp
	inst.append({ 0x5D });											// pop		rbp
	inst.append({ 0xC3 });											// ret

	// Copy to executable page.
	auto thunk = VirtualAlloc(nullptr, inst.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!thunk)
		throw std::system_error(GetLastError(), std::system_category());

	try
	{
		memcpy(thunk, inst.data(), inst.size());
		FlushInstructionCache(GetCurrentProcess(), thunk, inst.size());
		return thunk;
	}
	catch (...)
	{
		VirtualFree(thunk, 0, MEM_RELEASE);
		throw;
	}
}
#else
#error Target platform is not supported.
#endif

callback_thunk::callback_thunk() :
	thunk(nullptr)
{
}

callback_thunk::~callback_thunk()
{
	if (thunk)
		VirtualFree(thunk, 0, MEM_RELEASE);
}

void * callback_thunk::get_thunk() const
{
	return thunk;
}

void * callback_thunk::create_stdcall_thunk(void *obj, void *callback, std::uint8_t nargs, const argument_description args[])
{
#if defined(_M_AMD64)
	return create_x64_thunk(obj, callback, nargs, args);
#else
#error Target platform is not supported.
#endif
}
