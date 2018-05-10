//
//  DBManager.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "DBManager.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"
#include <iostream>

namespace SF {
  
  DBManager DBManager::manager;
  
    DBManager::DBManager(){
    active=nullptr;
  }
  
  DBManager::~DBManager() {
    releaseDB();
  }
  
  DBManager& DBManager::releaseDB() {
    if(active) {
      delete active;
      active=nullptr;
    }
    return *this;
  }
  
  Database* DBManager::findDatabase(const std::string aName) {
    char thePath[128];
    sprintf(thePath,"/tmp/%s.%s",aName.c_str(),"db");
    
    if (FILE *file = fopen(thePath, "r")) {
      fclose(file);
      Database *theDB=new Database(aName);
      return theDB;
    }
    return nullptr;
  }
  
  StatusResult DBManager::createDatabase(const std::string aName) {
    active=releaseDB().findDatabase(aName);
    if(!active) {
      active=new Database(aName);
      return active->initializeStorage(true);
    }
    return StatusResult{true};
  }
  
  StatusResult DBManager::dropDatabase(const std::string aName) {
    //TBD...
    return StatusResult(true);
  }
  
  StatusResult DBManager::showDatabases() {
    
    //---------------------------------------------------------------
    //TBD: show properties of all known DBs...
    //---------------------------------------------------------------
    
    return 0;
  }
  
  StatusResult DBManager::useDatabase(const std::string aName) {
    if(Database *theDB=findDatabase(aName)) {
      active=theDB;
      theDB->initializeStorage(false);
      std::cout << "Using DB " << aName << std::endl;
      return StatusResult{true};
    }
    return StatusResult{false,gUnknownDatabase};
  }
  
  StatusResult DBManager::handleCommand(std::string &aUserInput, ICommandable *aNext) {
    
    Tokenizer theTokenizer(aUserInput);
    StatusResult theResult=theTokenizer.tokenize();
    
    if(theResult) {
      Token theToken = theTokenizer.tokenAt(0);
      Keywords theKW=theTokenizer.getKeywordId(theToken.data);
      
      Token theToken1 = theTokenizer.tokenAt(1);
      Keywords theType=theTokenizer.getKeywordId(theToken1.data);
      
      theResult=StatusResult{false};
      switch(theKW) {
        case Keywords::create_kw:
          
          if(Keywords::database_kw==theType) {
            Token theNameToken = theTokenizer.tokenAt(2);
            return createDatabase(theNameToken.data);
          }
          else if(Keywords::table_kw==theType) {
            if(active) {
              return active->handleCommand(aUserInput);
            }
            return StatusResult(false,gNoDatabaseSpecified);
          }
          
          break;
          
        case Keywords::drop_kw:
          {
            Token theNameToken = theTokenizer.tokenAt(2);
            return dropDatabase(theNameToken.data);
          }
          break;
          
        case Keywords::show_kw:
            if(Keywords::databases_kw==theType) {
              return showDatabases();
            }
            else {
              if(active) {
                return active->handleCommand(aUserInput);
              }
              return StatusResult(false,gNoDatabaseSpecified);
            }
            break;
          
        case Keywords::use_kw:
            {
              Token theDBName = theTokenizer.tokenAt(2);
              return useDatabase(theDBName.data);
            }
            break;
          
        default:
          break;
      };
      
      if(active) {
        theResult= active->handleCommand(aUserInput);
      }
      
      if(!theResult && aNext) {
        return aNext->handleCommand(aUserInput);
      }
    }
    
    return theResult;
  }
  
  //-------------------------
  
  DBManager& DBManager::getManager() {return manager;}
  
}
