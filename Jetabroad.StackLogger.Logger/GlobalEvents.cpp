#include "PCH.h"
#include "GlobalEvents.h"

////////////////////////////////////////////////////////////////////////////////
// Mutexes & Handlers.

namespace events
{
	namespace thread_exit
	{
		static std::shared_mutex mutex;
		static std::list<thread_handler> handlers;
	}
}

////////////////////////////////////////////////////////////////////////////////
// event_subscription

event_subscription::event_subscription(std::list<thread_handler>::iterator h, const std::function<void(std::list<thread_handler>::iterator)>& u) :
	handler_(h),
	unsubscribe_(u)
{
}

event_subscription::~event_subscription()
{
	unsubscribe();
}

void event_subscription::unsubscribe()
{
	unsubscribe_(handler_);
}

////////////////////////////////////////////////////////////////////////////////
// Global Functions.

std::unique_ptr<event_subscription> listen_thread_exit(const thread_handler& h)
{
	using namespace events::thread_exit;
	std::list<thread_handler>::iterator iter;

	{
		std::lock_guard<std::shared_mutex> lock(mutex);
		iter = handlers.insert(handlers.end(), h);
	}

	try
	{
		return std::make_unique<event_subscription>(iter, [](std::list<thread_handler>::iterator h)
		{
			std::lock_guard<std::shared_mutex> lock(mutex);
			handlers.erase(h);
		});
	}
	catch (...)
	{
		std::lock_guard<std::shared_mutex> lock(mutex);
		handlers.erase(iter);
		throw;
	}
}

void raise_thread_exit()
{
	using namespace events::thread_exit;
	std::shared_lock<std::shared_mutex> lock(mutex);

	for (auto& h : handlers)
		h();
}
