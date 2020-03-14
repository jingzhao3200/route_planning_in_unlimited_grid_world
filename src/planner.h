#ifndef RIDEOS_OA_SRC_PLANNER_H_
#define RIDEOS_OA_SRC_PLANNER_H_

#include "robot_routing_env.h"

void findPathAstarWithDynamicLasersAndWormHoles(Node *origin,
                                                Node *destination,
                                                vector<Barrier *> &barriers,
                                                vector<Laser *> &lasers,
                                                vector<Holes *> &holes,
                                                vector<Node *> &path,
                                                int &current_time,
                                                bool static_laser_enabled,
                                                bool dynamic_laser_enabled,
                                                bool wormholes_enabled);

#endif //RIDEOS_OA_SRC_PLANNER_H_
