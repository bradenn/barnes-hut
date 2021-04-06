/* *
 * @project barnesHut
 * @file TestManager.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/28/21
 */

#ifndef BARNESHUT_TESTMANAGER_H
#define BARNESHUT_TESTMANAGER_H

#include<vector>
#include<string>
#include<filesystem>
#include "Body.h"

namespace fs = std::filesystem;
using std::vector, std::string;

struct Test {
    string path;
    string name;
};

class TestManager {
private:
    vector<Test *> tests;
    Test *currentTest;
    Test *selected;
    vector<Body *> *bodies;
    void findTests();

public:
    vector<Test *> getTests();

    explicit TestManager(vector<Body *> *bodies);

    void selectTest();
    void nextTest();
    void lastTest();
    Test *getTest();
    Test *getSelected();
    void getBodies();

};


#endif //BARNESHUT_TESTMANAGER_H
