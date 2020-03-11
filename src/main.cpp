#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <queue>
#include <cstdlib>
#include <unordered_set>
#include <getopt.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

const int DELTA_X[4] = {-1, 0, 1, 0};
const int DELTA_Y[4] = {0, -1, 0, 1};
const int MAX_COST = 1e6;
const int LOWER_BOUND = -20;
const int UPPER_BOUND = 20;


// global variables: default values for env setting flags
bool static_laser_enabled = false;
bool dynamic_laser_enabled = false;
bool wormholes_enabled = false;

enum laser_direction {
  N, E, S, W
};


struct pair_hash {
  size_t operator()(const pair<int, int> &pair) const {
    return hash<int>()(pair.first) ^ hash<int>()(pair.second);
  }
};

class Node {
 public:
  int x;
  int y;
  bool isObs;
  int gcost;  // gcost: real cost from start point
  int fcost;  // fcost: eva cost in open
//  int hcost;  // hcost: heuristic cost to destination
  bool visited;
  Node *prev;
  int timestamp;

  Node() : x(0), y(0), isObs(false), gcost(MAX_COST), fcost(MAX_COST), visited(false), prev(nullptr) {}
  Node(int _x, int _y) : x(_x), y(_y), isObs(false), gcost(MAX_COST), fcost(MAX_COST), visited(false), prev(nullptr) {}
  Node(int _x, int _y, bool obs_flag) : x(_x), y(_y), isObs(obs_flag), gcost(MAX_COST), fcost(MAX_COST), visited
      (false), prev(nullptr) {}

  bool operator<(const Node &p2) const {
    return fcost < p2.fcost;
  }
  bool operator>(const Node &p2) const {
    return fcost > p2.fcost;
  }

  friend ostream &operator<<(ostream &os, const Node &Node) {
    os << "Node : [" << Node.x << ", " << Node.y << "]" << endl;
    return os;
  }

  bool operator==(const Node &rhs) {
    return (this->x == rhs.x && this->y == rhs.y);
  }
  bool operator==(const Node *rhs) {
    return (this->x == rhs->x && this->y == rhs->y);
  }
};

struct node_hash {
  size_t operator()(const Node *node) const {
    return hash<int>()(node->x * 31) ^ hash<int>()(node->y);
  }
};

class Barrier : public Node {
 public:
  Barrier(int _x, int _y) : Node(_x, _y, true) {}

  friend ostream &operator<<(ostream &os, const Barrier &barrier) {
    os << "Barrier : [" << barrier.x << ", " << barrier.y << "]" << endl;
    return os;
  }
};

class Laser : public Node {
 public:
  int x;
  int y;
  int direction;

  Laser(int _x, int _y, char _direction) : x(_x), y(_y), direction(_direction) {}

  void set(int _x, int _y, int _direction) {
    x = _x;
    y = _y;
    direction = _direction;
  }

  friend ostream &operator<<(ostream &os, const Laser &laser) {
    os << "L: (" << laser.x << ", " << laser.y << ", " << laser.direction << ")" << endl;
    return os;
  }

};

