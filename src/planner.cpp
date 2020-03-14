#include "robot_routing_env.h"

void findPathAstarBarriersOnly(Node *origin, Node *destination,
                               vector<Barrier *> &barriers,
                               vector<Laser *> &lasers,
                               vector<Holes *> &holes,
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
    closed.insert(make_pair(curr->x, curr->y));

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
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);
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

void findPathAstarWithLasers(Node *origin, Node *destination,
                             vector<Barrier *> &barriers,
                             vector<Laser *> &lasers,
                             vector<Holes *> &holes,
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
    closed.insert(make_pair(curr->x, curr->y));
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
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);
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
                                    vector<Barrier *> &barriers,
                                    vector<Laser *> &lasers,
                                    vector<Holes *> &holes,
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
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          open.push(next);
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
                                                int &current_time,
                                                bool static_laser_enabled,
                                                bool dynamic_laser_enabled,
                                                bool wormholes_enabled) {

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
    // check if reached
    if (*curr == *destination) {
      destination = curr;
      cout << "reached at time " << timestamp << endl;
      current_time = timestamp;
      reach_flag = true;
      break;
    }

    vector<Laser *> laser_path;
    if (static_laser_enabled) {
      if (dynamic_laser_enabled) {
        // in this case, laser path will change as timestamp
        for (Laser *la : lasers) {
          getDynamicLaserPath(la, barriers, laser_path, timestamp);
        }

      } else {
        for (Laser *la : lasers) {
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
      if (closed.count(make_pair(nx, ny)) < 1 && (!isBarrier(next, barriers)) && (!isLaser(next, laser_path))) {
        // update gcost and put to open list
        if (next->gcost > curr->gcost + 1) {
          next->gcost = curr->gcost + 1;
          next->fcost = next->gcost + MahattanDistance(next, destination);
          next->prev = curr;
          next->timestamp = timestamp;
          open.push(next);
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

