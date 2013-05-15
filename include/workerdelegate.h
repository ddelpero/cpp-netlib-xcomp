// Worker Delegate

#ifndef WORKERDELEGATE_H_
#define WORKERDELEGATE_H_

#include <boost/any.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Worker.h"

class WorkerDelegate : public boost::enable_shared_from_this<WorkerDelegate> {
public:
    virtual void init(Worker::ParamMap&) = 0;
    virtual Worker::ParamMap run(Worker::ParamMap&) = 0;
    virtual void cancel();
	void checkForCancelledThread();
protected:
	 bool cancelled;
};

#endif