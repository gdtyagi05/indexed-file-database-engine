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
    std::cout << "5. Display All\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter Choice: ";
}

int main() {
    std::string filename = "data.dat";
    Database db(filename);

    int choice;

    while (true) {
        showMenu();

        // Safe menu input
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << " Invalid input. Enter a number.\n";
            continue;
        }

        if (choice == 0) break;

        try {
            switch (choice) {

                // ================= ADD STUDENT =================
                case 1: {
                    Student s;
                    std::string tempName, tempEmail;

                    std::cout << "Enter ID: ";
                    std::cin >> s.id;

                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    // Name
                    std::cout << "Enter Name: ";
                    std::getline(std::cin, tempName);

                    if (tempName.length() >= sizeof(s.name)) {
                        std::cout << "Warning: Name too long, will be truncated.\n";
                    }
                    std::strncpy(s.name, tempName.c_str(), sizeof(s.name) - 1);
                    s.name[sizeof(s.name) - 1] = '\0';

                    // Email
                    std::cout << "Enter Email: ";
                    std::getline(std::cin, tempEmail);

                    if (tempEmail.length() >= sizeof(s.email)) {
                        std::cout << "Warning: Email too long, will be truncated.\n";
                    }
                    std::strncpy(s.email, tempEmail.c_str(), sizeof(s.email) - 1);
                    s.email[sizeof(s.email) - 1] = '\0';

                    // Marks
                    std::cout << "Enter Marks: ";
                    if (!(std::cin >> s.marks)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid marks input.\n";
                        break;
                    }

                    s.isDeleted = false;

                    db.addStudent(s);
                    std::cout << " Record added successfully.\n";
                    break;
                }

                // ================= SEARCH =================
                case 2: {
                    int id;
                    Student s;

                    std::cout << "Enter ID to Search: ";
                    if (!(std::cin >> id)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid ID input.\n";
                        break;
                    }

                    if (db.getStudent(id, s)) {
                        std::cout << " Found:\n";
                        std::cout << "ID: " << s.id
                                  << " | Name: " << s.name
                                  << " | Email: " << s.email
                                  << " | Marks: " << s.marks << "\n";
                    } else {
                        std::cout << " ID not found.\n";
                    }
                    break;
                }

                // ================= UPDATE =================
                case 3: {
                    int id;
                    double marks;

                    std::cout << "Enter ID: ";
                    if (!(std::cin >> id)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid ID input.\n";
                        break;
                    }

                    std::cout << "Enter New Marks: ";
                    if (!(std::cin >> marks)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid marks input.\n";
                        break;
                    }

                    if (db.updateMarks(id, marks)) {
                        std::cout << " Record updated successfully.\n";
                    } else {
                        std::cout << " Update failed. ID not found.\n";
                    }
                    break;
                }

                // ================= DELETE =================
                case 4: {
                    int id;
                    char confirm;

                    std::cout << "Enter ID to Delete: ";
                    if (!(std::cin >> id)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << " Invalid ID input.\n";
                        break;
                    }

                    std::cout << "Are you sure? (y/n): ";
                    std::cin >> confirm;

                    if (confirm == 'y' || confirm == 'Y') {
                        if (db.deleteStudent(id)) {
                            std::cout << " Student soft-deleted.\n";
                        } else {
                            std::cout << " ID not found.\n";
                        }
                    } else {
                        std::cout << " Deletion cancelled.\n";
                    }
                    break;
                }

                // ================= DISPLAY =================
                case 5:
                    db.displayAll();
                    break;

                default:
                    std::cout << " Invalid option. Try again.\n";
            }
        } catch (const std::exception& e) {
            std::cout << " Error: " << e.what() << "\n";
        }
    }

    std::cout << "\nEngine shutting down safely...\n";
    return 0;
}