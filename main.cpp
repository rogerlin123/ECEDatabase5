//
//  main.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "CommandProcessor.hpp"

int main() {

  /*
  AVLTree<int,int> theTree;
  
  theTree.insert(10,10);
  theTree.insert(20,20);

  int theValue{0};
  if(theTree.find(20,theValue)) {
    std::cout << "found value[20] " << theValue << std::endl;
  }
*/
  
  return SF::CommandProcessor::run();
}

