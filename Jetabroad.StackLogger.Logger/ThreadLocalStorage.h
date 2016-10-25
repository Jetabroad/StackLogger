#pragma once

#include "GlobalEvents.h"

////////////////////////////////////////////////////////////////////////////////
// Actual Declaration:

template<class T>
class thread_storage final
{
public:
	thread_storage();
	thread_storage(const thread_storage&) = delete;

	thread_storage& operator=(const thread_storage&) = delete;

	void clear_value();
	std::shared_ptr<T> get_value();
	void set_value(const std::shared_ptr<T>& v);
private:
	std::shared_mutex mtx;
	std::unordered_map<DWORD, std::shared_ptr<T>> vals;
	std::unique_ptr<event_subscription> evsub;
};

////////////////////////////////////////////////////////////////////////////////
// thread_storage

template<class T>
inline thread_storage<T>::thread_storage()
{
	evsub = listen_thread_exit(std::bind(&thread_storage<T>::clear_value, this));
}

template<class T>
inline void thread_storage<T>::clear_value()
{
	std::lock_guard<std::shared_mutex> lock(mtx);

	vals.erase(GetCurrentThreadId());
}

template<class T>
inline std::shared_ptr<T> thread_storage<T>::get_value()
{
	std::shared_lock<std::shared_mutex> lock(mtx);

	auto it = vals.find(GetCurrentThreadId());
	if (it == vals.end())
		return nullptr;

	return it->second;
}

template<class T>
inline void thread_storage<T>::set_value(const std::shared_ptr<T>& v)
{
	std::lock_guard<std::shared_mutex> lock(mtx);

	vals.insert_or_assign(GetCurrentThreadId(), v);
}
