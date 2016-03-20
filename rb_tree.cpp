#include "rb_tree.h"

using namespace std;

rb_tree::rb_tree() {
  nil = new rb_node(0, true);
  nil->left = nil->right = nil->parent = nil;
  root = nil;
}

/*
creates a perfectly balanced BST from a sorted list by recursively finding medians
every node is black except for the last level of the tree
O(n) time complexity
*/
rb_tree::rb_tree(vector<pair<int, int> > &init) {
  nil = new rb_node(0, true);
  nil->left = nil->right = nil->parent = nil;
  root = nil;

  init_rec(init, 1, nil, true, 0, init.size() - 1);
  root = nil->left;
}

void rb_tree::init_rec(vector<pair<int, int> > &init, int level, rb_node* parent, bool left, int low, int high) {
  if (low > high) return;

  int mid = low + (high - low) / 2;

  rb_node* new_node = create_node(init[mid].first, true);
  new_node->count = init[mid].second;
  new_node->parent = parent;

  if (left) parent->left = new_node;
  else parent->right = new_node;

  // if we are on last level of BST, set node to red
  if ((1 << level) > init.size()) 
    new_node->black = false;
  else {
    // use up to 4 threads to initialize tree
    if (level <= 2) {
      thread left(&rb_tree::init_rec, this, ref(init), level + 1, new_node, true, low, mid-1);
      thread right(&rb_tree::init_rec, this, ref(init), level + 1, new_node, false, mid+1, high);
      left.join();
      right.join();
    } else {
      init_rec(init, level + 1, new_node, true, low, mid-1);
      init_rec(init, level + 1, new_node, false, mid+1, high);
    }
  }
}

rb_tree::~rb_tree() {
  postorder_delete(root);

  delete nil;
}

// postorder traversal to deallocate memory
void rb_tree::postorder_delete(rb_node* current) {
  if (current == nil) return;

  postorder_delete(current->left);
  postorder_delete(current->right);
  delete current;
}

/* 
  1: if right subtree is not nil
      successor of a node is the smallest node in right subtree
  2: if right subtree is nil, then travel up the tree until the current node is a left child
      the parent of that node is the successor
      if no left child is found, then there is no successor  
*/
pair<int, int> rb_tree::next(int id) {
  // find node with given id
  rb_node* current = root;
  rb_node* parent = nil;

  while (current != nil && current->id != id) {
    parent = current;
    if (current->id > id) 
      current = current->left;
    else 
      current = current->right;
  }

  // if node with given id is not found, set current to parent
  if (current == nil) {
    current = parent;

    // if current is greater then given id, current node is the successor
    if (current->id > id)
      return pair<int, int>(current->id, current->count);
  }

  // case 1:
  if (current->right != nil) {
    current = current->right;
    while (current->left != nil)
      current = current->left;
  } 

  // case 2:
  else {
    while (current->parent->right == current) {
      if (current == nil) return pair<int, int>(0, 0);
      current = current->parent;
    }
    current = current->parent;
  }
  return pair<int, int>(current->id, current->count);
}


// symmetric with next()
pair<int, int> rb_tree::previous(int id) {
  rb_node* current = root;
  rb_node* parent = nil;

  while (current != nil && current->id != id) {
    parent = current;
    if (current->id > id) 
      current = current->left;
    else 
      current = current->right;
  }

  if (current == nil) {
    current = parent;
    if (current->id < id)
      return pair<int, int>(current->id, current->count);
  }

  if (current->left != nil) {
    current = current->left;
    while (current->right != nil)
      current = current->right;
  } else {
    while (current->parent->left == current) {
      if (current == nil) return pair<int, int>(0, 0);
      current = current->parent;
    }
    current = current->parent;
  }
  return pair<int, int>(current->id, current->count);
}

