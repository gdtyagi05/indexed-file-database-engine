#include "database.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <cstdio> 

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
    file.clear(); // Clear any existing stream state flags before seeking
    file.seekg(0, std::ios::beg);

    Student s;
    while (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
        std::streampos pos = file.tellg() - static_cast<std::streamoff>(sizeof(Student));
        
        if (!s.isDeleted) {
            index[s.id] = pos;
        }
    }
    file.clear(); // Clear EOF flag set by the sequential scanning loop
}

// CREATE: Includes Duplicate ID check
bool Database::addStudent(const Student& s) {
    if (index.find(s.id) != index.end()) {
        std::cerr << "Error: ID " << s.id << " already exists in system.\n";
        return false;
    }

    file.clear();
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
    auto it = index.find(id);
    if (it == index.end()) return false;

    file.clear();
    file.seekg(it->second, std::ios::beg);
    if (file.read(reinterpret_cast<char*>(&result), sizeof(Student))) {
        return true;
    }
    return false;
}

// UPDATE: In-Place Binary Overwrite
bool Database::updateMarks(int id, double newMarks) {
    auto it = index.find(id);
    if (it == index.end()) return false;

    std::streampos pos = it->second;
    Student s;
    
    file.clear();
    file.seekg(pos, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(&s), sizeof(Student))) return false;
    
    s.marks = newMarks;

    file.seekp(pos, std::ios::beg);
    if (file.write(reinterpret_cast<const char*>(&s), sizeof(Student))) {
        file.flush();
        return true;
    }
    return false;
}

// DELETE: Soft Delete (Efficiency O(1))
bool Database::deleteStudent(int id) {
    auto it = index.find(id);
    if (it == index.end()) return false;

    std::streampos pos = it->second;
    Student s;

    file.clear();
    file.seekg(pos, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(&s), sizeof(Student))) return false;

    s.isDeleted = true; 

    file.seekp(pos, std::ios::beg);
    if (file.write(reinterpret_cast<const char*>(&s), sizeof(Student))) {
        file.flush();
        index.erase(it); // Erase using iterator for optimal performance
        return true;
    }
    return false;
}

// MAINTENANCE: Physically removes deleted records safely
void Database::compact() {
    if (index.empty()) return;

    std::string tempName = "temp_" + fileName;
    std::ofstream tempFile(tempName, std::ios::binary);
    
    if (!tempFile.is_open()) {
        std::cerr << "Maintenance Error: Compaction failed to initialize temporary storage.\n";
        return;
    }

    std::map<int, std::streampos> newIndex;
    file.clear(); // Critical state-reset before processing seeks

    for (auto it = index.begin(); it != index.end(); ++it) {
        int id = it->first;
        std::streampos oldPos = it->second;

        Student s;
        file.seekg(oldPos, std::ios::beg);
        if (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            std::streampos newPos = tempFile.tellp();
            if (tempFile.write(reinterpret_cast<const char*>(&s), sizeof(Student))) {
                newIndex[id] = newPos; 
            }
        }
    }

    tempFile.close();
    file.close();

    // Atomic Filesystem Swap Operations
    std::remove(fileName.c_str());
    std::rename(tempName.c_str(), fileName.c_str());

    // Re-establish primary engine data streams
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
    
    file.clear();
    for (auto it = index.begin(); it != index.end(); ++it) {
        std::streampos pos = it->second;
        Student s;
        file.seekg(pos, std::ios::beg);
        if (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            std::cout << std::left << std::setw(10) << s.id 
                      << std::setw(20) << s.name 
                      << std::setw(25) << s.email 
                      << s.marks << std::endl;
        }
    }
}

Database::~Database() {
    if (file.is_open()) file.close();
}
