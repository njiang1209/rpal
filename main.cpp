#include "rb_tree.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  ifstream myfile;
  myfile.open (argv[1]);
  int n;
  myfile >> n;

  vector<pair<int, int> > input;

  for (int i = 0; i < n; i++) {
    pair<int, int> p;
    myfile >> p.first >> p.second;
    input.push_back(p);
  }

  rb_tree tree(input);
  tree.increase(17, 1);
  tree.increase(18, 1);
  tree.increase(19, 1);
  tree.increase(20, 1);

  while (true) {
    string command;
    cin >> command;

    int id, m;

    if (command == "increase") {
      cin >> id >> m;
      cout << tree.increase(id, m) << endl;
    }
    else if (command == "reduce") {
      cin >> id >> m;
      cout << tree.reduce(id, m) << endl;
    }
    else if (command == "count") {
      cin >> id;
      cout << tree.count(id) << endl;
    }
    else if (command == "inrange") {
      int low, high;
      cin >> low >> high;
      cout << tree.in_range(low, high) << endl;
    } 
    else if (command == "next") {
      cin >> id;
      auto res = tree.next(id);
      cout << res.first << " " << res.second << endl;
    }
    else if (command == "previous") {
      cin >> id;
      auto res = tree.previous(id);
      cout << res.first << " " << res.second << endl;
    }
    else if (command == "print") {
      tree.level_print();
    }
  }
}