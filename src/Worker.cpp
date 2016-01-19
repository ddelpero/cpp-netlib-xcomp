//
//  Worker.cpp
//  TMWorkQueue
//
//  Created by David McKeone on 11-11-01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Worker.h"
#include "WorkerDelegate.h"

#include <boost/format.hpp>

using boost::format;

Worker::Worker() : _complete(false), _running(false), _cancelled(false) 
{
	_http_request = false;
}

Worker::Worker(const ParamMap& p, const boost::shared_ptr<WorkerDelegate>& d) 
: _params(p), _delegate(d), _complete(false), _running(false), _cancelled(false) 
{
	_http_request = false;
}

Worker::Worker(const Worker& w)
{
    _workerName = w._workerName;
    _params = w._params;
    _result = w._result;
    
    _running = w._running;
    _complete = w._complete;
    _cancelled = w._cancelled;
    
    _queue = w._queue; 
    _delegate = w._delegate;

	_http_request = false;
} 

Worker::~Worker() {
    cancel();
    //_thread.join();  // Don't destruct objects until thread has finished
}


void Worker::callStudio()
{
	str255 methodName("$httpRequest");
	qret ret;
	qobjinst qobj = _object->getInstance();
	ret = ECOdoMethod( qobj, &methodName, 0, 0);
}

// Description of object used for logging
std::string Worker::desc() {
    if (!_workerName.empty()) {
        return str(format("Worker (%s)") % _workerName);
    } else {
        return "Worker";
    }  
}

// Override point for sub-classes to init objects that must run in the main thread
void Worker::init() {
    if (_delegate) {
        _delegate->init(_params);
    }
}

bool Worker::running() {
    // Get shared lock (multiple readers / one writer)
    boost::shared_lock<boost::shared_mutex> lock(_runMutex);
    return _running; 
}

void Worker::setRunning(bool r) {
    // Get unique lock
    boost::unique_lock<boost::shared_mutex> lock(_runMutex);
    _running = r; 
}

bool Worker::complete() {
    // Get shared lock (multiple readers / one writer)
    boost::shared_lock<boost::shared_mutex> lock(_completeMutex);
    
    return _complete; 
}

void Worker::setComplete(bool c) {
    // Get unique lock
    boost::unique_lock<boost::shared_mutex> lock(_completeMutex);
    _complete = c;
}

Worker::ParamMap Worker::result() 
{ 
    // Get shared lock (multiple readers / one writer)
    boost::shared_lock<boost::shared_mutex> lock(_resultMutex);
    
    return _result; 
}

void Worker::setResult(const Worker::ParamMap& pm) 
{ 
    boost::unique_lock<boost::shared_mutex> lock(_resultMutex);
    _result = pm; 
}

// Cancel status
bool Worker::cancelled() {
    // Get shared lock (multiple readers / one writer)
    boost::shared_lock<boost::shared_mutex> lock(_cancelMutex);
    
    return _cancelled; 
}

// Cancel request
void Worker::cancel() { 
    boost::unique_lock<boost::shared_mutex>(_cancelMutex); // Locked so many callers can cancel at the same time
    
    _cancelled = true; 
    
    if(_delegate) {
        _delegate->cancel();
    }
    LOG_DEBUG << desc() << " requested cancel.";
    
    if( !_thread.interruption_requested() ) {
        _thread.interrupt();
    }  
}


// Reset the current worker objects list
void Worker::reset() {
}

// Run worker
void Worker::run() {
    if(_delegate) {
        setResult( _delegate->run(_params) );
    }
}

// Start-up to run item on a thread
void Worker::start() {
    
    if(running() == true) {
        // Only start if not already running
        return;
    }
    
    // Run thread
	boost::shared_ptr<Worker> ptr = shared_from_this();
    _thread = boost::thread(WorkerThread(ptr/*shared_from_this()*/, _delegate));
}

// Start-up to run item on a thread
void Worker::start(boost::shared_ptr<Queue> q) {
    
    if(running() == true) {
        // Only start if not already running
        return;
    }
    
    _queue = q;
    
    // Run thread
    _thread = boost::thread(WorkerThread(shared_from_this(), _delegate));
}

// Thread entry point
void Worker::WorkerThread::operator()() {
    
    // Get shared pointer
    boost::shared_ptr<Worker> ptr;
    Worker::ParamMap params;
    {
        try { 
			ptr = _worker.lock();
        } catch (const boost::bad_weak_ptr& e) {
            return;
        }

		if (ptr == NULL)
		{
            LOG_ERROR << "Pointer to worker class is null.";
			return;
		}
        
        // Lock work mutex
        boost::unique_lock<boost::mutex> lock(ptr->_workMutex);
        
        if(ptr->running()) {
            // Thread is already running
            return;
        }
        
        // Mark worker as running        
        ptr->setRunning(true);
        
        LOG_INFO << ptr->desc() << " started";
        
        // Copy Params
        params = ptr->_params;
        
        // Release shared pointer prior to starting logic (otherwise it holds a reference and prevents worker destruct)
        
    }
    
    // Perform worker work
	_delegate->setWorker(ptr);
    Worker::ParamMap result = _delegate->run(params); 
    
	if (ptr != NULL){
		ptr.reset(); 
	}

    // Re-acquire shared pointer
    try { 
        ptr = _worker.lock();
    } catch (const boost::bad_weak_ptr& e) {
        // Worker out of scope
        LOG_INFO << "Worker out of scope at completion";
        return;
    }
    
    if (ptr) {
        ptr->setResult(result);
        
        if(ptr->cancelled()) {
            LOG_INFO << ptr->desc() << " canceled";
        } else {
            LOG_INFO << ptr->desc() << " complete";
        }
        
        ptr->setComplete(true);
        ptr->setRunning(false);
    }
}




