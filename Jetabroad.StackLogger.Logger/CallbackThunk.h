// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Type Delcaration.

enum class calling_convention
{
	stdcall
};

enum class argument_type
{
	pointer,
	integer
};

struct argument_description
{
	argument_type type;
	std::size_t size;
};

// This class will wrap a specified instance method inside another function that will be generate on runtime.
// The purpose of this class is to make it possible to use instance method as a callback for Win32 API.
class callback_thunk final
{
public:
	callback_thunk();
	~callback_thunk();

	template<typename T>
	void * create(void *obj, T callback, const argument_description args[] = nullptr, std::uint8_t narg = 0, calling_convention cc = calling_convention::stdcall);
	void * get_thunk() const;
private:
	static void * create_stdcall_thunk(void *obj, void *callback, std::uint8_t nargs, const argument_description args[]);

	void *thunk;
};

////////////////////////////////////////////////////////////////////////////////
// Inline Implementation.

template<typename T>
inline void * callback_thunk::create(void *obj, T callback, const argument_description args[], std::uint8_t narg, calling_convention cc)
{
	union { T cb; void *addr; } cb = { callback }; // C++ not allow use to cast function pointer to void pointer directly, this is a hack to make it possible.

	switch (cc)
	{
	case calling_convention::stdcall:
		thunk = create_stdcall_thunk(obj, cb.addr, narg, args);
		break;
	default:
		throw std::invalid_argument("Invalid calling convention.");
	}

	ATLTRACE2(L"Created callback thunk at 0x%IX.\n", thunk);

	return thunk;
}
