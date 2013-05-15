
#include "cppnetlibWorkerDelegate.h"

void cppnetlibDelegate::init(Worker::ParamMap&)
{
   // DEV NOTE: Lists can be populated in a background object, but must be allocated on the main thread.
    _listResult = boost::shared_ptr<EXTqlist>(new EXTqlist(listVlen));
	_headerResult = boost::shared_ptr<EXTqlist>(new EXTqlist(listVlen));
}

void cppnetlibDelegate::buildHeaderList(http::client::response response_)
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

Worker::ParamMap cppnetlibDelegate::run(Worker::ParamMap& params){
    Worker::ParamMap result;
	str255 colName;
	EXTfldval colVal;
    
	cancelled = false;

	// Fetch query command
    Worker::ParamMapIterator it = params.find("url");
    if( it == params.end()) {
        return result;
    }
	std::string url;
    try {
        url = boost::any_cast<std::string>(it->second);
    } catch (const boost::bad_any_cast& e) {
        return result;
    }

	try {
		http::client::request request_(url);
		request_ << header("Connection", "close");
		http::client client_(http::_follow_redirects=true, http::_cache_resolved=true);
		http::client::response response_ = client_.get(request_);
		std::string body_ = http::body(response_);

		buildHeaderList(response_);
		colName = initStr255("headers");
		_listResult->addCol(fftList, dpFcharacter, 1, &colName);

		colName = initStr255("body");
		_listResult->addCol(fftCharacter, dpFcharacter, 10000000, &colName);

		_listResult->insertRow();

		//add headers
		_listResult->getColValRef(1,1,colVal,qtrue);
		boost::shared_ptr<EXTqlist> ptr = boost::any_cast<boost::shared_ptr<EXTqlist> > (_headerResult);
		colVal.setList(ptr.get(), qtrue);
		
		//add body
		_listResult->getColValRef(1,2,colVal,qtrue);
		getEXTFldValFromString(colVal,body_);


		// Return list via parameters
		result["Result"] = _listResult;

	}
	catch (std::exception &e) {
	}
	return result;
}