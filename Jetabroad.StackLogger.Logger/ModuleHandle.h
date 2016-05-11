#pragma once

class module_handle final
{
public:
	module_handle() noexcept;
	explicit module_handle(HMODULE h) noexcept;
	module_handle(module_handle&& other) noexcept;
	module_handle(const module_handle&) = delete;
	~module_handle();

	operator HMODULE() const;

	module_handle& operator=(module_handle&& other);
	module_handle& operator=(const module_handle&) = delete;

	void unload();
private:
	HMODULE h;
};
