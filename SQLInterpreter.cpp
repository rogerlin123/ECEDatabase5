//
//  SQLInterpreter.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//


#include "SQLInterpreter.hpp"
#include "Database.hpp"
#include "Statement.hpp"
#include "CreateTableStatement.hpp"
#include "InsertStatement.hpp"
#include "SelectStatement.hpp"
#include "DeleteRowsStatement.hpp"
#include "Schema.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include <iostream>

namespace SF {
  
  SQLInterpreter::SQLInterpreter(Database &aDatabase) : database(aDatabase) {}
  SQLInterpreter::~SQLInterpreter() {}

  
    //hidden method to run "insert into..." sql command...
  StatusResult runInsert(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>7) {
      Token theToken = aTokenizer.tokenAt(1);
      if(Keywords::into_kw==aTokenizer.getKeywordId(theToken.data)) {
        Token theToken = aTokenizer.tokenAt(2);

        Schema* theSchema = aDatabase.getSchema(theToken.data);
        if(!theSchema) {
          return StatusResult{false,gUnknownTable};}
        
        Parser theParser(aTokenizer);
        theParser.skip(2);
        
        InsertStatement theStatement(theSchema->getName());
        StatusResult theResult=theStatement.parse(theParser, *theSchema);
        
        if(theResult) {
          theResult=theStatement.validate(*theSchema);
          if(theResult) {
            
              //Insert a record for each user-given valuegroup...
            size_t theCount=theStatement.getValueGroupCount();
            for(int i=0; theResult, i<theCount; i++) {
              if(auto theValues = theStatement.getValueGroupAt(i)) {
                theResult=aDatabase.insertRow(*theSchema, *theValues);
              }
            }
          }
        }
        return theResult;

      }
    }
    return StatusResult{false};
  }
  
  //hidden method to run "create table" sql command...
  StatusResult runCreateTable(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>6) {
      Token theToken = aTokenizer.tokenAt(1);
      if(Keywords::table_kw==aTokenizer.getKeywordId(theToken.data)) {
        Token theToken = aTokenizer.tokenAt(2);
        if(!aDatabase.tableExists(theToken.data)) {
          Parser theParser(aTokenizer);
          theParser.skip(2);
          
          CreateTableStatement theStatement(theToken.data);
          StatusResult theResult=theStatement.parse(theParser);
          if(theResult) {
            Schema theSchema(theStatement); //convert statement to schema, then store in the db...
            return aDatabase.createTable(theSchema);
          }
        }
        return StatusResult{false,gTableExists};
      }
    }
    return StatusResult{false,gInvalidCommand};
  }
  
  //hidden method to run "select * from {table} where..." sql command...
  StatusResult runSelectRows(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>3) {
      Parser theParser(aTokenizer);
      SelectStatement theStatement{};
      StatusResult theResult=theStatement.parse(theParser);

      if(theResult) {
        Schema* theSchema=aDatabase.getSchema(theStatement.getTableName());
        if(theSchema) {
          RowCollection theRows;
          aDatabase.selectRows(theRows, *theSchema, theStatement.getFilters());
          aDatabase.reorder(theRows, theStatement.getOrderBy()); //ignore GROUPBY for now...
          
          //-----------------------------------------------
          // TODO: Add your logic to show the rows...
          //------------------------------------------------

        }
      }
    }
    return StatusResult{false};
  }

  StatusResult SQLInterpreter::runSQL(Tokenizer &aTokenizer) {
    
    Token theToken = aTokenizer.tokenAt(0);
    Keywords theKW=aTokenizer.getKeywordId(theToken.data);
    switch(theKW) {
      case Keywords::create_kw: //create table...
        return runCreateTable(aTokenizer, database);
        break;
        
      case Keywords::alter_kw:  //alter table...
        break;
        
      case Keywords::drop_kw:   //drop table...
        break;
        
      case Keywords::insert_kw:  //insert record(s)...
        return runInsert(aTokenizer, database);
        break;
        
      case Keywords::update_kw:  //update record(s)...
        break;
        
      case Keywords::delete_kw:  //delete records(s)...
        break;
        
      case Keywords::select_kw:  //select records(s)...
        return runSelectRows(aTokenizer, database);
        break;

      case Keywords::describe_kw:
        {
          Token theDBName = aTokenizer.tokenAt(1);
          database.describeTable(theDBName.data);
        }
        break;

      case Keywords::show_kw:
        database.showTables();
        break;
        
      default:
        break;
    };
    
    return StatusResult{false}; //we didn't handle it...
  }
  
}