class Holes {
 public:
  Holes(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

  friend ostream &operator<<(ostream &os, const Holes &holes) {
    os << "holes at : [" << holes.x1 << ", " << holes.y1
       << "], [" << holes.x2 << ", " << holes.y2 << "]" << endl;
    return os;
  }

 public:
  int x1;
  int y1;
  int x2;
  int y2;
};

void parseIntFromString(string &data, int &number, int &Node1, int &Node2) {
  // get int from the read data
  while (!isdigit(data[Node1])) {
    Node1++;
  }
  Node2 = Node1;
  while (isdigit(data[Node2])) {
    Node2++;
  }
  string sub_data = data.substr(Node1, Node2 - Node1);
  number = stoi(sub_data);
}

/*
 * parse integer from string
 * */
void parseIntFromString(string &data, int &number) {
  int Node1 = 0;
  int Node2 = 0;
  // get int from the read data
  while (!isdigit(data[Node1])) {
    Node1++;
  }
  Node2 = Node1;
  while (isdigit(data[Node2])) {
    Node2++;
  }
  string sub_data = data.substr(Node1, Node2 - Node1);
  number = stoi(sub_data);
}

void parseDirectionFromString(string &data, int &direction, int &Node1, int &Node2) {
  // get int from the read data
  while (data[Node1] != 39) { // ascii ' is 39
    Node1++;
  }
  Node2 = Node1 + 2;
  direction = data[Node1 + 1];
}

/*
 * parse direction 'N', 'S', 'W', 'E' from string
 * */
void parseDirectionFromString(string &data, int &direction) {
  int Node1 = 0;
  int Node2 = 0;
  // get int from the read data
  while (data[Node1] != 39) { // ascii ' is 39
    Node1++;
  }
  Node2 = Node1 + 2;
  char direction_char = data[Node1 + 1];
//  cout << data << endl;
//  cout << direction_char << endl;
  switch (direction_char) {
    case 'N': direction = 0;
      break;
    case 'E': direction = 1;
      break;
    case 'S': direction = 2;
      break;
    case 'W': direction = 3;
      break;
    default: cout << "not right direction given!" << endl;
      exit(EXIT_FAILURE);
  }

}

/*
 * read one pair from the string to a vactor
 * */
void readOnePair(ifstream &infile, Node *Node, string &data) {
  int number = 0;
  // read x
  infile >> data;
  parseIntFromString(data, number);
  Node->x = number;

  // read y
  infile >> data;
  parseIntFromString(data, number);
  Node->y = number;
}

void getBarrierCoordinatesFromLine(vector<Barrier *> &barriers, string &line) {
  istringstream instr(line);
  string temp;
  int number;
  vector<int> barrier_coordinates(2, 0);
  int i = 0;
  while (instr >> temp) {
//        cout << "barrier :" << temp << endl;
    // check if no space between x and y
    temp = temp.substr(0, temp.length() - 1);
    const char *temp_c = temp.c_str();
    const char *ptr = strchr(temp_c, ',');
    if (ptr != nullptr) {
      // ',' exist in temp, need to parse twice
      string sub_temp1 = temp.substr(0, ptr - temp_c + 1);
      string sub_temp2 = temp.substr(ptr - temp_c + 1);
//            cout << "sep 1: " << sub_temp1 << endl;
//            cout << "sep 2: " << sub_temp2 << endl;
      parseIntFromString(sub_temp1, number);
      barrier_coordinates[i % 2] = number;
      i++;
      parseIntFromString(sub_temp2, number);
      barrier_coordinates[i % 2] = number;
//            i++;

    } else {
      parseIntFromString(temp, number);
      barrier_coordinates[i % 2] = number;
//            cout << number << endl;
    }

    if ((i + 1) % 2 == 0) {
      Barrier *barrier = new Barrier(barrier_coordinates[i % 2 - 1], barrier_coordinates[i % 2]);
      barriers.push_back(barrier);
      cout << *barrier;
    }
    i++;
  }
}

void getLaserCoordinatesFromLine(vector<Laser *> &lasers, string &line) {
  if (line == "[]") {
    cout << "no laser" << endl;
    return;
  }
  istringstream instr(line);
  string temp;
  int number;
  int direction;
  int i = 0;
  vector<int> laser_coordinates(3, 0);

  while (instr >> temp) {
//        cout << i << ": "<< temp << endl;
    if ((i + 1) % 3) {
      parseIntFromString(temp, number);
      laser_coordinates[i % 3] = number;
    } else {
      parseDirectionFromString(temp, direction);
      laser_coordinates[i % 3] = direction;
      Laser *laser_ptr = new Laser(laser_coordinates[i % 3 - 2], laser_coordinates[i % 3 - 1],
                                   laser_coordinates[i % 3]);
      lasers.push_back(laser_ptr);
      cout << *laser_ptr;
    }
    i++;
  }
}

void getHolesCoordinatesFromLine(vector<Holes *> holes, string &line) {
  if (line == "[]") {
    cout << "no wormholes" << endl;
    return;
  }
  istringstream instr(line);
  string temp;
  int number;
  vector<int> hole_coordinates(4, 0);
  int i = 0;
  while (instr >> temp) {
//        cout << i << endl;
//        cout << "holes :" << temp << endl;
    parseIntFromString(temp, number);
    hole_coordinates[i % 4] = number;
//        cout << number << endl;
    if ((i + 1) % 4 == 0 && i != 0) {
      Holes *hole = new Holes(hole_coordinates[i % 4 - 3], hole_coordinates[i % 4 - 2],
                              hole_coordinates[i % 4 - 1], hole_coordinates[i % 4]);
      holes.push_back(hole);
      cout << *hole;
    }
    i++;
  }
}

/*
 * readInputData: read the problem file
 * */
void readInputData(const string &problem_file, Node *origin, Node *destination,
                   vector<Barrier *> &barriers, vector<Laser *> &lasers, vector<Holes *> &holes) {

  ifstream infile;
  string line;
  string data;
  // open a file in read mode.
  infile.open(problem_file);

  if (!infile) {
    cout << "Unable to open " << problem_file << endl;
    exit(1); // terminate with error
  }
  // read origin and destination
  readOnePair(infile, origin, data);
  readOnePair(infile, destination, data);

  cout << "origin: " << origin->x << ", " << origin->y << endl;
  cout << "destination: " << destination->x << ", " << destination->y << endl;

  // read barrier coordinates
  getline(infile, line); // empty line

  if (getline(infile, line)) {
    cout << "barrier LINE :" << line << endl;
    getBarrierCoordinatesFromLine(barriers, line);
  } else {
    cout << "no barriers!" << endl;
  }

  // read lasers line
  if (getline(infile, line)) {
    cout << "laser LINE :" << line << endl;
    getLaserCoordinatesFromLine(lasers, line);
  } else {
    cout << "no lasers!" << endl;
  }

  // read wormholes line
  if (getline(infile, line)) {
    cout << "wormholes LINE :" << line << endl;
    getHolesCoordinatesFromLine(holes, line);
  } else {
    cout << "no wormholes!" << endl;
  }
  infile.close();
}

int MahattanDistance(Node *n1, Node *n2) {
  return abs(n1->x - n2->x) + abs(n1->y - n2->y);
}

bool isBarrier(const Node *node, const vector<Barrier *> &barriers) {
  for (Barrier *b : barriers) {
    if ((b->x == node->x) && (b->y == node->y)) {
      return true;
    }
  }
  return false;
}

bool isLaser(const Node *node, const vector<Laser *> &laser_path) {
  for (Laser *lp : laser_path) {
    if ((lp->x == node->x) && (lp->y == node->y)) {
      return true;
    }
  }
  return false;
}

void findPathAstarBarriersOnly(Node *origin, Node *destination,
                               vector<Barrier *> &barriers, vector<Laser *> &lasers, vector<Holes *> &holes,
                               vector<Node *> &path) {

  // first version: only have minimum path
  priority_queue<Node *, vector<Node *>, greater<Node *>> open;
  unordered_set<pair<int, int>, pair_hash> closed;

  origin->gcost = 0;
  open.push(origin);

  int cnt = 0;
  bool reach_flag = false;
  while (!open.empty() && cnt < 20) {
    cnt++;
    Node *curr = open.top();
    open.pop();
//        curr->visited = true;
    closed.insert(make_pair(curr->x, curr->y));
//        cout << "curr" << *curr << "fcost = " << curr->gcost << ", expand number = " << cnt << endl;
    // check if reached
    if (*curr == *destination) {
      destination = curr;
      cout << "reached!" << endl;
      reach_flag = true;
      break;
    }

    for (int i = 0; i < 4; i++) {
      int nx = curr->x + DELTA_X[i];
      int ny = curr->y + DELTA_Y[i];
      // Assume the grid itself is infinite in size, no need to check boundary condition
      // only check if obs
      Node *next = new Node(nx, ny);
//            cout << *next << isBarrier(next, barriers) << endl;
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);

          // cout << "add next : " << next;
        }
      }
    } // for i
  } // while open

  if (!reach_flag) {
    cout << "can't find path!" << endl;
  } else {
    cout << "PATH" << endl;
    // reconstruct path;
    Node *curr = destination;
    path.push_back(curr);

    while (curr->prev) {
      path.push_back(curr->prev);
      curr = curr->prev;
    }
    // check path
    reverse(path.begin(), path.end());
    for (Node *p : path) {
      cout << *p;
    }
  }
}

