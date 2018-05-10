//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "Storage.hpp"
#include "Tokenizer.hpp"
#include "CommandProcessor.hpp"
#include "Schema.hpp"
#include "UserValue.hpp"
#include "Filters.hpp"
#include "Row.hpp"

namespace SF {
  
  class Database : public ICommandable {
    
    std::string           name;
    Storage               storage;
    std::vector<Schema*>  schemas;
    
  public:
    
    Database(const std::string aPath);    
    Database(const Database &aDatabase);
    
    ~Database();
    
    std::string&  getName() {return name;}
    
    bool          tableExists(const std::string aName);
    Schema*       getSchema(const std::string aName);
    
    StatusResult  initializeStorage(bool reset=false);
    
    StatusResult  createTable(Schema &aSchema);
    StatusResult  describeTable(std::string aTableName);
    StatusResult  dropTable(std::string aTableName);
    StatusResult  showTables();
    
    StatusResult  selectRows(RowCollection &aCollection, Schema &aSchema, Filters &aFilters);
    StatusResult  deleteRows(Schema &aSchema, Filters &aFilters);
    StatusResult  insertRow(Schema &aSchema, KeyValueList &aList);
    StatusResult  updateRow(Schema &aSchema, Filters &aFilters, KeyValueList &aList);

    StatusResult  reorder(RowCollection &aCollection, PropertyList &anOrder);

    virtual StatusResult handleCommand(std::string &aUserInput, ICommandable *aNext=nullptr);
    
  };
  
}

#endif /* Database_hpp */
