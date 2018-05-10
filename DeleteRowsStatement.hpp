//
//  InsertStatement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/21/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef DeleteRowsStatement_hpp
#define DeleteRowsStatement_hpp

#include <stdio.h>
#include "SelectStatement.hpp"
#include "Attribute.hpp"
#include "Keywords.hpp"
#include "Parser.hpp"
#include <vector>
#include <string>

namespace SF {
  
  class Schema;
  class SQLInterpreter;
  
  class DeleteRowsStatement : public SelectStatement {
  protected:
    std::vector<Property> fields;
    std::vector<Token>    values;  //each is a collection of named-uservalues...
    std::string           tableName;
    
  public:
                DeleteRowsStatement();
                DeleteRowsStatement(const DeleteRowsStatement &aCopy);
    virtual     ~DeleteRowsStatement();
    
                StatusResult  parseValues(Parser &aParser, size_t aCount);
    virtual     StatusResult  parse(Parser &aParser);
    
    virtual     StatusResult  validate(Schema &aSchema) const;
    
    std::string getTableName() const {return tableName;}
  };
  
}
#endif /* DeleteRowsStatement_hpp */