void writePathTofile(const string &solution_file, const vector<Node *> &path) {
  ofstream outfile(solution_file);
  string line;
  string data;

  if (!outfile.is_open()) {
    cout << "Unable to open " << solution_file << endl;
    exit(1); // terminate with error
  }
  outfile << "[";
  for (int i = 0; i < path.size(); i++) {
    outfile << "(" << path[i]->x << ", " << path[i]->y << ")";
    if (i < path.size() - 1) {
      outfile << ", ";
    }
  }
  outfile << "]";
  outfile.close();
}

void getStaticLaserPath(Laser *laser, vector<Barrier *> barriers, vector<Laser *> laser_path) {
  laser_path.push_back(laser);
  int x = laser->x;
  int y = laser->y;
  int direction = laser->direction;
  int dx = 0;
  int dy = 0;

  switch (direction) {
    case N: dy = 1;
      break;
    case E: dx = 1;
      break;
    case S: dy = -1;
      break;
    case W: dx = -1;
      break;
    default: cout << "not right direction given!" << endl;
      break;
  }

  while (LOWER_BOUND < y && y < UPPER_BOUND && LOWER_BOUND < x && x < UPPER_BOUND) {
    x += dx;
    y += dy;
    Laser *laser_extend = new Laser(x, y, direction);
    if (isBarrier(laser_extend, barriers)) {
      break;
    }
    laser_path.push_back(laser_extend);
  }
//  cout << "laser size = " << laser_path.size() << endl;
}

