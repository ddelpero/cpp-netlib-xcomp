//
//  Logging.cpp
//  TMWorkQueue
//
//  Created by David McKeone on 11-11-11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Logging.he"

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

void Logging::init() {
}

void Logging::stop() {
}

/*include <boost/log/common.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/record_ordering.hpp>

using boost::shared_ptr;
using namespace boost::log;

typedef boost::log::sinks::text_file_backend backend_t;
typedef boost::log::sinks::synchronous_sink<backend_t> sink_t;

static boost::shared_ptr<sink_t> fileSink;

void Logging::init() {
    
//#ifdef _DEBUG
    init_log_to_console();
//#endif

    // Create a text file sink
    fileSink = shared_ptr<sink_t>(new sink_t(keywords::file_name = "TheatreManager_%Y%m%d_%H%M_%5N.log",      // file name pattern
                                             keywords::rotation_size = 16384));                                 // rotation size, in characters
                                 
    // Set up where the rotated files will be stored
    fileSink->locked_backend()->set_file_collector(sinks::file::make_collector(
                                                                           keywords::target = "/Library/Logs/TheatreManager",  // where to store rotated files
                                                                           keywords::max_size = 16 * 1024 * 1024,              // maximum total size of the stored files, in bytes
                                                                           keywords::min_free_space = 100 * 1024 * 1024        // minimum free space on the drive, in bytes
                                                                           ));
    
    // Upon restart, scan the target directory for files matching the file_name pattern
    fileSink->locked_backend()->scan_for_files();
    
    fileSink->locked_backend()->set_formatter(
                                          formatters::format("%1%: [%2%] - %3%")
                                          % formatters::attr<unsigned int>("RecordID")
                                          % formatters::date_time<boost::posix_time::ptime>("TimeStamp")
                                          % formatters::message()
                                          );
    
    fileSink->locked_backend()->auto_flush(true); 
    
    // Add it to the core
    core::get()->add_sink(fileSink);
    
    // Add some attributes too
    core::get()->add_global_attribute("TimeStamp", attributes::local_clock());
    core::get()->add_global_attribute("RecordID", attributes::counter<unsigned int>());
}

void Logging::stop() {    
	// Remove the sink from the core, so that no records are passed to it
    core::get()->remove_sink(fileSink);
    
    fileSink.reset();
}
*/