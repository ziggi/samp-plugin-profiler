#include <iostream>

#include "perfcounter.h"

PerformanceCounter::PerformanceCounter() 
	: started_(false)
	, paused_(false)
	, child_(0)
	, parent_(0)
    , calls_(0)
	, counter_()
	, time_(0)  
{
}

PerformanceCounter::~PerformanceCounter() {
	//std::cout << "counter " << this << " destroyed" << std::endl;
	Stop();
}

void PerformanceCounter::Start(PerformanceCounter *parent) {
	if (!started_) {	
		//std::cout << "counter " << this << " started" << std::endl;

		if (parent != 0) {
			parent->child_ = this;
			parent->Pause();
		}	
		parent_ = parent;

		++calls_;
		counter_.start();

		started_ = true;		
	}
}

void PerformanceCounter::Stop() {
	if (started_) {
		counter_.stop();
		time_ += counter_.get_microseconds();

		//std::cout << "counter " << this << " stopped" << std::endl;

		if (child_ != 0) {
			child_->Stop();
			child_ = 0;
		}

		if (parent_ != 0) {
			parent_->child_ = 0;
			parent_->Resume();
			parent_ = 0;
		}

		started_ = false;
	}
}

void PerformanceCounter::Pause() {
	if (!paused_) {
		counter_.stop();
		time_ += counter_.get_microseconds();
		paused_ = true;
		//std::cout << "counter " << this << " paused" << std::endl;
	}
}

void PerformanceCounter::Resume() {
	if (paused_) {
		counter_.start();
		paused_ = false;
		//std::cout << "counter " << this << " resumed" << std::endl;
	}
}

platformstl::int64_t PerformanceCounter::GetCalls() const {
    return calls_;
}

platformstl::int64_t PerformanceCounter::GetTime() const {
    return time_;
}