/*
 * given static direction, get dynamic direction by timestamp
 * */
int getDynamicDirection(int static_direction, int timestamp) {
  int dynamic_direction = (static_direction + timestamp) % 4;
  return dynamic_direction;
}

void getDynamicLaserPath(Laser *laser, vector<Barrier *> &barriers, vector<Laser *> &laser_path, int timestamp) {
//  cout << "get dynamic laser " << endl;
  laser_path.push_back(laser);
  int x = laser->x;
  int y = laser->y;
  int static_direction = laser->direction;
  int dynamic_direction = getDynamicDirection(static_direction, timestamp);
//  cout << "dynamic_direction = " << dynamic_direction << endl;

  int dx = 0;
  int dy = 0;

//  cout << dynamic_direction << endl;
  switch (dynamic_direction) {
    case N: dy = 1;
      break;
    case E: dx = 1;
      break;
    case S: dy = -1;
      break;
    case W: dx = -1;
      break;
    default: cout << "not right direction given!" << endl;
      break;
  }

  while (LOWER_BOUND < y && y < UPPER_BOUND && LOWER_BOUND < x && x < UPPER_BOUND) {
    x += dx;
    y += dy;
    Laser *laser_extend = new Laser(x, y, dynamic_direction);
    if (isBarrier(laser_extend, barriers)) {
      break;
    }
    laser_path.push_back(laser_extend);
  }
//  cout << "laser size = " << laser_path.size() << endl;
}


void checkWormholes(const vector<Holes*>& holes, Node* curr) {
  for (Holes *h : holes) {
    // check pair1
    if (curr->x == h->x1 && curr->y == h->y1){
      curr->x = h->x2;
      curr->y = h->y2;
      return;
    }
    // check pair2
    if (curr->x == h->x2 && curr->y == h->y2){
      curr->x = h->x1;
      curr->y = h->y1;
      return;
    }
  }
}

void findPathAstarWithLasers(Node *origin, Node *destination,
                             vector<Barrier *> &barriers, vector<Laser *> &lasers, vector<Holes *> &holes,
                             vector<Node *> &path) {
  // get laser path
  vector<Laser *> laser_path;
  for (auto la : lasers) {
    getStaticLaserPath(la, barriers, laser_path);
  }

  // first version: only have minimum path
  priority_queue<Node *, vector<Node *>, greater<Node *>> open;
  unordered_set<pair<int, int>, pair_hash> closed;

  origin->gcost = 0;
  open.push(origin);

  int cnt = 0;
  bool reach_flag = false;
  while (!open.empty() && cnt < 5000) {
    cnt++;
    Node *curr = open.top();
    open.pop();
//        curr->visited = true;
    closed.insert(make_pair(curr->x, curr->y));
//        cout << "curr" << *curr << "fcost = " << curr->gcost << ", expand number = " << cnt << endl;
    // check if reached
    if (*curr == *destination) {
      destination = curr;
      cout << "reached!" << endl;
      reach_flag = true;
      break;
    }

    for (int i = 0; i < 4; i++) {
      int nx = curr->x + DELTA_X[i];
      int ny = curr->y + DELTA_Y[i];
      // Assume the grid itself is infinite in size, no need to check boundary condition
      // only check if obs
      Node *next = new Node(nx, ny);
//            cout << *next << isBarrier(next, barriers) << endl;
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);

          // cout << "add next : " << next;
        }
      }
    } // for i
  } // while open

  if (!reach_flag) {
    cout << "can't find path!" << endl;
  } else {
    cout << "PATH" << endl;
    // reconstruct path;
    Node *curr = destination;
    path.push_back(curr);

    while (curr->prev) {
      path.push_back(curr->prev);
      curr = curr->prev;
    }
    // check path
    reverse(path.begin(), path.end());
    for (Node *p : path) {
      cout << *p;
    }
  }

}
/*
 * add timestamp in this function
 * */
