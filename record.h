#ifndef RECORD_H
#define RECORD_H

#include <string>

struct Student {
    int id;
    char name[50];   // Fixed-size strings make indexing MUCH easier
    char email[50];
    double marks;
    bool isDeleted;  // The "Soft Delete" flag
};

#endif