#include "database.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <cstdio> // Required for std::remove and std::rename

// Constructor: Handles file creation and initial indexing
Database::Database(std::string name) : fileName(name) {
    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::ofstream create(fileName, std::ios::binary);
        create.close();
        file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
        
        if (!file.is_open()) {
            throw std::runtime_error("CRITICAL: Data file is locked or access denied.");
        }
    }
    
    buildIndex(); 
}

// THE ENGINE: Scans the disk to build the RAM-based Offset Map
void Database::buildIndex() {
    index.clear();
    file.seekg(0, std::ios::beg);

    Student s;
    while (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
        std::streampos pos = file.tellg() - (std::streamoff)sizeof(Student);
        
        if (!s.isDeleted) {
            index[s.id] = pos;
        }
    }
    file.clear(); 
}

// CREATE: Includes Duplicate ID check
bool Database::addStudent(const Student& s) {
    if (index.find(s.id) != index.end()) {
        std::cerr << "Error: ID " << s.id << " already exists in system.\n";
        return false;
    }

    file.seekp(0, std::ios::end);
    std::streampos pos = file.tellp();

    if (file.write(reinterpret_cast<const char*>(&s), sizeof(Student))) {
        file.flush(); 
        index[s.id] = pos; 
        return true;
    }
    return false;
}

// READ: O(log n) Instant Search
bool Database::getStudent(int id, Student& result) {
    if (index.find(id) == index.end()) return false;

    file.seekg(index[id], std::ios::beg);
    file.read(reinterpret_cast<char*>(&result), sizeof(Student));
    return true;
}

// UPDATE: In-Place Binary Overwrite
bool Database::updateMarks(int id, double newMarks) {
    if (index.find(id) == index.end()) return false;

    std::streampos pos = index[id];
    Student s;
    
    file.seekg(pos, std::ios::beg);
    file.read(reinterpret_cast<char*>(&s), sizeof(Student));
    
    s.marks = newMarks;

    file.seekp(pos, std::ios::beg);
    file.write(reinterpret_cast<char*>(&s), sizeof(Student));
    file.flush();
    return true;
}

// DELETE: Soft Delete (Efficiency O(1))
bool Database::deleteStudent(int id) {
    if (index.find(id) == index.end()) return false;

    std::streampos pos = index[id];
    Student s;

    file.seekg(pos, std::ios::beg);
    file.read(reinterpret_cast<char*>(&s), sizeof(Student));

    s.isDeleted = true; 

    file.seekp(pos, std::ios::beg);
    file.write(reinterpret_cast<char*>(&s), sizeof(Student));
    file.flush();

    index.erase(id); 
    return true;
}

// NEW MAINTENANCE: Physically removes deleted records
void Database::compact() {
    if (index.empty()) return;

    std::string tempName = "temp_" + fileName;
    std::ofstream tempFile(tempName, std::ios::binary);
    
    if (!tempFile.is_open()) return;

    std::map<int, std::streampos> newIndex;

    // Use an iterator to avoid "id" and "pos" being undefined
    for (auto it = index.begin(); it != index.end(); ++it) {
        int id = it->first;
        std::streampos oldPos = it->second;

        Student s;
        file.seekg(oldPos, std::ios::beg);
        file.read(reinterpret_cast<char*>(&s), sizeof(Student));

        std::streampos newPos = tempFile.tellp();
        tempFile.write(reinterpret_cast<char*>(&s), sizeof(Student));
        
        newIndex[id] = newPos; 
    }

    tempFile.close();
    file.close();

    std::remove(fileName.c_str());
    std::rename(tempName.c_str(), fileName.c_str());

    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    index = newIndex;
}

// UTILITY: Sequential Scan of the Index
void Database::displayAll() {
    if (index.empty()) {
        std::cout << "\n[System] No active records found.\n";
        return;
    }

    std::cout << "\n" << std::left << std::setw(10) << "ID" 
              << std::setw(20) << "Name" 
              << std::setw(25) << "Email" 
              << "Marks" << std::endl;
    std::cout << std::string(65, '=') << std::endl;
    
    // Changing the loop to use 'it' prevents the undefined error
    for (auto it = index.begin(); it != index.end(); ++it) {
        std::streampos pos = it->second;
        Student s;
        file.seekg(pos, std::ios::beg);
        file.read(reinterpret_cast<char*>(&s), sizeof(Student));
        
        std::cout << std::left << std::setw(10) << s.id 
                  << std::setw(20) << s.name 
                  << std::setw(25) << s.email 
                  << s.marks << std::endl;
    }
}

Database::~Database() {
    if (file.is_open()) file.close();
}