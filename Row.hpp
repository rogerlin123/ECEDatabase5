//
//  Row.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include "UserValue.hpp"

namespace SF {
  
  class Schema;
  class Block;
  
  class Row {
  protected:
    int32_t       blockNumber;
    KeyValueList  columns;
    
  public:
    Row(int32_t aBlockNumber=0);
    Row(const Row &aRow);
    ~Row();
    
    KeyValueList& getColumns() {return columns;}
    
    friend class RowCollection;
  };
  
  typedef std::vector<Row*> RowList;
  
  class RowCollection {
  protected:
    RowList rows;
  public:
    RowCollection();
    RowCollection(const RowCollection &aCopy);
    ~RowCollection();
    
    RowList&          getRows() {return rows;}
    
    RowCollection&    add(Row* aRow);
  };
  
}

#endif /* Row_hpp */
//
//  Row.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include "UserValue.hpp"

namespace SF {
  
  class Schema;
  class Block;
  
  class Row {
  protected:
    int32_t       blockNumber;
    KeyValueList  columns;
    
  public:
    Row(int32_t aBlockNumber=0);
    Row(const Row &aRow);
    ~Row();
    
    KeyValueList& getColumns() {return columns;}
    
    friend class RowCollection;
  };
  
  typedef std::vector<Row*> RowList;
  
  class RowCollection {
  protected:
    RowList rows;
  public:
    RowCollection();
    RowCollection(const RowCollection &aCopy);
    ~RowCollection();
    
    RowList&          getRows() {return rows;}
    
    RowCollection&    add(Row* aRow);
  };
  
}

#endif /* Row_hpp */
