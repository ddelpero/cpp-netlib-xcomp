
#include "cppnetlibHTTPServerWorkerDelegate.h"
extern concurrent_queue<int> queue_to_studio;
extern concurrent_queue<int> queue_from_studio;

//struct hello_world;
//typedef http::server<hello_world> server;

/*<< Defines the request handler.  It's a class that defines two
     functions, `operator()` and `log()` >>*/
struct hello_world {
	boost::shared_ptr<Worker> _worker;
    /*<< This is the function that handles the incoming request. >>*/
    void operator() (server::request const &request,
                     server::response &response) {
        server::string_type ip = source(request);
        unsigned int port = request.source_port;
        std::ostringstream data;
        int new_value;
		queue_to_studio.push(port);
		// boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		while (!queue_from_studio.try_pop(new_value))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
		data << "Hello, " << ip << ':' << new_value << '!';
        response = server::response::stock_reply(server::response::ok, data.str());
    }
    /*<< It's necessary to define a log function, but it's ignored in
         this example. >>*/
    void log(...) {
        // do nothing
    }
};

void cppnetlibHTTPServerDelegate::init(Worker::ParamMap&)
{
   // DEV NOTE: Lists can be populated in a background object, but must be allocated on the main thread.
    _listResult = boost::shared_ptr<EXTqlist>(new EXTqlist(listVlen));
	_headerResult = boost::shared_ptr<EXTqlist>(new EXTqlist(listVlen));

	
}

void cppnetlibHTTPServerDelegate::buildHeaderList(http::client::response response_)
{
	using namespace boost::network;
	typedef headers_range<http::client::response>::type response_headers;
	typedef boost::range_iterator<response_headers>::type iterator;

	std::string name;
	std::string value;
    str255 colName;
	EXTfldval colVal;
    int col = 0;

	response_headers headers_ = http::headers(response_);
	iterator it = headers_.begin();
	
	for (it; it != headers_.end(); ++it) 
	{
		name = it->first;
		colName = initStr255(name.c_str());
        _headerResult->addCol(fftCharacter, dpFcharacter, 10000000, &colName);
	}

	_headerResult->insertRow();
	it = headers_.begin();
	for (it; it != headers_.end(); ++it) 
	{
		value = it->second;
	    _headerResult->getColValRef(1,col+1,colVal,qtrue);
        getEXTFldValFromString(colVal,value);
		col++;
	}
}

Worker::ParamMap cppnetlibHTTPServerDelegate::run(Worker::ParamMap& params){
    Worker::ParamMap result;
	str255 colName;
	EXTfldval colVal;
    
	cancelled = false;    

	try {
		/*<< Creates the request handler. >>*/
        hello_world handler;
		handler._worker = _worker;
        /*<< Creates the server. >>*/
		server::options options(handler);
		options.address("127.0.0.1");
		options.port("8000");
        server server_(options);
        /*<< Runs the server. >>*/
        server_.run();


		// Return list via parameters
		result["Result"] = _listResult;

	}
	catch (std::exception &e) {
	}
	return result;
}