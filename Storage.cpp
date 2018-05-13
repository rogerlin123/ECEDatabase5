//
//  Storage.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/1/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <iostream>

namespace SF {
  
  const int gMultiplier = 37;
  
  uint32_t Storage::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }
  
  Storage::Storage(const std::string aName) : master('M',0), name(aName), lastWriteBlockNum(0) {
    char thePath[128];
    lastWriteBlockNum=0;
    sprintf(thePath,"/tmp/%s.%s",aName.c_str(),"db");
    stream.open (thePath, std::fstream::binary | std::fstream::in | std::fstream::out);

    master.header.count=0; //init for new db...
    memset(master.data,0,sizeof(master.data));    
  }
  
  Storage::Storage(const Storage &aCopy) : name(aCopy.name), lastWriteBlockNum(aCopy.lastWriteBlockNum) {
    lastWriteBlockNum=aCopy.lastWriteBlockNum;
  }
  
  Storage::~Storage() {
    master.header.dirty=false;
    writeBlock(0,master);
    stream.close();
  }
  
  uint32_t Storage::getSchemaBlock(const std::string &aName, Block &aBlock) {
    uint32_t theHash = Storage::hashString(aName.c_str());
    for(int i=0; i<master.header.count; i++) {
      if(master.schemas[i].hashId==theHash) {
        if(readBlock(master.schemas[i].blockId, aBlock)) {
          return master.schemas[i].blockId;
        }
      }
    }
    return 0; //return 0 means schema block not found...
  }

  StatusResult Storage::loadMaster() {
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    return Storage::readBlock(0, master);
  }
  
  StatusResult Storage::makeEmpty() {
    char thePath[128];
    sprintf(thePath,"/tmp/%s.%s",name.c_str(),"db");
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
    return addBlock(master);
  }
  
  bool Storage::isReady() {
    return stream.is_open();
  }
  
  uint32_t Storage::getAutoIncrement() {
    return ++master.header.id;
  }
  
  int Storage::getTotalBlockCount() {
    stream.seekg(stream.tellg(), std::ios::beg); //force read mode; dumb c++ issue...
    stream.seekg(0, std::ios::end);
    int thePos = (int)stream.tellg();
    return thePos / kBlockSize;
  }
  
  //scan archive for free blocks, and store them here. If you need more use block number for appended blocks...
  int Storage::getFreeBlock() {
    if(master.header.id) {
      //we have a free block...
    }
    return 0;
  }
  
  StatusResult Storage::readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize) {
    stream.seekg(aBlockNumber * kBlockSize);
    if(stream.read ((char*)&aBlock, aBlockSize)) {
      return StatusResult{true};
    }
    return StatusResult(false, gReadError);
  }
  
    //assumes seekp has already been moved...
  StatusResult write(Block &aBlock, std::fstream &aStream) {
    if(aStream.write ((char*)&aBlock, kBlockSize)) {
      aStream.flush();
      return StatusResult{true};
    }
    return StatusResult{false, gWriteError};
  }

  StatusResult Storage::writeBlock(int aBlockNumber, Block &aBlock) {
    lastWriteBlockNum=aBlockNumber;
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(aBlockNumber * kBlockSize);
    return write(aBlock, stream);
  }
  
  StatusResult Storage::addBlock(Block &aBlock) {
    if(int theBlockNum = getFreeBlock()) {
      return writeBlock(theBlockNum, aBlock);
    }
    
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(0, std::ios::end); //append...

    master.header.dirty=true; //TEST ONLY
    lastWriteBlockNum=getTotalBlockCount();
    
    return write(aBlock, stream);
  }

    //mark associated block as FREE...
  StatusResult Storage::releaseBlock(int aBlockNumber) {
    //------------------------------------------------
    // YOUR CODE HERE... (mark block free, track it?)
    //------------------------------------------------
    return StatusResult{false,gNotImplemented};
  }
  
  StatusResult Storage::addSchema(const std::string &aName, Block &aBlock) {
    if(addBlock(aBlock)) {
      master.schemas[master.header.count].hashId=Storage::hashString(aName.c_str());
      master.schemas[master.header.count++].blockId=lastWriteBlockNum;
      master.header.dirty=true;
      return StatusResult{true};
    }
    return StatusResult{false,gStorageFull};
  }
  
  StatusResult Storage::dropSchema(const std::string &aName) {
    return StatusResult{false};
  }
  
}

