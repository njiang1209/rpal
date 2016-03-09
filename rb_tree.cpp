#include "rb_tree.h"
#include <iostream>
#include <vector>
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
}

void rb_tree::init_rec(vector<pair<int, int> > &init, int level, rb_node* parent, bool left, int low, int high) {
  if (low > high) return;

  int mid = low + (high - low) / 2;

  rb_node* new_node = create_node(init[mid].first, true);
  if (level == 1) root = new_node;

  new_node->count = init[mid].second;
  new_node->parent = parent;
  if (left) parent->left = new_node;
  else parent->right = new_node;

  // if we are on last level of BST, set node to red; otherwise, set nodes to black
  if ((1 << level) > init.size()) 
    new_node->black = false;
  else {
    init_rec(init, level + 1, new_node, true, low, mid-1);
    init_rec(init, level + 1, new_node, false, mid+1, high);
  }
}

rb_tree::~rb_tree() {
  vector<rb_node*> to_delete;
  if (root != nil) {
    traverse(to_delete, root);
    for (auto it = to_delete.begin(); it != to_delete.end(); it++) {
      delete (*it);
    }
  }

  delete nil;
}

pair<int, int> rb_tree::next(int id) {
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
    if (current->id > id)
      return pair<int, int>(current->id, current->count);
  }

  if (current->right != nil) {
    current = current->right;
    while (current->left != nil)
      current = current->left;
  } else {
    while (current->parent->right == current) {
      if (current == nil) return pair<int, int>(0, 0);
      current = current->parent;
    }
    current = current->parent;
  }
  return pair<int, int>(current->id, current->count);
}

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

void rb_tree::traverse(vector<rb_node*>& to_delete, rb_node* current) {
  if (current == nil) return;

  to_delete.push_back(current);
  traverse(to_delete, current->left);
  traverse(to_delete, current->right);
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

int rb_tree::in_range(int low, int high) {
  int result = 0;
  in_range_rec(low, high, root, result);
  return result;
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

void rb_tree::fixup_insert(rb_node* z) {
  while (!z->parent->black) {
    if (z->parent == z->parent->parent->left) {
      rb_node* y = z->parent->parent->right;
      if (!y->black) {
        z->parent->black = true;
        y->black = true;
        z->parent->parent->black = false;
        z = z->parent->parent;
      } else if (z == z->parent->right) {
        z = z->parent;
        left_rotate(z);
      } else {
        z->parent->black = true;
        z->parent->parent->black = false;
        right_rotate(z->parent->parent);
      }
    }

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

rb_node* rb_tree::create_node(int id, bool black) {
  rb_node* res = new rb_node(id, black);
  res->count = 0;
  res->parent = nil;
  res->left = nil;
  res->right = nil;
  return res;
}

int rb_tree::increase(int id, int count) {
  if (root == nil) {
    root = create_node(id, true);
    root->count += count;
    return count;
  }

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

void rb_tree::fixup_delete(rb_node* current) {
  while (current != root && current->black) {
    rb_node* sibling;

    if (current == current->parent->left) {
      sibling = current->parent->right;

      if (!sibling->black) {
        sibling->black = true;
        current->parent->black = false;
        left_rotate(current->parent);
        sibling = current->parent->right;
      }

      if (sibling->left->black && sibling->right->black) {
        sibling->black = false;
        current = current->parent;
      } 
      else {
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
    else {
      sibling = current->parent->left;

      if (!sibling->black) {
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
}

void rb_tree::transplant(rb_node* original_node, rb_node* new_node) {
  if (original_node->parent == nil) root = new_node;
  else if (original_node == original_node->parent->left) original_node->parent->left = new_node;
  else original_node->parent->right = new_node;
  new_node->parent = original_node->parent;
}

int rb_tree::reduce(int id, int count) {

  rb_node* current = root;
  while (current != nil && current->id != id) {
    if (current->id > id) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  if (current == nil) return 0;

  current->count -= count;
  if (current->count > 0) return current->count;

  // delete node when count <= 0

  bool deleted_black = current->black;
  rb_node* deficient_subtree;

  if (current->left == nil) {
    deficient_subtree = current->right;
    transplant(current, current->right);
  }

  else if (current->right == nil) {
    deficient_subtree = current->left;
    transplant(current, current->left);
  }

  else {
    rb_node* successor = current->right;
    while (successor->left != nil) successor = successor->left;

    deleted_black = successor->black;
    deficient_subtree = successor->right;

    if (current->right == successor) {
      deficient_subtree->parent = successor;
    } else {
      transplant(successor, successor->right);
      successor->right = current->right;
      successor->right->parent = successor;
    }

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


