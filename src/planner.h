#ifndef RIDEOS_OA_SRC_PLANNER_H_
#define RIDEOS_OA_SRC_PLANNER_H_
#include <iostream>     // std::cout
#include <algorithm>    // std::reverse
#include <vector>       // std::vector
#include "robot_routing_env.h"

using namespace std;

void findPathAstarBarriersOnly(Node *origin, Node *destination,
                               vector<Barrier *> &barriers,
                               vector<Laser *> &lasers,
                               vector<Holes *> &holes,
                               vector<Node *> &path);

void findPathAstarWithLasers(Node *origin, Node *destination,
                             vector<Barrier *> &barriers,
                             vector<Laser *> &lasers,
                             vector<Holes *> &holes,
                             vector<Node *> &path);

void findPathAstarWithDynamicLasers(Node *origin, Node *destination,
                                    vector<Barrier *> &barriers,
                                    vector<Laser *> &lasers,
                                    vector<Holes *> &holes,
                                    vector<Node *> &path);


void findPathAstarWithDynamicLasersAndWormHoles(Node *origin, Node *destination,
                                                vector<Barrier *> &barriers,
                                                vector<Laser *> &lasers,
                                                vector<Holes *> &holes,
                                                vector<Node *> &path,
                                                int &current_time,
                                                bool static_laser_enabled,
                                                bool dynamic_laser_enabled,
                                                bool wormholes_enabled);

#endif //RIDEOS_OA_SRC_PLANNER_H_
