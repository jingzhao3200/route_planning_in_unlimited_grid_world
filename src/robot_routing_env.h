
#ifndef RIDEOS_OA_SRC_ROBOT_ROUTING_ENV_H_
#define RIDEOS_OA_SRC_ROBOT_ROUTING_ENV_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <cstdlib>
#include <unordered_set>
#include <getopt.h>
#include <unistd.h>

using namespace std;

const int DELTA_X[4] = {-1, 0, 1, 0};
const int DELTA_Y[4] = {0, -1, 0, 1};
const int MAX_COST = 1e6;
const int LOWER_BOUND = -20;
const int UPPER_BOUND = 20;



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

// math
int MahattanDistance(Node *n1, Node *n2);

// parse string
void parseIntFromString(string &data, int &number, int &Node1, int &Node2);
void parseIntFromString(string &data, int &number);
void parseDirectionFromString(string &data, int &direction, int &Node1, int &Node2);
void parseDirectionFromString(string &data, int &direction);
void readOnePair(ifstream &infile, Node *Node, string &data);
void getBarrierCoordinatesFromLine(vector<Barrier *> &barriers, string &line);
void getLaserCoordinatesFromLine(vector<Laser *> &lasers, string &line);
void getHolesCoordinatesFromLine(vector<Holes *> holes, string &line);
void readInputData(const string &problem_file, Node *origin, Node *destination,
                   vector<Barrier *> &barriers, vector<Laser *> &lasers, vector<Holes *> &holes);

// write result
void generateLaserLog(vector<Laser *> lasers, vector<Barrier *> barriers, int end_time, string problem_file, string
laser_log_file);
void writePathTofile(const string &solution_file, const vector<Node *> &path);

// laser
void getStaticLaserPath(Laser *laser, vector<Barrier *> barriers, vector<Laser *> laser_path);
int getDynamicDirection(int static_direction, int timestamp);
void getDynamicLaserPath(Laser *laser, vector<Barrier *> &barriers, vector<Laser *> &laser_path, int timestamp);

// check
bool isBarrier(const Node *node, const vector<Barrier *> &barriers);
bool isLaser(const Node *node, const vector<Laser *> &laser_path);
void checkWormholes(const vector<Holes *> &holes, Node *curr);

#endif //RIDEOS_OA_SRC_ROBOT_ROUTING_ENV_H_
