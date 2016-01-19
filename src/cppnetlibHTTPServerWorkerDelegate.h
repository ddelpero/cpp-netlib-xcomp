// HTTPServer Worker Delegate

#ifndef CPPNETLIBHTTPSERVERWORKERDELEGATE_H_
#define CPPNETLIBHTTPSERVERWORKERDELEGATE_H_

#include "WorkerDelegate.h"
#include "OmnisTools.he"
#include "common.he"

//#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::network;
//using namespace boost::network::http;


struct hello_world;
typedef http::server<hello_world> server;

//using namespace boost::interprocess;
	



class cppnetlibHTTPServerDelegate : public WorkerDelegate {
public:
    virtual void init(Worker::ParamMap&);
    virtual Worker::ParamMap run(Worker::ParamMap&);
	
private:
    boost::shared_ptr<EXTqlist> _listResult;
    boost::shared_ptr<EXTqlist> _headerResult;
	void buildHeaderList(http::client::response response_);
	
};

#endif