void findPathAstarWithDynamicLasers(Node *origin, Node *destination,
                                    vector<Barrier *> &barriers, vector<Laser *> &lasers, vector<Holes *> &holes,
                                    vector<Node *> &path) {
  // get laser path
  vector<Laser *> laser_path;

  // first version: only have minimum path
  priority_queue<Node *, vector<Node *>, greater<Node *>> open;
  unordered_set<pair<int, int>, pair_hash> closed;

  origin->gcost = 0;
  open.push(origin);

  int timestamp = 0;
  int cnt = 0;
  bool reach_flag = false;
  while (!open.empty() && cnt < 2500) {
    cnt++;
    Node *curr = open.top();
    open.pop();
//        curr->visited = true;
    closed.insert(make_pair(curr->x, curr->y));
//    cout << "curr" << *curr << "fcost = " << curr->gcost << ", expand number = " << cnt << endl;
    // check if reached
    if (*curr == *destination) {
      destination = curr;
      cout << "reached!" << endl;
      reach_flag = true;
      break;
    }

    timestamp++;
    laser_path.clear();
    // in this case, laser path will change as timestamp
    getDynamicLaserPath(lasers[0], barriers, laser_path, timestamp);

    for (int i = 0; i < 4; i++) {
      int nx = curr->x + DELTA_X[i];
      int ny = curr->y + DELTA_Y[i];
      // Assume the grid itself is infinite in size, no need to check boundary condition
      // only check if obs
      Node *next = new Node(nx, ny);
//      cout << *next << isBarrier(next, barriers) << endl;
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);

          // cout << "add next : " << next;
        }
      }
    } // for i
  } // while open

  if (!reach_flag) {
    cout << "can't find path!" << endl;
  } else {
    cout << "PATH" << endl;
    // reconstruct path;
    Node *curr = destination;
    path.push_back(curr);

    while (curr->prev) {
      path.push_back(curr->prev);
      curr = curr->prev;
    }
    // check path
    reverse(path.begin(), path.end());
    for (Node *p : path) {
      cout << *p;
    }
  }

}

void findPathAstarWithDynamicLasersAndWormHoles(Node *origin,
                                                Node *destination,
                                                vector<Barrier *> &barriers,
                                                vector<Laser *> &lasers,
                                                vector<Holes *> &holes,
                                                vector<Node *> &path,
                                                int& current_time) {



  // first version: only have minimum path
  priority_queue<Node *, vector<Node *>, greater<Node *>> open;
  unordered_set<pair<int, int>, pair_hash> closed;

  origin->gcost = 0;
  origin->timestamp = 0;
  open.push(origin);

  int cnt = 0;
  bool reach_flag = false;
  while (!open.empty() && cnt < 100000) {
    cnt++;
    Node *curr = open.top();
    open.pop();
    int timestamp = curr->timestamp + 1;

    closed.insert(make_pair(curr->x, curr->y));
//    cout << "curr" << *curr << "fcost = " << curr->gcost << ", expand number = " << cnt << endl;
    // check if reached
    if (*curr == *destination) {
      destination = curr;
      cout << "reached at time " << timestamp << endl;
      current_time = timestamp;
      reach_flag = true;
      break;
    }

//    cout << timestamp << endl;
    vector<Laser *> laser_path;
    if (static_laser_enabled) {
      if (dynamic_laser_enabled) {
        // in this case, laser path will change as timestamp
        for (Laser* la : lasers) {
          getDynamicLaserPath(la, barriers, laser_path, timestamp);
        }

      } else {
        for (Laser* la : lasers) {
          // in this case, laser path will stay the same
          getStaticLaserPath(la, barriers, laser_path);
        }
      }
    }


    // then check wormholes
    // if reach to holes, it will suddenly go to the other side
    // as wormholes only appears every 3 seconds, check at these 3 multiples timestamps
    if (wormholes_enabled) {
      if (timestamp % 3 == 0) {
        checkWormholes(holes, curr);
      }
    }


    for (int i = 0; i < 4; i++) {
      int nx = curr->x + DELTA_X[i];
      int ny = curr->y + DELTA_Y[i];
      // Assume the grid itself is infinite in size, no need to check boundary condition
      // only check if obs
      Node *next = new Node(nx, ny);
//      cout << *next << isBarrier(next, barriers) << endl;
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          next->timestamp = timestamp;
          open.push(next);

          // cout << "add next : " << next;
        }
      }
    } // for i
  } // while open

  if (!reach_flag) {
    cout << "can't find path!" << endl;
  } else {
    cout << "PATH" << endl;
    // reconstruct path;
    Node *curr = destination;
    path.push_back(curr);

    while (curr->prev) {
      path.push_back(curr->prev);
      curr = curr->prev;
    }
    // check path
    reverse(path.begin(), path.end());
    for (Node *p : path) {
      cout << *p;
    }
  }

}

