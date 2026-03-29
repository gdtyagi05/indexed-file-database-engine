#include <iostream>
#include <string>
#include <limits>
#include <cstring>
#include "database.h"
#include "record.h"

void showMenu() {
    std::cout << "\n===== Student Record System =====\n";
    std::cout << "1. Add Student\n";
    std::cout << "2. Search Student (Indexed)\n";
    std::cout << "3. Update Marks\n";
    std::cout << "4. Delete Student (Soft Delete)\n";
    std::cout << "5. Display All Active\n";
    std::cout << "6. Compact Database (Optimize Space)\n"; // NEW
    std::cout << "0. Exit\n";
    std::cout << "Enter Choice: ";
}

int main() {
    std::string filename = "data.dat";
    
    try {
        Database db(filename);
        int choice;

        while (true) {
            showMenu();

            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << " Invalid input. Enter a number.\n";
                continue;
            }

            if (choice == 0) break;

            switch (choice) {
                case 1: {
                    Student s;
                    std::string tempName, tempEmail;

                    std::cout << "Enter ID: ";
                    std::cin >> s.id;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::cout << "Enter Name: ";
                    std::getline(std::cin, tempName);
                    std::strncpy(s.name, tempName.c_str(), sizeof(s.name) - 1);
                    s.name[sizeof(s.name) - 1] = '\0';

                    std::cout << "Enter Email: ";
                    std::getline(std::cin, tempEmail);
                    std::strncpy(s.email, tempEmail.c_str(), sizeof(s.email) - 1);
                    s.email[sizeof(s.email) - 1] = '\0';

                    std::cout << "Enter Marks: ";
                    if (!(std::cin >> s.marks)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid marks input.\n";
                        break;
                    }

                    s.isDeleted = false;
                    if(db.addStudent(s)) std::cout << " Record added successfully.\n";
                    break;
                }

                case 2: {
                    int id;
                    Student s;
                    std::cout << "Enter ID to Search: ";
                    std::cin >> id;
                    if (db.getStudent(id, s)) {
                        std::cout << " Found: " << s.name << " | Marks: " << s.marks << "\n";
                    } else {
                        std::cout << " ID not found.\n";
                    }
                    break;
                }

                case 3: {
                    int id;
                    double marks;
                    std::cout << "Enter ID: "; std::cin >> id;
                    std::cout << "Enter New Marks: "; std::cin >> marks;
                    if (db.updateMarks(id, marks)) std::cout << " Updated successfully.\n";
                    else std::cout << " ID not found.\n";
                    break;
                }

                case 4: {
                    int id;
                    char confirm;
                    std::cout << "Enter ID to Delete: "; std::cin >> id;
                    std::cout << "Are you sure? (y/n): "; std::cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        if (db.deleteStudent(id)) std::cout << " Student soft-deleted.\n";
                        else std::cout << " ID not found.\n";
                    }
                    break;
                }

                case 5:
                    db.displayAll();
                    break;

                case 6: // NEW: Compaction logic
                    std::cout << " Running maintenance: Removing deleted records...\n";
                    db.compact(); 
                    break;

                default:
                    std::cout << " Invalid option.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << " CRITICAL ERROR: " << e.what() << "\n";
    }

    std::cout << "\nEngine shutting down safely...\n";
    return 0;
}
