//
//  Schema.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Attribute.hpp"
#include "UserValue.hpp"
#include "Row.hpp"

namespace SF {
  
  class CreateTableStatement;
  class Block;
  class Database;
  
  class Schema {
    
    std::vector<Attribute>  attributes;
    std::string             name;
    uint16_t                count; //number of records 
    uint32_t                autoincr; //current auto_increment value (# of records created)...
    uint32_t                blockId;  //storage location.
    uint32_t                hash;     //used temporally
    bool                    dirty;
    
  public:
    
    Schema(std::string aName);
    Schema(Block &aSchemaBlock , uint32_t aBlockId=0);
    Schema(const Schema &aCopy);
    
    Schema(const CreateTableStatement &aStatement); //conversion constructor...
    
    ~Schema();
    
    operator Block();

    std::string&  getName() {return name;}
    uint32_t      getHash() const {return hash;}
    
    std::vector<Attribute>& getAttributes() {return attributes;}
    
    bool          describeTabular(std::ostringstream &aStream); //describes the table

    Schema&       markDirty(bool aState);
    
    Schema&       addAttribute(std::string aName, Keywords aType, uint8_t aSize=0, bool autoIncr=false);
    Schema&       addAttribute(const Attribute &anAttribute);
    
    Attribute&    getAttribute(std::string &aName);
  
    StatusResult  hasCompatibleAttribute(const std::string &aName, UserValue &aValue) const;    
    StatusResult  validate(KeyValueList &aList);
    
    StatusResult  encodeBlock(Block &aBlock, KeyValueList &aList); 
    StatusResult  decodeBlock(Block &aBlock, KeyValueList &aList);
    
    friend class Database;
    
  };
}

#endif /* Schema_hpp */
