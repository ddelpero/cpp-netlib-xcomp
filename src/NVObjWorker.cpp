//
//  NVObjWorker.cpp
//  TMWorkQueue
//
//  Created by  on 11-10-21.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//#include <extcomp.he>
#include "NVObjWorker.he"
#include "ThreadTimer.he"
#include "Logging.he"
#include "common.he"

#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>

using boost::format;
using namespace OmnisTools;

/**************************************************************************************************
 **                       CONSTRUCTORS / DESTRUCTORS                                             **
 **************************************************************************************************/

// Constructor
NVObjWorker::NVObjWorker(qobjinst objinst, tThreadData* pThreadData) : NVObjBase(objinst) {

}

// Destructor
NVObjWorker::~NVObjWorker() {    
    // Unsubscribe this instance from the timer
    ThreadTimer& timerInst = ThreadTimer::instance();
    timerInst.unsubscribe(this);
}

/**************************************************************************************************
 **                              PROPERTY DECLERATION                                            **
 **************************************************************************************************/

// This is where the resource # of the methods is defined.  In this project it is also used as the Unique ID.
const static qshort cPropertyMyProperty = 4500;

/**************************************************************************************************
 **                               METHOD DECLERATION                                             **
 **************************************************************************************************/

// This is where the resource # of the methods is defined.  In this project is also used as the Unique ID.
const static qshort cMethodError      = 4000,
                    cMethodInitialize = 4001,
                    cMethodRun        = 4002,
                    cMethodStart      = 4003,
                    cMethodCancel     = 4004,
					cMethodDone       = 4005;

/**************************************************************************************************
 **                                 INSTANCE METHODS                                             **
 **************************************************************************************************/

// Call a method
qlong NVObjWorker::methodCall( tThreadData* pThreadData )
{
	tResult result = METHOD_OK;
	qshort funcId = (qshort)ECOgetId(pThreadData->mEci);
	qshort paramCount = ECOgetParamCount(pThreadData->mEci);
    
	switch( funcId )
	{
		case cMethodError:
			result = METHOD_OK; // Always return ok to prevent circular call to error.
			break;
		case cMethodInitialize:
			pThreadData->mCurMethodName = "$initialize";
			result = methodInitialize(pThreadData, paramCount);
			break;
        case cMethodRun:
            pThreadData->mCurMethodName = "$run";
            result = methodRun(pThreadData, paramCount);
            break;
        case cMethodStart:
            pThreadData->mCurMethodName = "$start";
            result = methodStart(pThreadData, paramCount);
            break;
        case cMethodCancel:
            pThreadData->mCurMethodName = "$canceled";
            result = methodCancel(pThreadData, paramCount);
            break;
	    case cMethodDone:
            pThreadData->mCurMethodName = "$done";
            result = methodCancel(pThreadData, paramCount);
            break;
	}
	
	callErrorMethod(pThreadData, result);
    
	return 0L;
}

/**************************************************************************************************
 **                                PROPERTIES                                                    **
 **************************************************************************************************/

// Assignability of properties
qlong NVObjWorker::canAssignProperty( tThreadData* pThreadData, qlong propID ) {
	switch (propID) {
		case cPropertyMyProperty:
			return qtrue;
		default:
			return qfalse;
	}
}

// Method to retrieve a property of the object
qlong NVObjWorker::getProperty( tThreadData* pThreadData ) 
{
	EXTfldval fValReturn;
    
	qlong propID = ECOgetId( pThreadData->mEci );
	switch( propID ) {
		case cPropertyMyProperty:
			//fValReturn.setLong(myProperty); // Put property into return value
			ECOaddParam(pThreadData->mEci, &fValReturn); // Return to caller
			break;	       
	}
    
	return 1L;
}

// Method to set a property of the object
qlong NVObjWorker::setProperty( tThreadData* pThreadData )
{
	// Retrieve value to set for property, always in first parameter
	EXTfldval fVal;
	if( getParamVar( pThreadData->mEci, 1, fVal) == qfalse ) 
		return qfalse;
    
	// Assign to the appropriate property
	qlong propID = ECOgetId( pThreadData->mEci );
	switch( propID ) {
		case cPropertyMyProperty:
			//myProperty = fVal.getLong();
			break;
	}
    
	return 1L;
}

/**************************************************************************************************
 **                                        STATIC METHODS                                        **
 **************************************************************************************************/

/* METHODS */

// Table of parameter resources and types.
// Note that all parameters can be stored in this single table and the array offset can be  
// passed via the MethodsTable.
//
// Columns are:
// 1) Name of Parameter (Resource #)
// 2) Return type (fft value)
// 3) Parameter flags of type EXTD_FLAG_xxxx
// 4) Extended flags.  Documentation states, "Must be 0"
ECOparam cFileCopyWorkerMethodsParamsTable[] = 
{
	4900, fftInteger  , 0, 0,
	4901, fftCharacter, 0, 0,
	4902, fftCharacter, 0, 0,
	4903, fftCharacter, 0, 0
};

