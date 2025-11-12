#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

  
  void rotateLeft(AVLNode<Key, Value>* x);
  void rotateRight(AVLNode<Key, Value>* x);

  void rebalanceInsert(AVLNode<Key, Value>* start);
  void rebalanceRemove(AVLNode<Key, Value>* start, int8_t diff);

  void attachAsLeft(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
  void attachAsRight(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    const Key& k = new_item.first;
    const Value& v = new_item.second;

    if(this->root_ == NULL) {
      this->root_ = new AVLNode<Key, Value>(k, v, NULL);
      return;
    }

    AVLNode<Key, Value>* cur = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while(cur != NULL) {
      parent = cur;
      if(k < cur->getKey()) {
        cur = cur->getLeft();
      } else if (k > cur->getKey()){
        cur = cur->getRight();
      } else {
        cur->setValue(v);
        return;
      }
    }

    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(k, v, parent);
    if(k < parent->getKey()) {
      parent->setLeft(n);
      parent->updateBalance(1);
      rebalanceInsert(parent);
    } else {
      parent->setRight(n);
      parent->updateBalance(-1);
      rebalanceInsert(parent);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* z = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(z == NULL){
      return;
    } 

    if(z->getLeft() != NULL && z->getRight() != NULL) {
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(z));
      nodeSwap(z, pred);
    }

    AVLNode<Key, Value>* parent = z->getParent();
    AVLNode<Key, Value>* child;
    if(z->getLeft() != NULL) {
      child = z->getLeft();
    } else {
      child = z->getRight();
    }

    int8_t diff = 0;

    if(child != NULL) {
      child->setParent(parent);
    }

    if(parent == NULL) {
      this->root_ = child;
    } else if(parent->getLeft() == z) {
      parent->setLeft(child);
      diff = -1;
    } else {
      parent->setRight(child);
      diff = 1;
    }

    delete z;

    if(parent != NULL) {
      rebalanceRemove(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::attachAsLeft(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child) {
  parent->setLeft(child);
  if(child) {
    child->setParent(parent);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::attachAsRight(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child) {
  parent->setRight(child);
  if(child){
    child->setParent(parent);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceRemove(AVLNode<Key, Value>* n, int8_t diff){
  while(n != NULL) {
    n->updateBalance(diff);
    int8_t b = n->getBalance();

    if(b == 2) {
      AVLNode<Key, Value>* L = n->getLeft();
      int8_t lb = L->getBalance();

      if(lb >= 0) {
        rotateRight(n);
        if(lb ==0) {
          n->setBalance(1);
          L->setBalance(-1);
          return;
        } else {
          n->setBalance(0);
          L->setBalance(0);
          n=L;
        }
      } else {
        AVLNode<Key, Value>* LR = L->getRight();
        int8_t lrb = LR->getBalance();
        rotateLeft(L);
        rotateRight(n);
        if(lrb == 1) {
          n->setBalance(-1);
          L->setBalance(0);
        } else if(lrb == 0) {
          n->setBalance(0);
          L->setBalance(0);
        } else {
          n->setBalance(0);
          L->setBalance(1);
        }
        LR->setBalance(0);
        n=LR;
      }
    } else if(b == -2) {
      AVLNode<Key, Value>* R = n->getRight();
      int8_t rb = R->getBalance();

      if(rb <=0) {
        rotateLeft(n);
        if(rb == 0) {
          n->setBalance(-1);
          R->setBalance(1);
          return;
        } else {
          n->setBalance(0);
          R->setBalance(0);
          n=R;
        }
      } else {
        AVLNode<Key, Value>* RL = R->getLeft();
        int8_t rlb = RL->getBalance();
        rotateRight(R);
        rotateLeft(n);
        if(rlb == -1) {
          n->setBalance(1);
          R->setBalance(0);
        } else if(rlb == 0 ){
          n->setBalance(0);
          R->setBalance(0);
        } else {
          n->setBalance(0);
          R->setBalance(-1);
        }
        RL->setBalance(0);
        n=RL;
      }
    } else if(b==0) {

    } else {
      return;
    }
    AVLNode<Key, Value>* p = n->getParent();
    if(p == NULL) {
      return;
    } 
    if(p->getLeft() == n) {
      diff = -1;
    } else {
      diff = 1;
    } 

    n=p;
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x) {
  AVLNode<Key, Value>* y = x->getRight();
  AVLNode<Key, Value>* p = x->getParent();

  attachAsRight(x, y->getLeft());
  attachAsLeft(y, x);
  y->setParent(p);
  if(p == NULL) {
    this->root_ = y;
  } else if(p->getLeft() == x) {
    p->setLeft(y);
  } else{
    p->setRight(y);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x) {
  AVLNode<Key, Value>* y = x->getLeft();
  AVLNode<Key, Value>* p = x->getParent();

  attachAsLeft(x, y->getRight());
  attachAsRight(y, x);

  y->setParent(p);
  if(p == NULL) {
    this->root_ = y;
  } else if(p->getLeft() == x) {
    p->setLeft(y);
  } else{
    p->setRight(y);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceInsert(AVLNode<Key, Value>* n) {
  while(n != NULL) {
    int8_t b = n->getBalance();

    if(b== 0) {
      return;
    } else if(b == 1||b == -1) {
      AVLNode<Key, Value>* p = n->getParent();
      if(p == NULL) {
        return ;
      }
      if(p->getLeft() == n) {
        p->updateBalance(1);
      } else {
        p->updateBalance(-1);
      }
      n=p;
    } else if(b == 2) {
      AVLNode<Key, Value>* L = n->getLeft();
      if(L->getBalance() >= 0) {
        rotateRight(n);
        n->setBalance(0);
        L->setBalance(0);
      } else {
        AVLNode<Key, Value>* LR = L->getRight();
        int8_t lrB = LR->getBalance();
        rotateLeft(L);
        rotateRight(n);
        if(lrB == 1) {
          n->setBalance(-1);
          L->setBalance(0);
        } else if(lrB == 0) {
          n->setBalance(0);
          L->setBalance(0);
        } else{ 
          n->setBalance(0);
          L->setBalance(1);
        }
        LR->setBalance(0);
      }
      return;
    } else if( b == -2) {
      AVLNode<Key, Value>* R = n->getRight();
      if(R->getBalance() <= 0) {
        rotateLeft(n);
        n->setBalance(0);
        R->setBalance(0);
      } else {
        AVLNode<Key, Value>* RL = R->getLeft();
        int8_t rlB = RL->getBalance();
        rotateRight(R);
        rotateLeft(n);
        if(rlB == -1) {
          n->setBalance(1);
          R->setBalance(0);
        } else if(rlB == 0) {
          n->setBalance(0);
          R->setBalance(0);
        } else{ 
          n->setBalance(0);
          R->setBalance(-1);
        
        }
        RL->setBalance(0);
      }
      return;
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
