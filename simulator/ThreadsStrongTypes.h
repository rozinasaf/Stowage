#pragma once

template<typename T>
class Named {
	T value;
public:
	explicit Named(T value) : value{ value } {}
	operator T() const { return value; }
};
class NumTasks : public Named<int> {
	using Named<int>::Named;
};
class IterationsPerTask : public Named<int> {
	using Named<int>::Named;
};
class NumThreads : public Named<int> {
	using Named<int>::Named;
};
