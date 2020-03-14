#include "robot_routing_env.h"

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

void generateLaserLog(vector<Laser *> lasers, vector<Barrier *> barriers, int end_time,
                      const string problem_file, const string laser_log_file) {

  ofstream outfile(laser_log_file);

  if (!outfile.is_open()) {
    cout << "Unable to open " << laser_log_file << endl;
    exit(1); // terminate with error
  }

  int timestamp = 0;
  while (timestamp < end_time) {
    vector<Laser *> laser_path;
    for (Laser *la : lasers) {
      getDynamicLaserPath(la, barriers, laser_path, timestamp);
    }
    // write laser path to file
    for (Laser *lp : laser_path) {
      outfile << "[" << lp->x << ", " << lp->y << "], ";
    }
    outfile << endl;
    timestamp++;
  }

  outfile.close();
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

void checkWormholes(const vector<Holes *> &holes, Node *curr) {
  for (Holes *h : holes) {
    // check pair1
    if (curr->x == h->x1 && curr->y == h->y1) {
      curr->x = h->x2;
      curr->y = h->y2;
      return;
    }
    // check pair2
    if (curr->x == h->x2 && curr->y == h->y2) {
      curr->x = h->x1;
      curr->y = h->y1;
      return;
    }
  }
}

