// AlgorithmRegistrar.h
#pragma once

#include <map>
#include <functional>
#include <memory> 
#include <vector>
#include "../interfaces/AbstractAlgorithm.h"

// A singleton to hold algorithm factories and .so handles 
class AlgorithmRegistrar
{
	struct DlCloser
	{
		void operator()(void* dlhandle) const noexcept;
	};

	// Algorithm factories signature type

	typedef std::unique_ptr<void, DlCloser> DlHandler;

	// Dynamic-linker handles map (to keep algorithms code in memory)
	std::map<std::string, DlHandler> _handles;

	// Algorithm factories
	std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> _factories;

	// iteration over factories
	typedef decltype(_factories)::const_iterator const_iterator;

	// Default Constructor
	AlgorithmRegistrar() = default;
	~AlgorithmRegistrar();

	// Prevent copy - this is singelton
	AlgorithmRegistrar(const AlgorithmRegistrar&) = delete;
	AlgorithmRegistrar& operator=(const AlgorithmRegistrar&) = delete;

	// AlgorithmRegistration needs to internally registerFactory
	friend class AlgorithmRegistration;

	// AlgorithmRegistration's internal interface to support register-on-load policy
	inline void registerFactory(std::function<std::unique_ptr<AbstractAlgorithm>()> factory) {
		int sizeBefore = (int)_factories.size();
		int sizeAfter = 0;
		_factories.push_back(factory);
		sizeAfter = (int)_factories.size();
		if (sizeBefore != (sizeAfter - 1)) {}
		//	errorVec.push_back("");
	}

public:
	// Gets a reference to AlgorithmRegistrar's singleton instance
	static AlgorithmRegistrar& getInstance();

	std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> getFactories() {
		return _factories;
	}
	// Rreturns an iterator to the first algorithm factory
	inline const_iterator begin() const { return _factories.begin(); }

	// Returns an iterator that points to the end
	inline const_iterator end() const { return _factories.end(); }

	// Loads an algorithm from given shared object file into the registrar
	// parameters:
	//   file_path - shared object file path to load
	//   error - On failure, it will be filled with a Human-readable error message.
	// Returns:
	//   true if given shared object's algorithm was registered successfully, false oterwise
	bool loadAlgorithmFromFile(const char* file_path);
};
