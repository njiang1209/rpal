#include <iostream>
#include <vector>

using namespace std;

struct rb_node {
  rb_node* left;
  rb_node* right;
  rb_node* parent;
  bool black;
  int count;
  int id;

  rb_node(int id, bool black) : 
    id(id),
    count(0), 
    black(black), 
    left(NULL), 
    right(NULL),
    parent(NULL)
  {}

  ~rb_node() {}
};

class rb_tree {
  void traverse(vector<rb_node*>& to_delete, rb_node* current);
  void in_order_print(rb_node* node);
  void fixup_insert(rb_node* node);
  void left_rotate(rb_node* node);
  void right_rotate(rb_node* node);
  void transplant(rb_node* original_node, rb_node* new_node);
  void fixup_delete(rb_node* current);
  void in_range_rec(int low, int high, rb_node* current, int& result);
  void init_rec(vector<pair<int, int> > &init, int level, rb_node* parent, bool left, int low, int high);
  rb_node* create_node(int id, bool black);

public:
  rb_node* root;
  rb_node* nil;

  rb_tree();
  rb_tree(vector<pair<int, int> > &init);
  ~rb_tree();
  int increase(int id, int count);
  int reduce(int id, int count);
  void print();
  void level_print();
  int count (int id);
  int in_range(int low, int high);
  pair<int, int> next(int id);
  pair<int, int> previous(int id);

};