int rb_tree::count(int id) {
  rb_node* current = root;
  while (current != nil && current->id != id) {
    if (current->id > id) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  if (current == nil) return 0;
  return current->count;
}


int rb_tree::in_range(int low, int high) {
  int result = 0;
  in_range_rec(low, high, root, result);
  return result;
}

/* 
traverse the tree
  if current node is within the range, add it to the result and continue on both sides
  if the current node is less, continue traversing on the right and vice versa
*/
void rb_tree::in_range_rec(int low, int high, rb_node* current, int& result) {
  if (current == nil) return;

  if (current->id >= low && current->id <= high) {
    result += current->count;
    in_range_rec(low, high, current->left, result);
    in_range_rec(low, high, current->right, result);
  } 

  else if (current->id < low) {
    in_range_rec(low, high, current->right, result);
  } else {
    in_range_rec(low, high, current->left, result);
  }
}


void rb_tree::left_rotate(rb_node* node) {
  rb_node* y = node->right;
  node->right = y->left;
  if (y->left != nil) y->left->parent = node;
  y->parent = node->parent;
  if (node->parent == nil) root = y;
  else if (node == node->parent->left) node->parent->left = y;
  else node->parent->right = y;
  y->left = node;
  node->parent = y;
}

void rb_tree::right_rotate(rb_node* node) {
  rb_node* y = node->left;
  node->left = y->right;
  if (y->right != nil) y->right->parent = node;
  y->parent = node->parent;
  if (node->parent == nil) root = y;
  else if (node == node->parent->right) node->parent->right = y;
  else node->parent->left = y;
  y->right = node;
  node->parent = y;
}

int rb_tree::increase(int id, int count) {
  // if tree is empty create the root and return
  if (root == nil) {
    root = create_node(id, true);
    root->count += count;
    return count;
  }

  // find node with given id
  rb_node* parent = nil;
  bool pc_left;

  rb_node* current = root;
  while (current != nil && current->id != id) {
    parent = current;
    if (current->id > id) {
      current = current->left;
      pc_left = true;
    } else {
      current = current->right;
      pc_left = false;
    }
  }

  // if node does not exist, insert it as a red node; fix imbalances
  if (current == nil) {
    current = create_node(id, false);
    current->parent = parent;
    if (pc_left)
      parent->left = current;
    else
      parent->right = current;

    fixup_insert(current);
  }

  current->count += count;
  return current->count;
}


// if parent of current node is red, there is a rb violation
void rb_tree::fixup_insert(rb_node* z) {
  while (!z->parent->black) {
    // if parent node is a left child
    if (z->parent == z->parent->parent->left) {
      // uncle of current node
      rb_node* y = z->parent->parent->right;

      // if uncle is red, set parent and uncle to black and grandparent to red
      // and move up to grandparent
      if (!y->black) {
        z->parent->black = true;
        y->black = true;
        z->parent->parent->black = false;
        z = z->parent->parent;
      } 
      // when uncle is black:
      //  if current node is a right child, do left rotation
      else if (z == z->parent->right) {
        z = z->parent;
        left_rotate(z);
      } 
      //  if current node is a left child, set parent and uncle to black
      //  right rotate
      else {
        z->parent->black = true;
        z->parent->parent->black = false;
        right_rotate(z->parent->parent);
      }
    }

    // if current node is a right child (symmetric with above)
    else {
      rb_node* y = z->parent->parent->left;
      if (!y->black) {
        z->parent->black = true;
        y->black = true;
        z->parent->parent->black = false;
        z = z->parent->parent;
      } else if (z == z->parent->left) {
        z = z->parent;
        right_rotate(z);
      } else {
        z->parent->black = true;
        z->parent->parent->black = false;
        left_rotate(z->parent->parent);
      }
    }
  }
  root->black = true;
}

// helper to construct a node
rb_node* rb_tree::create_node(int id, bool black) {
  rb_node* res = new rb_node(id, black);
  res->count = 0;
  res->parent = nil;
  res->left = nil;
  res->right = nil;
  return res;
}

/*
  delete node normally
  when a red node is deleted, there is no rule violation
  if a black node is deleted, then a subtree will be deficient by one black node
  fix afterwards
*/
int rb_tree::reduce(int id, int count) {
  // find node with given id
  rb_node* current = root;
  while (current != nil && current->id != id) {
    if (current->id > id) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  // early return if a node is not deleted
  if (current == nil) return 0;
  current->count -= count;
  if (current->count > 0) return current->count;

  // delete node only when count <= 0
  bool deleted_black = current->black;
  rb_node* deficient_subtree;

  // if node has no left subtree, transplant right subtree onto current node
  if (current->left == nil) {
    deficient_subtree = current->right;
    transplant(current, current->right);
  }

  // symmetric with above
  else if (current->right == nil) {
    deficient_subtree = current->left;
    transplant(current, current->left);
  }

  // if node has 2 children:
  // replace current node with  smallest node from right subtree
  else {
    // find successor
    rb_node* successor = current->right;
    while (successor->left != nil) successor = successor->left;

    deleted_black = successor->black;
    deficient_subtree = successor->right;

    // if successor is not right child of current, replace successor with its right subtree
    if (current->right != successor) {
      transplant(successor, successor->right);
      successor->right = current->right;
      successor->right->parent = successor;
    }

    // replace current with successor
    transplant(current, successor);
    successor->left = current->left;
    successor->left->parent = successor;
    successor->black = current->black;
  }

  if (deleted_black) {
    fixup_delete(deficient_subtree);
  }

  delete current;
  return 0;
}


// helper method that moves subtree rooted at new_node to where original_node was
void rb_tree::transplant(rb_node* original_node, rb_node* new_node) {
  if (original_node->parent == nil) root = new_node;
  else if (original_node == original_node->parent->left) original_node->parent->left = new_node;
  else original_node->parent->right = new_node;
  new_node->parent = original_node->parent;
}

// called when a black node is deleted
// current points to a subtree that is deficient by one black node
void rb_tree::fixup_delete(rb_node* current) {
  // stop when 
  while (current != root && current->black) {
    rb_node* sibling;

    // if current is a left child
    if (current == current->parent->left) {
      sibling = current->parent->right;

      // if sibling is red, switch current and sibling's colors
      // set parent to red, left rotate
      if (!sibling->black) {
        current->black = false;
        sibling->black = true;
        current->parent->black = false;
        left_rotate(current->parent);
        sibling = current->parent->right;
      }

      // sibling is black and its children are black
      if (sibling->left->black && sibling->right->black) {
        sibling->black = false;
        current = current->parent;
      } 
      else {
        // sibling is black and sibling's right child is black
        if (sibling->right->black) {
          sibling->left->black = true;
          sibling->black = false;
          right_rotate(sibling);
          sibling = current->parent->right;
        } 

        sibling->black = current->parent->black;
        current->parent->black = true;
        sibling->right->black = true;
        left_rotate(current->parent);
        current = root;
      }
    }

    // if current is a right child, symmetric with above
    else {
      sibling = current->parent->left;

      if (!sibling->black) {
        current->black = false;
        sibling->black = true;
        current->parent->black = false;
        right_rotate(current->parent);
        sibling = current->parent->left;
      }

      if (sibling->right->black && sibling->left->black) {
        sibling->black = false;
        current = current->parent;
      } 
      else {
        if (sibling->left->black) {
          sibling->right->black = true;
          sibling->black = false;
          left_rotate(sibling);
          sibling = current->parent->left;
        } 

        sibling->black = current->parent->black;
        current->parent->black = true;
        sibling->left->black = true;
        right_rotate(current->parent);
        current = root;
      }
    }
  }

  current->black = true;
}



// print methods for debugging
void rb_tree::in_order_print(rb_node* node) {
  if (node != nil) {
    in_order_print(node->left);
    cout << node->id << ": " << node->count;
    if (node->black)
      cout << " (black)" << endl;
    else
      cout << " (red)" << endl;
    in_order_print(node->right);
  }
}

void rb_tree::print() {
  in_order_print(root);
}

void rb_tree::level_print() {
  vector<rb_node*> current_level;
  vector<rb_node*> next_level;
  current_level.push_back(root);
  int nodes_at_level = 1;

  while (nodes_at_level > 0) {
    nodes_at_level = 0;
    next_level.clear();
    for (auto it = current_level.begin(); it != current_level.end(); it++) {
      rb_node* node = *it;
      if (node != nil) {
        nodes_at_level++;
        cout << node->id;
        if (node->black) cout << "B";
        else cout << "R";
        cout << " ";

        next_level.push_back(node->left);
        next_level.push_back(node->right);
      }
      else {
        cout << "nil ";
        next_level.push_back(nil);
        next_level.push_back(nil);
      }
    }
    current_level = next_level;
    cout << endl;
  }
}