// Table of Methods available for Simple
// Columns are:
// 1) Unique ID 
// 2) Name of Method (Resource #)
// 3) Return Type 
// 4) # of Parameters
// 5) Array of Parameter Names (Taken from MethodsParamsTable.  Increments # of parameters past this pointer) 
// 6) Enum Start (Not sure what this does, 0 = disabled)
// 7) Enum Stop (Not sure what this does, 0 = disabled)
ECOmethodEvent cFileCopyWorkerMethodsTable[] = 
{
	cMethodError,      cMethodError,      fftNumber,  4, &cFileCopyWorkerMethodsParamsTable[0], 0, 0,
	cMethodInitialize, cMethodInitialize, fftBoolean, 0,                               0, 0, 0,
    cMethodRun,        cMethodRun,        fftNone,    0,                               0, 0, 0,
    cMethodStart,      cMethodStart,      fftNone,    0,                               0, 0, 0,
    cMethodCancel,     cMethodCancel,     fftNone,    0,                               0, 0, 0,
	cMethodDone,	   cMethodDone,       fftNone,    0,                               0, 0, 0
};

// List of methods in Simple
qlong NVObjWorker::returnMethods(tThreadData* pThreadData)
{
	const qshort cMethodCount = sizeof(cFileCopyWorkerMethodsTable) / sizeof(ECOmethodEvent);
	
	return ECOreturnMethods( gInstLib, pThreadData->mEci, &cFileCopyWorkerMethodsTable[0], cMethodCount );
}

/* PROPERTIES */

// Table of properties available from Simple
// Columns are:
// 1) Unique ID 
// 2) Name of Property (Resource #)
// 3) Return Type 
// 4) Flags describing the property
// 5) Additional Flags describing the property
// 6) Enum Start (Not sure what this does, 0 = disabled)
// 7) Enum Stop (Not sure what this does, 0 = disabled)
ECOproperty cFileCopyWorkerPropertyTable[] = 
{
	cPropertyMyProperty, cPropertyMyProperty, fftInteger, EXTD_FLAG_PROPCUSTOM, 0, 0, 0 /* Shows under Custom category */
};

// List of properties in Simple
qlong NVObjWorker::returnProperties( tThreadData* pThreadData )
{
	const qshort propertyCount = sizeof(cFileCopyWorkerPropertyTable) / sizeof(ECOproperty);
    
	return ECOreturnProperties( gInstLib, pThreadData->mEci, &cFileCopyWorkerPropertyTable[0], propertyCount );
}

/**************************************************************************************************
 **                       THREAD TIMER NOTIFIER                                                  **
 **************************************************************************************************/

int NVObjWorker::notify() 
{        
    if(_worker->complete()) {
        // Worker completed.  Call back into Omnis
        EXTfldval retVal;
        Worker::ParamMap pm = _worker->result();
        getRowFromParams(retVal, pm);
        
        str255 methodName("$done");
        ECOdoMethod( this->getInstance(), &methodName, &retVal, 1 );
        
        return ThreadTimer::kTimerStop;
    } else if (_worker->cancelled()) {
        str31 methodName(initStr31("$canceled"));
        ECOdoMethod( this->getInstance(), &methodName, 0, 0 );
        
        return ThreadTimer::kTimerStop;
    }
    
    return ThreadTimer::kTimerContinue;
}

/**************************************************************************************************
 **                              CUSTOM (YOUR) METHODS                                           **
 **************************************************************************************************/

// Initialize the worker
tResult NVObjWorker::methodInitialize( tThreadData* pThreadData, qshort pParamCount )
{    
    EXTfldval rowVal;
    if (getParamVar(pThreadData,1,rowVal) == qfalse) {
        pThreadData->mExtraErrorText = "1st parameter must be a row of parameters";
        return ERR_METHOD_FAILED;
    }
    
    // Convert row into parameters
    Worker::ParamMap params;
    if( getParamsFromRow(pThreadData, rowVal, params) == false) {
        pThreadData->mExtraErrorText = "1st parameter must be a row of parameters";
        return ERR_METHOD_FAILED;
    }
    
    // Create new worker object
    _worker = boost::make_shared<Worker>(params,boost::make_shared<cppnetlibDelegate>());
    
    // Call all worker initialization code while on main thread
    _worker->init();
    
    EXTfldval retVal;
    getEXTFldValFromBool(retVal,true);
    ECOaddParam(pThreadData->mEci, &retVal);
	
	return METHOD_DONE_RETURN;
}

tResult NVObjWorker::methodRun( tThreadData* pThreadData, qshort pParamCount )
{
    if( _worker == boost::shared_ptr<Worker>() ) {
        return ERR_METHOD_FAILED;
    }
    
    _worker->run();  // Run worker function object
    
    // Manually call notify since this is single-threaded
	_worker->setComplete(true);
    notify();
    
	return METHOD_DONE_RETURN;
}

tResult NVObjWorker::methodStart( tThreadData* pThreadData, qshort pParamCount )
{
    if( _worker == boost::shared_ptr<Worker>() ) {
        return ERR_METHOD_FAILED;
    }
    
    // Initiate timer to watch for finished events
    ThreadTimer& timerInst = ThreadTimer::instance();
    timerInst.subscribe(this);
    
    _worker->start();  // Run background thread
    
	return METHOD_DONE_RETURN;
}

tResult NVObjWorker::methodCancel( tThreadData* pThreadData, qshort pParamCount )
{
    if( _worker == boost::shared_ptr<Worker>() ) {
        return ERR_METHOD_FAILED;
    }
    
    _worker->cancel();  // Attempt to cancel worker
    
	return METHOD_DONE_RETURN;
}