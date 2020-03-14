#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <getopt.h>
#include "robot_routing_env.h"
#include "planner.h"

using namespace std;

// global variables: default values for env setting flags
bool static_laser_enabled_flag = false;
bool dynamic_laser_enabled_flag = false;
bool wormholes_enabled_flag = false;

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
        static_laser_enabled_flag = true;
        break;
      case 'd': // dynamic laser
        dynamic_laser_enabled_flag = true;
        break;
      case 'w': // wormholes
        wormholes_enabled_flag = true;
        break;
      default:usage();

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

  const string problem_file = "../robot_routing/" + problem_name + "/problem.txt";
  const string solution_file = "../robot_routing/" + problem_name + "/solution.txt";
  const string laser_log_file = "../robot_routing/" + problem_name + "/laser_log.txt";

  readInputData(problem_file, origin, destination, barriers, lasers, holes);
  findPathAstarWithDynamicLasersAndWormHoles(origin, destination, barriers, lasers, holes, path,
      current_time, static_laser_enabled_flag, dynamic_laser_enabled_flag, wormholes_enabled_flag);

  writePathTofile(solution_file, path);
  generateLaserLog(lasers, barriers, current_time, problem_name, laser_log_file);

  return 0;
}

