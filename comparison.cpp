#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <cstdio>
#include "database.h"
#include "record.h"

// Simulated traditional full-file scan search (O(N) Baseline)
bool linearSearchDisk(const std::string &filename, int targetId, Student &result)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;

    Student s;
    while (file.read(reinterpret_cast<char *>(&s), sizeof(Student)))
    {
        if (!s.isDeleted && s.id == targetId)
        {
            result = s;
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

int main()
{
    std::string filename = "benchmark_scale.dat";
    const int SCALE_LIMIT = 1000000; // 1 Million Records

    std::remove(filename.c_str());

    std::cout << "=================================================================\n";
    std::cout << "SCALING TEST: RUNNING 1,000,000 BINARY RECORDS\n";
    std::cout << "=================================================================\n\n";

    try
    {
        Database db(filename);

        // 1. MASSIVE BULK INSERTION
        std::cout << "Phase 1: Committing 1,000,000 records to disk...\n";

        auto insertStart = std::chrono::high_resolution_clock::now();

        for (int i = 1; i <= SCALE_LIMIT; ++i)
        {
            Student s;
            s.id = i;
            std::snprintf(s.name, sizeof(s.name), "User_Profile_%d", i);
            std::snprintf(s.email, sizeof(s.email), "user_%d@nsut.ac.in", i);
            s.marks = 75.25;
            s.isDeleted = false;

            db.addStudent(s);
        }

        auto insertEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> insertDuration = insertEnd - insertStart;
        std::cout << "Insertion Finished. Total Time: " << insertDuration.count() << " ms.\n\n";

        // 2. STRESS TEST PERFORMANCE LOOKUP (Worst-Case Scenario)
        int targetId = SCALE_LIMIT - 5; // Querying the absolute back end of the file
        Student resultStudent;

        std::cout << " Phase 2: Worst-Case Random Access Search (ID " << targetId << ")\n";
        std::cout << "-----------------------------------------------------------------\n";

        // Test A: Linear File Scan Search (O(N))
        auto startLinear = std::chrono::high_resolution_clock::now();
        bool foundLinear = linearSearchDisk(filename, targetId, resultStudent);
        auto endLinear = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationLinear = endLinear - startLinear;

        std::cout << " Traditional Linear Scan [O(N)]:    " << std::fixed << durationLinear.count() << " ms";
        if (foundLinear)
            std::cout << " (Found: " << resultStudent.name << ")";
        std::cout << "\n";

        // Test B: Indexed Engine Seek Jump (O(log N))
        auto startIndexed = std::chrono::high_resolution_clock::now();
        bool foundIndexed = db.getStudent(targetId, resultStudent);
        auto endIndexed = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationIndexed = endIndexed - startIndexed;

        std::cout << " Indexed Engine [O(log N)]: " << std::fixed << durationIndexed.count() << " ms";
        if (foundIndexed)
            std::cout << " (Found: " << resultStudent.name << ")";
        std::cout << "\n";

        // 3. PERFORMANCE COMPRESSION METRICS
        std::cout << "-----------------------------------------------------------------\n";
        if (durationIndexed.count() > 0)
        {
            double ratio = durationLinear.count() / durationIndexed.count();
            std::cout << " SCALE FACTOR: Indexed lookups are " << ratio << "x faster at 1M scale.\n";
        }
        std::cout << "=================================================================\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal Exception: " << e.what() << "\n";
    }

    return 0;
}
