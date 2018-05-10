//
//  Filters.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Filters_hpp
#define Filters_hpp

#include <stdio.h>
#include "UserValue.hpp"
#include "Attribute.hpp"
#include <vector>

namespace SF {
  
  class Parser;
  
  struct Expression {
    Property    key;
    UserValue   value;
    Operators   op;
    
    Expression(Property aKey, UserValue aValue, Operators anOp) :
      key(aKey), value(aValue), op(anOp) {}
    
    Expression(std::string aKey, UserValue aValue, Operators anOp ) :
      key(Property(aKey)), value(aValue), op(anOp) {}
    
  };
  
  class Filters {
  protected:
    std::vector<Expression*> expressions;
    
  public:
    
    Filters();
    Filters(const Filters &aFilters);
    ~Filters();
    
    //KeyValueList& getFilters() {return filters;}
    StatusResult  parse(Parser &aParser);
  };

}

#endif /* Filters_hpp */
