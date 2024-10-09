#pragma once
#
#include <vector>

template<class T>
class State
{
public:
	State(T* owner) :owner_(owner) {}
	virtual ~State() {}

	virtual void Initialize() = 0;
	virtual void Update(const float& elapsedTime) = 0;
	virtual void Finalize() = 0;

protected:
	T* owner_;
};