// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Types.

typedef std::function<void()> thread_handler;

class event_subscription final
{
public:
	event_subscription(std::list<thread_handler>::iterator h, const std::function<void(std::list<thread_handler>::iterator)>& u);
	event_subscription(const event_subscription&) = delete;
	~event_subscription();

	event_subscription& operator=(const event_subscription&) = delete;

	void unsubscribe();
private:
	std::list<thread_handler>::iterator handler_;
	std::function<void(std::list<thread_handler>::iterator)> unsubscribe_;
};

////////////////////////////////////////////////////////////////////////////////
// Functions.

// h must not call into listen_thread_exit(), raise_thread_exit() and event_subscription::unsubscribe().
std::unique_ptr<event_subscription> listen_thread_exit(const thread_handler& h);

void raise_thread_exit();
