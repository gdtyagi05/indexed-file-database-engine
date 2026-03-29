#ifndef DATABASE_H
#define DATABASE_H

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include "record.h"

class Database {
    private:

        std :: string fileName;
        std :: fstream file;
        // The Index : Map ID-> byte location
        std :: map<int , std:: streampos> index;
        void buildIndex();

    public:

        Database(std::string name);
        ~Database();

        // CRUD Operations
        bool addStudent(Student s);
        Student getStudent(int id);
        bool updateStudent(int id , double newMarks);
        bool deleteStudent(int id);


        void displayAll();


};

#endif
