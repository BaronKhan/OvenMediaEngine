//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Jaejong Bong
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <config/items/items.h>
#include <modules/http/server/http_server.h>

#include <list>
#include <thread>

#include "segment_worker_manager.h"

class SegmentStreamInterceptor : public HttpDefaultInterceptor
{
public:
	SegmentStreamInterceptor();
	~SegmentStreamInterceptor() override;

	bool Start(int thread_count, const SegmentProcessHandler &process_handler);
	HttpInterceptorResult OnHttpData(const std::shared_ptr<HttpConnection> &client, const std::shared_ptr<const ov::Data> &data) override;
	void SetCrossdomainBlock()
	{
		_is_crossdomain_block = false;
	}

	bool IsInterceptorForRequest(const std::shared_ptr<const HttpConnection> &client) override;

protected:
	SegmentWorkerManager _worker_manager;
	bool _is_crossdomain_block;
};
