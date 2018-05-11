//
//  Schema.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Schema.hpp"
#include "CreateTableStatement.hpp"
#include "Storage.hpp"
#include "Row.hpp"
#include "MemStream.hpp"
#include <sstream>

namespace SF {
  
  Schema::Schema(std::string aName)
    : name(aName), attributes(), dirty(false), count(0), blockId(0) {
    hash = Storage::hashString(aName.c_str());
  }
  
  Schema::Schema(const Schema &aCopy) :
    name(aCopy.name), attributes(aCopy.attributes), dirty(aCopy.dirty),
    count(aCopy.count), blockId(aCopy.blockId), hash(aCopy.hash) {
  }

  Schema::Schema(Block &aSchemaBlock, uint32_t aBlockId) {
    name=aSchemaBlock.attributes[0].name;
    hash = Storage::hashString(name.c_str());
    count=aSchemaBlock.header.count;
    autoincr=aSchemaBlock.header.id;
    blockId=aBlockId;
    
    for(int i=1; i<=aSchemaBlock.header.count;i++) {
      AttributeDef theDef=aSchemaBlock.attributes[i];
      attributes.push_back(Attribute(theDef));
    }
  }
  
  //conversion constructor...
  Schema::Schema(const CreateTableStatement &aStatement) :
    name(aStatement.getTableName()), attributes(aStatement.attributes) {
      count=attributes.size();
      hash = Storage::hashString(name.c_str());
      blockId=0;
      dirty=true;
  }
  
  Schema::~Schema() {
  }
  
  Schema& Schema::markDirty(bool aState) {
    dirty=aState;
    return *this;
  }
    
  bool Schema::describeTabular(std::ostringstream &aStream) {
    
    //------------------------------------------------
    // TODO: Describe tablular data
    //------------------------------------------------

    return true;
  }
  
  Schema& Schema::addAttribute(std::string aName, Keywords aType, uint8_t aSize, bool autoIncr) {
    attributes.push_back(Attribute(aName, aType, aSize));
    return *this;
  }
  
  Schema& Schema::addAttribute(const Attribute &anAttribute) {
    attributes.push_back(anAttribute);
    return *this;
  }
  
  Attribute& Schema::getAttribute(std::string &aName) {
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      if(theIter->getName()==aName) {
        return *theIter;
      }
    }
    throw std::runtime_error("Attribute not found");
  }

   //pack this schema into a storage block...
  Schema::operator Block() {
    Block theBlock{'S',0}; //0 because location is unknown...
    int   theIndex=1;
    
    strcpy(theBlock.attributes[0].name, name.c_str());
    theBlock.attributes[0].type='S'; //schema...
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      theBlock.attributes[theIndex++]=(*theIter);
    }
    theBlock.header.count=(uint32_t)attributes.size();
    theBlock.header.id=autoincr; //save current auto-incr value...

    return theBlock;
  }
  
  StatusResult Schema::hasCompatibleAttribute(const std::string &aName, UserValue &aValue) const {
    for (auto theAttribute : attributes) {
      if(theAttribute.getName()==aName) {
        if(theAttribute.isCompatible(aValue.type)) {
          return StatusResult{true};
        }
        return StatusResult{false, gInvalidAttribute};
      }
    }
    return StatusResult{false, gInvalidAttribute};
  }

  StatusResult Schema::validate(KeyValueList &aList) {
    for (auto theKV : aList) {
      if(!hasCompatibleAttribute(theKV.first, theKV.second)) {
        return StatusResult{false, gInvalidAttribute};
      }
    }
    return StatusResult{true};
  }

  //Init datablock from a key/value list; set id; FORMERLY: updateDataBlock()...
  StatusResult Schema::encodeBlock(Block &aBlock, KeyValueList &aList) {
    aBlock.header.type='D';
    aBlock.header.dirty=true;
    aBlock.header.count=hash;    //store here for manual table search...
    aBlock.header.id=++autoincr; //record number
    
    //--------------------------------------------------------
    //now copy given data (list) into data section of block...
    //--------------------------------------------------------
    
    markDirty(true); //autoincr changed...
    
    StatusResult theResult{true};
    return theResult;
  }
  
  StatusResult Schema::decodeBlock(Block &aBlock, KeyValueList &aList) {
    
    //----------------------------------------------------------
    //Copy block data (list) into data given list of key/values
    //----------------------------------------------------------
    
    return StatusResult{true};
  }
  


}
