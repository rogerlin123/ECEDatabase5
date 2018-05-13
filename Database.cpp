//
//  Database.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "Parser.hpp"
#include "CreateTableStatement.hpp"
#include "Tokenizer.hpp"
#include "Schema.hpp"
#include "SQLInterpreter.hpp"
#include <iostream>

namespace SF {
  
  
  Database::Database(const std::string aName) : name(aName), storage(aName) {
  }
  
  Database::Database(const Database &aCopy) : name(aCopy.name), storage(aCopy.storage) {
  }
  
  Database::~Database() {
    for(auto theSchema : schemas) {
      if(theSchema->dirty) {
        theSchema->markDirty(false);
        Block theBlock=(*theSchema); //encode schema to block...
        storage.writeBlock(theSchema->blockId, theBlock);
      }
      delete theSchema;
    }
  }
    

  StatusResult Database::initializeStorage(bool reset /*=false*/) {
    if(reset) {
      return storage.makeEmpty();
    }
    return storage.loadMaster();
  }
  
  bool Database::tableExists(const std::string aName) {
    Block theBlock;
    return storage.getSchemaBlock(aName,theBlock);
  }
  

  //given the schema, create an associated table, allocate storage, update the DB...
  StatusResult Database::createTable(Schema &aSchema) {
    
    Block temp;
    if(storage.getSchemaBlock(aSchema.getName(),temp)) {
      return StatusResult{false,gTableExists};
    }
    
    Block theBlock=aSchema; //encode schema to block...
    StatusResult theResult= storage.addSchema(aSchema.getName(), theBlock);
    if(theResult) {
      Schema *theSchema = new Schema(aSchema);
      theSchema->blockId=theBlock.header.id;
      schemas.push_back(theSchema);
    }
    return theResult;
  }
  
  //CALLER owns the schema returned; make sure you delete it!
  Schema* Database::getSchema(const std::string aName) {
    
    //try to pull from live cache...
    for(auto theSchema : schemas) {
      if(theSchema->name==aName) {
        return theSchema;
      }
    }
    
    Block theBlock;
    if(uint32_t theBlockId=storage.getSchemaBlock(aName,theBlock)) {
      Schema *theSchema = new Schema(theBlock, theBlockId);
      schemas.push_back(theSchema);
      return theSchema;
    }
    return nullptr;
  }

  
  StatusResult Database::describeTable(std::string aTableName) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{false, gUnknownTable};
  }
  
  StatusResult Database::showTables() {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{false, gUnknownTable};
  }
  
  //Add a new row into DB using given data and schema...
  StatusResult Database::insertRow(Schema &aSchema, KeyValueList &aList) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{false, gUnknownTable};
  }
  
  StatusResult Database::updateRow(Schema &aSchema, Filters &aFilters, KeyValueList &aList) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{true};
  }
  
  
  StatusResult Database::selectRows(RowCollection &aCollection, Schema &aSchema, Filters &aFilters) {
    
    //------------------------------------
    // Your code here...
    //------------------------------------
    
    StatusResult theResult{true};
    return theResult;
  }
  
  StatusResult Database::reorder(RowCollection &aCollection, PropertyList &anOrder) {
    if(anOrder.size()) {
      //------------------------------------
      // Your code here...
      //------------------------------------
    }
    return StatusResult{true};
  }
  
  //oh look! - deleteRows uses existing features, so you get it for free...
  StatusResult Database::deleteRows(RowCollection &aCollection, Schema &aSchema, Filters &aFilters) {
    StatusResult theResult=selectRows(aCollection, aSchema, aFilters);
    if(theResult) {
      RowList &theRows=aCollection.getRows();
      for(auto theRow : theRows) {
        theResult=storage.releaseBlock(theRow->getBlockNumber()); //free up the block...
      }
    }
    return StatusResult{true};
  }
  
  
  StatusResult Database::dropTable(std::string aTableName) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{true};
  }
  
  //Enable class to be part of ICommandable chain...
  StatusResult Database::handleCommand(std::string &aUserInput,
                                       ICommandable *aNext) {
    Tokenizer theTokenizer(aUserInput);
    StatusResult theResult=theTokenizer.tokenize();
    
    if(theResult) {
      SQLInterpreter theInterpreter(*this);
      theResult=theInterpreter.runSQL(theTokenizer);
    }
    
    return theResult;
  }
  
}

