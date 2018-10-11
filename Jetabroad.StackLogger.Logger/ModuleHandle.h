// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

class module_handle final
{
public:
	module_handle();
	explicit module_handle(HMODULE h);
	module_handle(module_handle&& other);
	module_handle(const module_handle&) = delete;
	~module_handle();

	operator HMODULE() const;

	module_handle& operator=(module_handle&& other);
	module_handle& operator=(const module_handle&) = delete;

	void unload();
private:
	HMODULE h;
};
