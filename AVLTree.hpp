//
//  AVLTree.hpp
//  ECEDatabase5
//
//  Added by rick gessner on 4/27/18.
//  From version by Domenico Porcino
//

#ifndef AVLTree_h
#define AVLTree_h

const int kMaxDepth =32;

template<class KeyType, class ItemType>
class AVLTree {
protected:

  struct Node {
    Node(KeyType aKey, ItemType aValue) :
      balance(0), depth(0), key(aKey), value(aValue), left(0), right(0) {}
    
    KeyType  key;
    ItemType value;
    short    balance;
    short    depth;

    Node* left;
    Node* right;
  };
  
  Node *root;
  
public:
  
  AVLTree() : root(0) { }
  ~AVLTree() { }
  
  void insert(KeyType key, ItemType item) {
    if (root == 0) {
      root = new Node(key, item);
    }
    else _Insert(key, item, root);
  }
  
  void remove(KeyType key) {
    bool delOK = false;
    _Remove(root, key, delOK);
  }
  
  bool find(KeyType aKey, ItemType& aValue) {
    return _Find(aKey, aValue, root);
  }
  
  short getDepth() const {  return (root ? root->depth : 0);  }
  
  //---------------------------------------------
  
  class AVLIterator {
  public:
    
    AVLIterator(AVLIterator* aCopy) :
      tree(aCopy->tree), current(aCopy->current) {
      stackIndex = aCopy->stackIndex;
      
      for (int i = 0; i < stackIndex; ++i)
        stack[i] = aCopy->stack[i];
    }
    
    AVLIterator(AVLTree *aTree) : tree(aTree) {
      KeyType key;
      ItemType item;
      getFirst(key, item);
    }
    
    ~AVLIterator() { }
    
    bool getCurrent(KeyType& aKey, ItemType& aValue) {
      if (current) {
        aKey = current->key;
        aValue = current->value;
        return true;
      }
      return false;
    }
    
    // returns false if the tree is empty
    bool getFirst(KeyType& aKey, ItemType& aValue) {
      stackIndex = 0;
      
      if (!tree->root) {
        current = 0;
        aValue = 0;
        return false;
      }
      else {
        Node theCurr = tree->root;
        Node thePrev = theCurr;
        while (theCurr) {
          thePrev = theCurr;
          if (theCurr->left)
            stack[stackIndex++] = theCurr;
          theCurr = theCurr->left;
        }
        
        current = thePrev;
        aKey = current->key;
        aValue = current->value;
        return true;
      }
    }
    
    bool getNext(KeyType& aKey, ItemType& aValue) {
      if (!current) {
        aValue = 0;
        return false;
      }

      Node* theCurr = current->right;  // start looking to the right
        
      while (true) { // this while forces a traversal as far left as possible
        if (theCurr){  // if we have a pcurr, push it and go left, and repeat.
          stack[stackIndex++] = theCurr;
          theCurr = theCurr->left;
        }
        else { // backtrack
          if (stackIndex > 0) {

            Node* pCandidate = stack[--stackIndex];
            
            // did we backtrack up a right branch?
            if (current == pCandidate->right) {
              // if there is a parent, return the parent.
              if (stackIndex > 0) {
                current = stack[--stackIndex];
                aKey = current->key;
                aValue = current->value;
                return true;
              }
              else  { // if up a right branch, and no parent, traversal finished
                current = 0;
                aValue = 0;
                return false;
              }
            }
            else { // up a left branch, done.
              current = pCandidate;
              aKey = current->key;
              aValue = current->value;
              return true;
            }
          }
          else{  // totally done
            current = 0;
            aValue = 0;
            return false;
          }
        }
      }
    }
    
    bool find(KeyType aKey, ItemType& aValue) {
      Node* pCurr = tree->root;
      stackIndex = 0;
      
      while (true) {
        Node* pPushMe = pCurr;
        if (pCurr->m_Key == aKey) { // already done?
          current = pCurr;
          aValue = current->value;
          return true;
        }
        
        if (pCurr->key > aKey)
          pCurr = pCurr->left;
        else
          pCurr = pCurr->right;
        
        if (pCurr) { // maintain the stack so that GetNext will work.
          stack[stackIndex++] = pPushMe;
        }
        else { // couldn't find it.
          current = 0;
          stackIndex = 0;
          return false;
        }
      }
      
      return true;
    }
    
