#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <fstream>
#include <map>
#include "record.h"

class Database {
private:
    std::string fileName;
    std::fstream file;
    
    // The Index: Maps Student ID -> Physical Byte Location in file
    std::map<int, std::streampos> index;

    // Internal engine function to scan file and populate the map
    void buildIndex(); 

public:
    // Constructor: Opens file and builds the RAM index
    Database(std::string name);

    // CREATE: Adds a new record to the end of the file
    bool addStudent(const Student& s);

    // READ: Uses the map for O(log n) lookup
    bool getStudent(int id, Student& result);

    // UPDATE: Overwrites data at a specific byte offset
    bool updateMarks(int id, double newMarks);

    // DELETE: Marks record as deleted on disk (Soft Delete)
    bool deleteStudent(int id);

    // MAINTENANCE: Reclaims space from deleted records
    void compact();

    // UTILITY: Prints all active records
    void displayAll();

    // Destructor: Ensures file is closed safely
    ~Database();
};

#endif