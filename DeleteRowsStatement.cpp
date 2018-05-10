//
//  DeleteRowsStatement.cpp
//  ECEDatabase4
//
//  Created by rick gessner on 5/2/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "DeleteRowsStatement.hpp"

namespace SF {
  
  
  DeleteRowsStatement::DeleteRowsStatement() : SelectStatement() {
  }
  
  DeleteRowsStatement::DeleteRowsStatement(const DeleteRowsStatement &aCopy)
    : SelectStatement(aCopy) {
  }
  
  DeleteRowsStatement::~DeleteRowsStatement() {
  }
  
  //delete from  TABLE where k1=v1, k2=v2...
  StatusResult DeleteRowsStatement::parseValues(Parser &aParser, size_t aCount) {
    StatusResult theResult{true};
    
    Token theToken{};
    while(aParser.peekToken(theToken)) {
      if(TokenType::lparen==theToken.type) {
        aParser.skip(1);
        std::vector<Token> theTokens;
        theResult=parseCommaSepList(aParser, theTokens, TokenType::rparen);
      }
    }
    
    return theResult;
  }
  
  StatusResult DeleteRowsStatement::parse(Parser &aParser) {
    
    //we've already consumed... "insert into {name}"...
    StatusResult theResult={true};
    if(theResult) {
      Token theToken{};
      if(aParser.peekToken(theToken)) {
        if(Keywords::values_kw==theToken.keyword) {
          aParser.skip(1);
          theResult=parseValues(aParser, fields.size());
        }
      }
    }
    return theResult;
  }
  
  //validating this statement implies validating field and argument lists...
  StatusResult DeleteRowsStatement::validate(Schema &aSchema) const {
    
    if(values.size()) {
      return StatusResult{true};
    }
    
    return StatusResult{false, gInvalidArguments}; //needs to be exact multiple...
  };
  
}
