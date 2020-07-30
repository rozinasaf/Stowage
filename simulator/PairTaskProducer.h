#pragma once
#include <iostream>
#include<thread>
#include<mutex>
#include<atomic>
#include<vector>
#include<optional>
#include<functional>
#include<chrono>
#include "ThreadPoolExecuter.h"
#include "ThreadsStrongTypes.h"


class PairTaskProducer {
	const std::vector<PairTask>& tasks;
	const int numTasks = -1;
	std::atomic_int task_counter = 0;
	std::mutex m;
public:
	PairTaskProducer(const std::vector<PairTask>& tasks)
		: tasks(tasks), numTasks(tasks.size()) {}
	PairTaskProducer(PairTaskProducer&& other)
		: tasks(other.tasks), numTasks(other.numTasks), task_counter(other.task_counter.load()) {}
	
	std::optional<int> next_task_index() {
		for (int curr_counter = task_counter.load(); curr_counter < numTasks;) {
			if (task_counter.compare_exchange_weak(curr_counter, curr_counter + 1)) {
				return { curr_counter }; // zero based
			}
		}
		return {};
	}
	std::optional<PairTask> getTask() {
		auto task_index = next_task_index();
		if (task_index) {
			PairTask task = tasks[task_index.value()];
			return task;
		}
		else return {};
	}
};
