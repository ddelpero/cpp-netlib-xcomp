// FileCopy Worker Delegate

#ifndef CPPNETLIBWORKERDELEGATE_H_
#define CPPNETLIBWORKERDELEGATE_H_

#include "WorkerDelegate.h"
#include "OmnisTools.he"
#include "common.he"


using namespace boost::network;
//using namespace boost::network::http;



class cppnetlibDelegate : public WorkerDelegate {
public:
    virtual void init(Worker::ParamMap&);
    virtual Worker::ParamMap run(Worker::ParamMap&);
private:
    boost::shared_ptr<EXTqlist> _listResult;
    boost::shared_ptr<EXTqlist> _headerResult;
	void buildHeaderList(http::client::response response_);
};

#endif