#pragma once

#include <iostream>
#include<thread>
#include<mutex>
#include<atomic>
#include<vector>
#include<optional>
#include<functional>
#include<chrono>
#include "ThreadsStrongTypes.h"


template<typename Producer>
class ThreadPoolExecuter {
	Producer producer;
	const int numThreads = -1;
    	std::mutex printLock;
	std::vector<std::thread> workers;
	std::atomic_bool running = false;
	std::atomic_bool stopped = false;
	static thread_local int num_tasks_finished;
	std::atomic_int total_num_tasks_finished{ 0 };

	void worker_function() {
		while (!stopped) {
			auto task = producer.getTask();
			if (!task) break;
			(*task).simulatePair();
			++num_tasks_finished;
			++total_num_tasks_finished;
		}

        	const std::lock_guard<std::mutex> lock(printLock);

		if (stopped) {
			std::cout << std::this_thread::get_id() << " - stopped gracefully after processing " << num_tasks_finished << " task(s)" << std::endl;
		}
		else {
			std::cout << std::this_thread::get_id() << " - finished after processing " << num_tasks_finished << " task(s)" << std::endl;
		}
	}
public:
	ThreadPoolExecuter(Producer producer, NumThreads numThreads)
		: producer(std::move(producer)), numThreads(numThreads) {
		workers.reserve(numThreads);
	}
	bool start() {
		bool running_status = false;
		if (!running.compare_exchange_strong(running_status, true)) {
			return false;
		}
		for (int i = 0; i < numThreads; ++i) {
			workers.push_back(std::thread([this] {
				worker_function();
				}));
		}
		return true;
	}
	void stop_gracefully() {
		stopped = true;
		wait_till_finish();
	}
	void wait_till_finish() {
		for (auto& t : workers) {
			t.join();
		}
		std::cout << "thread pool finished/stopped after processing " << total_num_tasks_finished << " task(s)" << std::endl;
	}
};

template<typename Producer>
thread_local int ThreadPoolExecuter<Producer>::num_tasks_finished{ 0 };

