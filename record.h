#ifndef RECORD_H
#define RECORD_H

#include <cstring> // Required for memset and strncpy

struct Student {
    int id;
    char name[50];
    char email[50];
    double marks;
    bool isDeleted;

    // Constructor to initialize a clean record
    Student() : id(0), marks(0.0), isDeleted(false) {
        // Zero out the char arrays to prevent "garbage" data in the file
        std::memset(name, 0, sizeof(name));
        std::memset(email, 0, sizeof(email));
    }
};

#endif