#include "common.he"

/**************************************************************************************************
 **                              PARAM CONVERSION                                                **
 **************************************************************************************************/

bool getParamsFromRow(tThreadData* pThreadData, EXTfldval& row, Worker::ParamMap& params) {
    
    if(getType(row).valType != fftRow && getType(row).valType != fftList) {
        return false;
    }
    
    // Convert row into more appropriate variables for parameters
    EXTfldval colVal, colTitleVal;
    EXTqlist rowData;
    row.getList(&rowData, qfalse);
    for( qshort col = 1; col <= rowData.colCnt(); ++col) {
        str255 colName;
        rowData.getCol(col, qfalse, colName);
        colTitleVal.setChar(colName);
        
        rowData.getColValRef(1, col, colVal, qfalse);
        
        // Get column definition type
        ffttype fft;
        qshort fdp;
        rowData.getColType( col, fft, fdp );
        
        // Assign map based on definition
        switch (fft) {
            case fftCharacter:
                params[getStringFromEXTFldVal(colTitleVal)] = getStringFromEXTFldVal(colVal);
                break;
            case fftInteger:
                params[getStringFromEXTFldVal(colTitleVal)] = getIntFromEXTFldVal(colVal);
                break;
            case fftNumber:
                params[getStringFromEXTFldVal(colTitleVal)] = getDoubleFromEXTFldVal(colVal);
                break;
            case fftBoolean:
                params[getStringFromEXTFldVal(colTitleVal)] = getBoolFromEXTFldVal(colVal);
                break;
			case fftList:
                params[getStringFromEXTFldVal(colTitleVal)] = colVal.getList(qtrue);
				break;
            default:
                LOG_DEBUG << "Unknown column type when converting parameters.";
                break;
        }
    }    
    
    return true;
}

bool getRowFromParams(EXTfldval& row, Worker::ParamMap& params) {
    
    Worker::ParamMapIterator it;
    str255 colName;
    EXTfldval colVal;
    EXTqlist* retList = new EXTqlist(listVlen); // Return row
    
    // Add all output columns
    colName = initStr255("Result");
    retList->addCol(fftRow, dpDefault, 0, &colName);
    
    retList->insertRow();
    
    // Look for output data
    it = params.find("Result");
    if( it != params.end()) {
        retList->getColValRef(1,1,colVal,qtrue);
        
        try {
            boost::shared_ptr<EXTqlist> ptr = boost::any_cast<boost::shared_ptr<EXTqlist> >(it->second);
            colVal.setList(ptr.get(), qtrue); 
        } catch( const boost::bad_any_cast& e ) {
            LOG_ERROR << "Unable to cast return value from PostgreSQL worker.";
        }
    }
    
    row.setList(retList,qtrue);
    
    return true;
}

EXTqlist* getListFromParams(Worker::ParamMap& params, std::string paramName)
{
	EXTqlist* pList;
    Worker::ParamMapIterator it = params.find(paramName);
    if( it == params.end()) {
        LOG_ERROR << "No " << paramName << " list";
        return NULL;
    }
    try {
        pList = boost::any_cast<EXTqlist*>(it->second);
    } catch (const boost::bad_any_cast& e) {
        LOG_ERROR << "Unable to get list parameter. Error: " << e.what();
        return NULL;
    }
	return pList;
}


BOOL FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string getStringFromParam(Worker::ParamMap& params, std::string paramName)
{
    std::string command;
    Worker::ParamMapIterator it = params.find(paramName);
    if( it == params.end()) {
        LOG_ERROR << paramName << "not found.";
        return command;
    }
    try {
        command = boost::any_cast<std::string>(it->second);
    } catch (const boost::bad_any_cast& e) {
        LOG_ERROR << "Unable to convert query parameter to string. Error: " << e.what();
        return command;
    }
	return command;
}

