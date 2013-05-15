
#include "WorkerDelegate.h"

// Check for cancel on each row(in case there are a lot of rows)
void WorkerDelegate::checkForCancelledThread()
{
    try {
       boost::this_thread::interruption_point();
    } catch (const boost::thread_interrupted&) {
       cancel();
    }
}

void WorkerDelegate::cancel() {
    cancelled = true;
}