void generateLaserLog(vector<Laser* >lasers, vector<Barrier* >barriers, int end_time, string problem_file) {
  string laser_log_file = "../../robot_routing/" + problem_file + "/laser_log.txt";
  ofstream outfile(laser_log_file);

  if (!outfile.is_open()) {
    cout << "Unable to open " << laser_log_file << endl;
    exit(1); // terminate with error
  }

  int timestamp = 0;
  while (timestamp < end_time) {
    vector<Laser *> laser_path;
    for (Laser* la : lasers) {
      getDynamicLaserPath(la, barriers, laser_path, timestamp);
    }
    // write laser path to file
    for (Laser* lp : laser_path) {
      outfile << "[" << lp->x << ", " << lp->y << "], ";
    }
    outfile << endl;
    timestamp++;
  }

  outfile.close();
}


void usage() {
  cout << "Usage: shell [-fsdw]" << endl;
  cout << "    -f  problem folder name, such as, sample, problem2, etc." << endl;
  cout << "    -s  enable static laser." << endl;
  cout << "    -d  enable dynamic laser." << endl;
  cout << "    -d  enable wormholes." << endl;

  exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
  // arguments passed
  string problem_name = "sample";
  string input_problem_name = "";

  // Parse the command line
  char c;
  while ((c = getopt(argc, argv, "hf:sdw")) != -1) {
    switch (c) {
      case 'h': // Prints help message
        usage();
        break;
      case 'f': // problem file folder
        input_problem_name = optarg;
        break;
      case 's': // static laser
        static_laser_enabled = true;
        break;
      case 'd': // dynamic laser
        dynamic_laser_enabled = true;
        break;
      case 'w': // wormholes
        wormholes_enabled = true;
        break;
      default:
        usage();

    }
  }
  if (input_problem_name.length() > 0) {
    problem_name = input_problem_name;
  } else {
    cout << "No input problem name, use sample instead." << endl;
  }
  cout << "Running " + problem_name + " ..." << endl;


  Node *origin = new Node();
  Node *destination = new Node();
  vector<Barrier *> barriers;
  vector<Laser *> lasers;
  vector<Holes *> holes;
  vector<Node *> path;
  int current_time = 0;

  const string problem_file = "../../robot_routing/" + problem_name + "/problem.txt";
  const string solution_file = "../../robot_routing/" + problem_name + "/solution.txt";

  readInputData(problem_file, origin, destination, barriers, lasers, holes);
//  cout << "bar size = " << barriers.size() << endl;
//    findPathAstarBarriersOnly(origin, destination, barriers, lasers, holes, path);
//    findPathAstarWithLasers(origin, destination, barriers, lasers, holes, path);
//    findPathAstarWithDynamicLasers(origin, destination, barriers, lasers, holes, path);
  findPathAstarWithDynamicLasersAndWormHoles(origin, destination, barriers, lasers, holes, path, current_time);
  writePathTofile(solution_file, path);

  generateLaserLog(lasers, barriers, current_time, problem_name);

  return 0;
}

