#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <queue>
#include <cstdlib>

using namespace std;

const int DELTA_X[4] = {-1, 0, 1, 0};
const int DELTA_Y[4] = {0, -1, 0, 1};

class Node {
public:
    int x;
    int y;
    bool isObs;
    int cost;  // cost from start point

    Node() : x(0), y(0), isObs(false), cost(INT_MAX) {}
    Node(int _x, int _y) : x(_x), y(_y), isObs(false), cost(INT_MAX)  {}
    Node(int _x, int _y, bool obs_flag) : x(_x), y(_y), isObs(obs_flag) , cost(INT_MAX) {}

    bool operator< (const Node& p2) const {
        return cost < p2.cost;
    }

    friend ostream&operator<< (ostream& os, const Node& Node) {
        os << "Node : [" << Node.x << ", " << Node.y << "]" << endl;
        return os;
    }
};


class Barrier : public Node {
public:
    Barrier(int _x, int _y) : Node(_x, _y, true) {}

    friend ostream&operator<< (ostream& os, const Barrier& barrier) {
        os << "Barrier : [" << barrier.x<< ", " << barrier.y << "]" << endl;
        return os;
    }
};

class Laser {
public:
    Laser(int x, int y, char direction) : x_(x), y_(y), direction_(direction) {}

    void set(int x, int y, char direction) {
        x_ = x; y_ = y; direction_ = direction;
    }

    friend ostream& operator<< (ostream& os, const Laser& laser) {
        os << "L: (" << laser.x_ << ", " << laser.y_ << ", " << laser.direction_ << ")" << endl;
        return os;
    }

private:
    int x_;
    int y_;
    char direction_;
};


class Holes {
public:
    Holes(int x1, int y1, int x2, int y2) : x1_(x1), y1_(y1), x2_(x2), y2_(y2){}

    friend ostream &operator<< (ostream& os, const Holes& holes) {
        os << "holes at : [" << holes.x1_ << ", " << holes.y1_
           << "], [" << holes.x2_ << ", " << holes.y2_ << "]" << endl;
        return os;
    }

private:
    int x1_;
    int y1_;
    int x2_;
    int y2_;
};


void parseIntFromString(string& data, int& number, int& Node1, int& Node2) {
    // get int from the read data
    while(!isdigit(data[Node1])) {
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
void parseIntFromString(string& data, int& number) {
    int Node1 = 0;
    int Node2 = 0;
    // get int from the read data
    while(!isdigit(data[Node1])) {
        Node1++;
    }
    Node2 = Node1;
    while (isdigit(data[Node2])) {
        Node2++;
    }
    string sub_data = data.substr(Node1, Node2 - Node1);
    number = stoi(sub_data);
}


void parseDirectionFromString(string& data, char& direction, int& Node1, int& Node2) {
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
void parseDirectionFromString(string& data, char& direction) {
    int Node1 = 0;
    int Node2 = 0;
    // get int from the read data
    while (data[Node1] != 39) { // ascii ' is 39
        Node1++;
    }
    Node2 = Node1 + 2;
    direction = data[Node1 + 1];
}

/*
 * read one pair from the string to a vactor
 * */
void readOnePair(ifstream& infile, Node& Node, string& data) {
    int number = 0;
    // read x
    infile >> data;
    parseIntFromString(data, number);
    Node.x = number;

    // read y
    infile >> data;
    parseIntFromString(data, number);
    Node.y = number;
}


void getBarrierCoordinatesFromLine(vector<Barrier>& barriers, string& line) {
    istringstream instr(line);
    string temp;
    int number;
    vector<int> barrier_coordinates(2, 0);
    int i = 0;
    while (instr >> temp) {
//        cout << "barrier :" << temp << endl;
        // check if no space between x and y
        temp = temp.substr(0, temp.length() - 1);
        const char* temp_c = temp.c_str();
        const char* ptr = strchr( temp_c, ',');
        if (ptr != nullptr) {
            // ',' exist in temp, need to parse twice
            string sub_temp1 = temp.substr(0, ptr - temp_c + 1);
            string sub_temp2 = temp.substr(ptr - temp_c + 1);
//            cout << "sep 1: " << sub_temp1 << endl;
//            cout << "sep 2: " << sub_temp2 << endl;
            parseIntFromString(sub_temp1, number);
            barrier_coordinates[i%2] = number;
            i++;
            parseIntFromString(sub_temp2, number);
            barrier_coordinates[i%2] = number;
//            i++;

        } else {
            parseIntFromString(temp, number);
            barrier_coordinates[i%2] = number;
//            cout << number << endl;
        }


        if ((i+1) % 2 == 0) {
            Barrier barrier = Barrier(barrier_coordinates[i%2-1],barrier_coordinates[i%2]);
            barriers.push_back(barrier);
            cout << barrier;
        }
        i++;
    }
}


void getLaserCoordinatesFromLine(vector<Laser>& lasers, string& line) {
    istringstream instr(line);
    string temp;
    int number;
    char direction;
    int i = 0;
    vector<int> laser_coordinates(3, 0);

    while (instr >> temp) {
//        cout << i << ": "<< temp << endl;
        if ((i+1) % 3) {
            parseIntFromString(temp, number);
            laser_coordinates[i%3] = number;
        } else {
            parseDirectionFromString(temp, direction);
            laser_coordinates[i%3] = direction;
            Laser laser_ptr = Laser(laser_coordinates[i%3-2], laser_coordinates[i%3-1], laser_coordinates[i%3]);
            lasers.push_back(laser_ptr);
            cout << laser_ptr;
        }
        i++;
    }
}


void getHolesCoordinatesFromLine(vector<Holes>& holes, string& line) {
    istringstream instr(line);
    string temp;
    int number;
    vector<int> hole_coordinates(4, 0);
    int i = 0;
    while (instr >> temp) {
//        cout << i << endl;
//        cout << "holes :" << temp << endl;
        parseIntFromString(temp, number);
        hole_coordinates[i%4] = number;
//        cout << number << endl;
        if ((i+1) % 4 == 0 && i != 0) {
            Holes hole = Holes(hole_coordinates[i%4-3], hole_coordinates[i%4-2],
                                    hole_coordinates[i%4-1],hole_coordinates[i%4]);
            holes.push_back(hole);
            cout << hole;
        }
        i++;
    }
}

/*
 * readInputData: read the problem file
 * */
void readInputData(const string& problem_file, Node& origin, Node& destination,
        vector<Barrier>& barriers, vector<Laser>& lasers, vector<Holes>& holes) {

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

    cout << "origin: " << origin.x << ", "<< origin.y << endl;
    cout << "destination: " << destination.x << ", "<< destination.y << endl;

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
}


int main() {

    Node origin = Node();
    Node destination = Node();
    vector<Barrier> barriers;
    vector<Laser> lasers;
    vector<Holes> holes;

    const string problem_file = "robot_routing/sample/problem_test.txt";
    readInputData(problem_file, origin, destination, barriers, lasers, holes);

    // first version: only have minimum path
    priority_queue<Node> node_q;

    origin.cost = 0;
    node_q.push(origin);

    while (!node_q.empty()) {
        Node curr = node_q.top(); node_q.pop();
        for (int i=0; i<4; i++) {
            int nx = curr.x + DELTA_X[i];
            int ny = curr.y + DELTA_Y[i];
        }
    }













    return 0;
}

