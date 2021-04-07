/* *
 * @project barnesHut
 * @file TestManager.cpp.cc
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/28/21
 */

#include <string>
#include <filesystem>
#include <fstream>
#include "TestManager.h"
#include "Body.h"

namespace fs = std::filesystem;


TestManager::TestManager(vector<Body *> *bodies) : bodies(bodies) {
    findTests();
    if(pathExists){
        selected = tests.front();
        currentTest = tests.front();
        getBodies();
    }


}

void TestManager::findTests() {
    string path = "./tests";
    fs::directory_iterator di;
    try{
        di = fs::directory_iterator(path);
    }catch(fs::filesystem_error &e) {
        printf("%s", e.what());
        pathExists = false;
        return;
    }
    pathExists = true;
    for (const auto &entry : di) {
        if (entry.path().extension().string() == ".in") {
            tests.push_back(
                    new Test{entry.path().string(), entry.path().filename()
                    .string()});
        }
    }

}

void TestManager::getBodies() {
    bodies->clear();
    std::ifstream file(currentTest->path);
    if (file.is_open()) {
        while (!file.eof()) {
            float px = 0, py = 0, pz = 0, vx = 0, vy = 0, vz = 0, mass = 0;
            file >> px;
            file >> py;
            file >> pz;
            file >> vx;
            file >> vy;
            file >> vz;
            file >> mass;
            Body *b = new Body(px, py, pz, vx, vy, vz, 0, 0, 0, mass);
            bodies->push_back(b);
        }
    }
}

vector<Test *> TestManager::getTests() {
    return tests;
}

void TestManager::selectTest() {
    currentTest = selected;
    getBodies();
}

Test *TestManager::getTest() {
    return currentTest;
}

void TestManager::nextTest() {
    if(selected == tests.back()){
        selected = tests.front();
        return;
    }
    auto it = std::find(tests.begin(), tests.end(), selected);
    auto nx = std::next(it, 1);
    selected = *nx;
}

Test *TestManager::getSelected() {
    return selected;
}

void TestManager::lastTest() {
    if(selected == tests.front()){
        selected = tests.back();
        return;
    }
    auto it = std::find(tests.begin(), tests.end(), selected);
    auto nx = std::next(it, -1);
    selected = *nx;
}
