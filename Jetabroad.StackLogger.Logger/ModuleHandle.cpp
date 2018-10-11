// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "ModuleHandle.h"

module_handle::module_handle() :
	h(nullptr)
{
}

module_handle::module_handle(HMODULE h) :
	h(h)
{
}

module_handle::module_handle(module_handle&& other) :
	h(other.h)
{
	other.h = nullptr;
}

module_handle::~module_handle()
{
	unload();
}

module_handle::operator HMODULE() const
{
	return h;
}

module_handle& module_handle::operator=(module_handle&& other)
{
	unload();

	h = other.h;
	other.h = nullptr;

	return *this;
}


void module_handle::unload()
{
	if (h && !FreeLibrary(h))
		throw std::system_error(GetLastError(), std::system_category());

	h = nullptr;
}
