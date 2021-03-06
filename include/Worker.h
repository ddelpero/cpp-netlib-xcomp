//
//  Worker.h
//  TMWorkQueue
//
//  Created by David McKeone on 11-11-01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef WORKER_H_
#define WORKER_H_

#include "Tools.h"
#include "Logging.he"


#include <string>
//#include <extcomp.he>

#include <boost/any.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

class Queue;  // Forward declare to hold a pointer to the queue the object is in
class WorkerDelegate;

class Worker : public boost::enable_shared_from_this<Worker> {
public:    
    typedef std::map<std::string, boost::any, Tools::CaseInsensitiveLess> ParamMap;
    typedef std::map<std::string, boost::any, Tools::CaseInsensitiveLess>::iterator ParamMapIterator;
    
    Worker();
    Worker(const ParamMap& p, const boost::shared_ptr<WorkerDelegate>& d);
    Worker(const Worker& w);
    virtual ~Worker();
    
    // Starting worker
    void run();
    void start();
    void start(boost::shared_ptr<Queue>);
    
    bool running();
    void setRunning(bool r);
    
    bool complete();
    void setComplete(bool c);
    
    void cancel();
    bool cancelled();
    
    ParamMap result();
    void setResult(const ParamMap& pm);
    
    virtual void init(); // Code to be run prior to entering the thread
    virtual std::string desc();
    virtual void reset();
    
    void setWorkerName(std::string wn) { _workerName = wn; }
    
    //void setDelegate(const boost::shared_ptr<WorkerDelegate>& d) { _delegate = d; }
    
protected:
    std::string _workerName;
    
    ParamMap _params;
    ParamMap _result;
private:
    
    class WorkerThread {
    public: 
        WorkerThread() {}
        WorkerThread(const boost::weak_ptr<Worker>& w, const boost::shared_ptr<WorkerDelegate>& wd) : _worker(w), _delegate(wd) {}
        
        void operator()();
    private:
        boost::weak_ptr<Worker> _worker;
        boost::shared_ptr<WorkerDelegate> _delegate;
    };
    
    bool _running;
    bool _complete;
    bool _cancelled;
    
    boost::shared_mutex _runMutex;
    boost::shared_mutex _completeMutex;
    boost::shared_mutex _cancelMutex;
    boost::shared_mutex _resultMutex;
    boost::mutex _workMutex;
    boost::thread _thread;
    
    boost::shared_ptr<Queue> _queue;
    boost::shared_ptr<WorkerDelegate> _delegate;
};


#endif // WORKER_H_



