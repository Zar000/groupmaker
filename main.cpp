#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

typedef struct {
    std::string name = "";
    int hasArduino = 0;
} person;

person createPerson(const std::string& name, int hasArduino) {
    person newPerson;
    newPerson.name = name;
    newPerson.hasArduino = hasArduino;
    return newPerson;
}

std::vector<person> readPeopleFromFile(const std::string& filename) {
    std::vector<person> peopleList;
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return peopleList;
    }
    std::string firstName, lastName;
    int hasArduino;
    while (inputFile >> firstName >> lastName >> hasArduino) {
        std::string fullName = firstName + " " + lastName;
        person newPerson = createPerson(fullName, hasArduino);
        peopleList.push_back(newPerson);
    }
    inputFile.close();
    return peopleList;
}

std::vector<std::vector<person>> groupPeople(std::vector<person>& peopleList, int groupSize) {
    std::vector<person> withArd, withoutArd;
    for (const auto& per : peopleList) {
        if (per.hasArduino) {
            withArd.push_back(per);
        } else {
            withoutArd.push_back(per);
        }
    }
    std::random_shuffle(withArd.begin(), withArd.end());
    std::random_shuffle(withoutArd.begin(), withoutArd.end());
    std::vector<std::vector<person>> groups;
    while (!withArd.empty() || !withoutArd.empty()) {
        std::vector<person> currGroup;
        if (!withArd.empty()) {
            currGroup.push_back(withArd.back());
            withArd.pop_back();
        }
        while (currGroup.size() < groupSize && !withoutArd.empty()) {
            currGroup.push_back(withoutArd.back());
            withoutArd.pop_back();
        }
        while (currGroup.size() < groupSize && !withArd.empty()) {
            currGroup.push_back(withArd.back());
            withArd.pop_back();
        }
        groups.push_back(currGroup);
    }
    std::vector<person> overflow;
    for (int i = 0; i < groups.size(); ++i) {
        if (groups[i].size() < groupSize) {
            overflow.insert(overflow.end(), groups[i].begin(), groups[i].end());
            groups.erase(groups.begin() + i);
            --i;
        }
    }
    for (auto& person : overflow) {
        for (auto& group : groups) {
            if (group.size() < groupSize) {
                group.push_back(person);
                break;
            }
        }
    }
    return groups;
}

void writeGroupsToFile(const std::vector<std::vector<person>>& groups, const std::string& filename) {
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }
    for (size_t i = 0; i < groups.size(); ++i) {
        outputFile << "Group " << (i + 1) << ":\n";
        for (const auto& per : groups[i]) {
            outputFile << "  Name: " << per.name
                       << ", Has Arduino: " << (per.hasArduino ? "Yes" : "No") << "\n";
        }
        outputFile << "\n";
    }
    outputFile.close();
}

int main() {
    std::string inputFilename, outputFilename;
    int groupSize;
    std::cout << "Enter the input filename (e.g., people.txt): ";
    std::cin >> inputFilename;
    std::cout << "Enter the group size: ";
    std::cin >> groupSize;
    std::cout << "Enter the output filename (e.g., groups.txt): ";
    std::cin >> outputFilename;
    std::vector<person> peopleList = readPeopleFromFile(inputFilename);
    if (peopleList.empty()) {
        std::cout << "No people were loaded from the file. Exiting.\n";
        return 0;
    }
    std::vector<std::vector<person>> groups = groupPeople(peopleList, groupSize);
    writeGroupsToFile(groups, outputFilename);
    std::cout << "Groups have been written to " << outputFilename << "\n";
    return 0;
}