  protected:
    AVLTree*    tree;
    Node*       stack[kMaxDepth];
    short       stackIndex;
    Node*       current;        // for iteration
  }; //iterator
  
  friend class AVLIterator;
  
protected:
  
  void _Insert(KeyType key, ItemType item, Node *&root) {
    if (key < root->key) {
      if (root->left)
        _Insert(key, item, root->left);
      else
        root->left = new Node(key, item);
    }
    else if (key > root->key) {
      if (root->right)
        _Insert(key, item, root->right);
      else
        root->right = new Node(key, item);
    }
    else {
      // error - can't have duplicate keys.
      // if duplicate keys are okay, change key < to key <= above
    }
    
    computeBalance(root);
    balance(root);
  }
  
  void _Remove(Node*& root, KeyType key, bool& delOK) {
    if (!root) {
      delOK = false;
    }
    else if (root->m_Key > key) {  // go to left subtree
      _Remove(root->left, key, delOK);
      if (delOK) {
        computeBalance(root);
        balanceRight(root);
      }
    }
    else if (root->key < key) { // go to right subtree
      _Remove(root->right, key, delOK);
      if (delOK) {
        computeBalance(root);
        balanceLeft(root);
      }
    }
    else {  // node found!
      Node* pMe = root;
      if (!root->right) {
        root = root->left;
        delOK = true;
        delete pMe;
      }
      else if (!root->left){
        root = root->right;
        delOK = true;
        delete pMe;
      }
      else {
        _RemoveBothChildren(root, root->left, delOK);
        if (delOK) {
          computeBalance(root);
          balance(root);
        }
        delOK = true;
      }
    }
  }
  
  void _RemoveBothChildren(Node* &root, Node*& curr, bool& delOK) {
    if (!curr->right){
      root->key = curr->key;
      root->value = curr->value;
      Node* pMe = curr;
      curr = curr->left;
      delete pMe;
      delOK = true;
    }
    else {
      _RemoveBothChildren(root, curr->right, delOK);
      if (delOK) {
        computeBalance(root);
        balance(root);
      }
    }
  }
  
  bool _Find(KeyType key, ItemType& item, Node* root) {
    if (root) {
      if (root->key == key) {
        item = root->value;
        return true;
      }
      return (key < root->key)
        ? _Find(key, item, root->left)
        : _Find(key, item, root->right);
    }
    return false;
  }

  void computeBalance(Node*  root) {
    if (root) {
      short leftDepth  = root->left  ? root->left->depth  : 0;
      short rightDepth = root->right ? root->right->depth : 0;
      
      root->depth = 1 + ((leftDepth > rightDepth) ? leftDepth : rightDepth);
      root->balance = rightDepth - leftDepth;
    }
  }
  
  void balance(Node*& root) {
    if (root->balance > 1)
      balanceRight(root);
    
    if (root->balance < -1)
      balanceLeft(root);
  }
  
  void balanceRight(Node*& root) {
    if (root->right) {
      if (root->right->balance > 0) {
        rotateLeft(root);
      }
      else if (root->right->balance < 0) {
        rotateRight(root->right);
        rotateLeft(root);
      }
    }
  }
  
  void balanceLeft(Node*& root) {
    if (root->left) {
      if (root->left->balance < 0) {
        rotateRight(root);
      }
      else if (root->left->balance > 0) {
        rotateLeft(root->left);
        rotateRight(root);
      }
    }
  }
  
  void rotateLeft(Node*& root) {
    Node* pTemp = root;
    root = root->right;
    pTemp->right = root->left;
    root->left = pTemp;
    
    computeBalance(root->left);
    computeBalance(root->right);
    computeBalance(root);
  }

  void rotateRight(Node*& root) {
    Node* pTemp = root;
    root = root->left;
    pTemp->left = root->right;
    root->right = pTemp;
    
    computeBalance(root->left);
    computeBalance(root->right);
    computeBalance(root);
  }
  
};


#endif /* AVLTree_h */
