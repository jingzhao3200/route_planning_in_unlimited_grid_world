#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <string.h>

using namespace std;

class Barrier {
public:
    Barrier (int x, int y) : x_(x), y_(y) {}
    friend ostream&operator<< (ostream& os, const Barrier& barrier) {
        os << "Barrier : [" << barrier.x_ << ", " << barrier.y_ << "]" << endl;
        return os;
    }

private:
    int x_;
    int y_;
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

void parseIntFromString(string& data, int& number, int& pos1, int& pos2) {
    // get int from the read data
    while(!isdigit(data[pos1])) {
        pos1++;
    }
    pos2 = pos1;
    while (isdigit(data[pos2])) {
        pos2++;
    }
    string sub_data = data.substr(pos1, pos2 - pos1);
    number = stoi(sub_data);
}

void parseIntFromString(string& data, int& number) {
    int pos1 = 0;
    int pos2 = 0;
    // get int from the read data
    while(!isdigit(data[pos1])) {
        pos1++;
    }
    pos2 = pos1;
    while (isdigit(data[pos2])) {
        pos2++;
    }
    string sub_data = data.substr(pos1, pos2 - pos1);
    number = stoi(sub_data);
}

void parseDirectionFromString(string& data, char& direction, int& pos1, int& pos2) {
    // get int from the read data
    while (data[pos1] != 39) { // ascii ' is 39
        pos1++;
    }
    pos2 = pos1 + 2;
    direction = data[pos1 + 1];
}

void parseDirectionFromString(string& data, char& direction) {
    int pos1 = 0;
    int pos2 = 0;
    // get int from the read data
    while (data[pos1] != 39) { // ascii ' is 39
        pos1++;
    }
    pos2 = pos1 + 2;
    direction = data[pos1 + 1];
}


void readOnePair(ifstream& infile, vector<int>& vec, string& data) {
    for (int i=0; i<2; i++) {
        infile >> data;
        int number = 0;
        int pos1 = 0;
        int pos2 = 0;
        parseIntFromString(data, number, pos1, pos2);
        vec.push_back(number);
    }
}

// TODO: add comma seperation
void getBarrierCoordinatesFromLine(vector<Barrier*>& barriers, string& line) {
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
        if (ptr != NULL) {
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
            Barrier* barrier = new Barrier(barrier_coordinates[i%2-1],barrier_coordinates[i%2]);
            barriers.push_back(barrier);
            cout << *barrier;
        }
        i++;
    }
}

void getLaserCoordinatesFromLine(vector<Laser*>& lasers, string& line) {
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
            Laser* laser_ptr = new Laser(laser_coordinates[i%3-2], laser_coordinates[i%3-1], laser_coordinates[i%3]);
            lasers.push_back(laser_ptr);
            cout << *laser_ptr;
        }
        i++;
    }
}

void getHolesCoordinatesFromLine(vector<Holes*>& holes, string& line) {
    istringstream instr(line);
    string temp;
    int number;
    vector<int> hole_coordinates(4, 0);
    int i = 0;
    while (instr >> temp) {
//        cout << "holes :" << temp << endl;
        parseIntFromString(temp, number);
        hole_coordinates[i%4] = number;
//        cout << number << endl;
        if (i % 3 == 0 && i != 0) {
            Holes* hole = new Holes(hole_coordinates[i%4-3], hole_coordinates[i%4-2],
                                    hole_coordinates[i%4-1],hole_coordinates[i%4]);
            holes.push_back(hole);
            cout << *hole;
        }
        i++;
    }
}


int main() {

    vector<int> origin, destination;
    vector<Barrier*> barriers;
    vector<Laser*> lasers;

    // open a file in read mode.
    ifstream infile;
    const string problem_file = "robot_routing/sample/problem_test.txt";
    infile.open(problem_file);
    string data;

    if (!infile) {
        cout << "Unable to open " << problem_file << endl;
        exit(1); // terminate with error
    }
    // read origin and destination
    readOnePair(infile, origin, data);
    cout << "origin: " << origin[0] << ", "<< origin[1] << endl;
    readOnePair(infile, destination, data);
    cout << "destination: " << destination[0] << ", "<< destination[1] << endl;

    // read barrier
    string line;

    int i = 0;
    int pos1 = 0;
    int pos2 = 0;
    getline(infile, line); // empty line


    // read barrier coordinates
    if (getline(infile, line)) {
        cout << "barrier LINE :" << line << endl;
        getBarrierCoordinatesFromLine(barriers, line);

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
        vector<Holes*> holes;
        getHolesCoordinatesFromLine(holes, line);
    } else {
        cout << "no wormholes!" << endl;
    }










    return 0;
}
