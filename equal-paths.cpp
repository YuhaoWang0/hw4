#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool toLeaves(Node* root, int depth, int& leaveDepth) {
  if(root == nullptr) {
    return true;
  }

  if(root->left == nullptr && root->right == nullptr) {
    
    if(leaveDepth == -1) {
      leaveDepth = depth;
      return true;
    }

    if(depth == leaveDepth) {
      return true;
    } else {
      return false;
    }
  }

  bool left = toLeaves(root->left, depth + 1, leaveDepth);
  bool right = toLeaves(root->right, depth + 1, leaveDepth);

  if(left && right) {
    return true;
  } 

  return false;
}

bool equalPaths(Node * root)
{

  // Add your code below
  int initialValue = -1;
  return toLeaves(root, 0, initialValue);
}

