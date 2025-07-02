#include<iostream>
#include<fstream>
using namespace std;

// Class Node 
struct ClassNode {
    string time;
    string course;
    string classroom;
    string section;
    ClassNode* next;

    ClassNode(string time, string course, string classroom, string section) {
        this->time = time;
        this->course = course;
        this->classroom = classroom;
        this->section = section;
        this->next = nullptr;
    }
};

// Day Node 
struct DayNode {
    string dayName;
    ClassNode* classHead;
    DayNode* nextDay;

    DayNode(string dayName) {
        this->dayName = dayName;
        classHead = nullptr;
        nextDay = nullptr;
    }
};

class TTADT {
    DayNode* root;

public:
    TTADT() {
        root = nullptr;
    }

    ~TTADT() {
        DayNode* dayCurr = root;
        while (dayCurr != nullptr) {
            DayNode* tempDay = dayCurr;
            ClassNode* classCurr = dayCurr->classHead;

            while (classCurr != nullptr) {
                ClassNode* tempClass = classCurr;
                classCurr = classCurr->next;
                delete tempClass;
            }

            dayCurr = dayCurr->nextDay;
            delete tempDay;
        }
    }

    DayNode* getOrCreateDay(string day) {
        if (root == nullptr) {
            root = new DayNode(day);
            return root;
        }

        DayNode* current = root;
        while (current != nullptr) {
            if (current->dayName == day) return current;
            if (current->nextDay == nullptr) break;
            current = current->nextDay;
        }

        current->nextDay = new DayNode(day);
        return current->nextDay;
    }

    void insert(string day, string time, string course, string classroom, string section) {
        DayNode* dayNode = getOrCreateDay(day);
        ClassNode* newClass = new ClassNode(time, course, classroom, section);

        if (dayNode->classHead == nullptr) {
            dayNode->classHead = newClass;
        }
        else {
            ClassNode* currClass = dayNode->classHead;
            while (currClass->next != nullptr) {
                currClass = currClass->next;
            }
            currClass->next = newClass;
        }
    }

    bool loadCSV(const char* timetable) {
        ifstream file(timetable);
        if (!file) {
            cout << "File is not open!" << endl;
            return false;
        }

        char line[200];
        file.getline(line, 200); // Skip header line

        while (file.getline(line, 200)) {
            char day[20], classroom[20], time[50], course[100], section[50];
            int i = 0, j = 0;

            // DAY
            while (line[i] != ',' && line[i] != '\0') day[j++] = line[i++];
            day[j] = '\0'; i++; j = 0;

            // CLASSROOM
            while (line[i] != ',' && line[i] != '\0') classroom[j++] = line[i++];
            classroom[j] = '\0'; i++; j = 0;

            // TIME
            while (line[i] != ',' && line[i] != '\0') time[j++] = line[i++];
            time[j] = '\0'; i++; j = 0;

            // COURSE
            while (line[i] != ',' && line[i] != '\0') course[j++] = line[i++];
            course[j] = '\0'; i++; j = 0;

            // SECTION
            while (line[i] != '\0') section[j++] = line[i++];
            section[j] = '\0';

            // Insert
            insert(day, time, course, classroom, section);
        }

        file.close();
        return true;
    }


    void display() {
        DayNode* dayCurr = root;

        while (dayCurr != nullptr) {
            cout << "Day: " << dayCurr->dayName << endl;

            ClassNode* printedClassrooms = nullptr;

            ClassNode* classCurr = dayCurr->classHead;
            while (classCurr != nullptr) {
                bool alreadyPrinted = false;
                ClassNode* temp = printedClassrooms;
                while (temp != nullptr) {
                    if (temp->classroom == classCurr->classroom) {
                        alreadyPrinted = true;
                        break;
                    }
                    temp = temp->next;
                }

                if (!alreadyPrinted) {
                    cout << "  Classroom: " << classCurr->classroom << endl;

                    ClassNode* inner = dayCurr->classHead;
                    while (inner != nullptr) {
                        if (inner->classroom == classCurr->classroom) {
                            cout << "    Time: " << inner->time
                                << ", Course: " << inner->course
                                << ", Section: " << inner->section << endl;
                        }
                        inner = inner->next;
                    }

                    ClassNode* newPrinted = new ClassNode("", "", classCurr->classroom, "");
                    newPrinted->next = printedClassrooms;
                    printedClassrooms = newPrinted;
                }

                classCurr = classCurr->next;
            }

            cout << endl;
            dayCurr = dayCurr->nextDay;
            cout << "===========================================================================================" << endl;
        }
    }

    void printCourseInfo(char coursename[]) {
        DayNode* dayCurr = root;
        bool found = false;

        while (dayCurr != nullptr) {
            ClassNode* classCurr = dayCurr->classHead;
            while (classCurr != nullptr) {
                if (classCurr->course == coursename) {
                    if (!found) {
                        cout << "Course: " << coursename << endl;
                        found = true;
                    }
                    cout << "  Day: " << dayCurr->dayName
                        << ", Time: " << classCurr->time
                        << ", Classroom: " << classCurr->classroom
                        << ", Section: " << classCurr->section << endl;
                }
                classCurr = classCurr->next;
            }
            dayCurr = dayCurr->nextDay;
            cout << "=============================================================================================" << endl;
        }

        if (!found) {
            cout << "Course '" << coursename << "' not found in the timetable." << endl;
        }
    }

    void DetermineFreeClassrooms(char day[], char timeslot[]) {
        DayNode* dayCurr = root;
        bool dayFound = false;

        // Find the correct day
        while (dayCurr != nullptr) {
            if (dayCurr->dayName == day) {
                dayFound = true;
                break;
            }
            dayCurr = dayCurr->nextDay;
        }

        if (!dayFound) {
            cout << "Day '" << day << "' not found in timetable." << endl;
            return;
        }

        ClassNode* classCurr = dayCurr->classHead;
        bool anyFree = false;
        ClassNode* printedClassrooms = nullptr;

        while (classCurr != nullptr) {
            // Check if classroom already checked
            bool alreadyChecked = false;
            ClassNode* temp = printedClassrooms;
            while (temp != nullptr) {
                if (temp->classroom == classCurr->classroom) {
                    alreadyChecked = true;
                    break;
                }
                temp = temp->next;
            }

            if (!alreadyChecked) {
                // Check for this classroom at the given timeslot
                ClassNode* check = dayCurr->classHead;
                bool foundSlot = false;
                while (check != nullptr) {
                    if (check->classroom == classCurr->classroom && check->time == timeslot) {
                        foundSlot = true;
                        // If the course is empty, classroom is free
                        if (check->course == "") {
                            cout << "Free Classroom: " << classCurr->classroom << endl;
                            anyFree = true;
                        }
                        break; // Found the relevant timeslot
                    }
                    check = check->next;
                }

                // Mark this classroom as checked
                ClassNode* newPrinted = new ClassNode("", "", classCurr->classroom, "");
                newPrinted->next = printedClassrooms;
                printedClassrooms = newPrinted;
            }

            classCurr = classCurr->next;
        }

        if (!anyFree) {
            cout << "No free classrooms at " << timeslot << " on " << day << "." << endl;
        }

        // Cleanup memory
        while (printedClassrooms != nullptr) {
            ClassNode* temp = printedClassrooms;
            printedClassrooms = printedClassrooms->next;
            delete temp;
        }
    }

    void PrintTimetableOfSection(char section[]) {
        DayNode* dayCurr = root;
        bool found = false;

        cout << "Timetable for Section: " << section << endl;
        cout << "------------------------------------------------------------------" << endl;
        cout << "| Day        | Time          | Classroom | Course                 |" << endl;
        cout << "------------------------------------------------------------------" << endl;

        while (dayCurr != nullptr) {
            ClassNode* classCurr = dayCurr->classHead;
            while (classCurr != nullptr) {
                if (classCurr->section == section) {
                    found = true;
                    cout << "| " << dayCurr->dayName
                        << " | " << classCurr->time
                        << " | " << classCurr->classroom
                        << " | " << classCurr->course
                        << " |" << endl;
                }
                classCurr = classCurr->next;
            }
            dayCurr = dayCurr->nextDay;
        }

        cout << "---------------------------------------------------------" << endl;

        if (!found) {
            cout << "No timetable entries found for section " << section << "." << endl;
        }
    }



};

// Main function
int main() {
    TTADT timetable;
    int choice;

    do {
        cout << "\n======= Timetable Menu =======" << endl;
        cout << "1. Load Timetable from File" << endl;
        cout << "2. Display Timetable" << endl;
        cout << "3. Course Information" << endl;
        cout << "4. Finding Free slot" << endl;
        cout << "5. Print Timetable of Section" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (timetable.loadCSV("timetable.csv")) {
                cout << "Timetable Loaded Successfully!" << endl;
            }
            else {
                cout << "Timetable Load Failed!" << endl;
            }
            break;

        case 2:
            timetable.display();
            break;

        case 3: {
            char courseName[100];
            cout << "Enter Course Name: ";
            cin.ignore();
            cin.getline(courseName, 100);
            timetable.printCourseInfo(courseName);
            break;
        }
        case 4: {
            char day[20], timeslot[50];
            cout << "Enter Day: ";
            cin.ignore();
            cin.getline(day, 20);
            cout << "Enter Timeslot: ";
            cin.getline(timeslot, 50);
            timetable.DetermineFreeClassrooms(day, timeslot);
            break;
        }
        case 5: {
            char section[100];
            cout << "Enter Section: ";
            cin.ignore();
            cin.getline(section, 100);
            timetable.PrintTimetableOfSection(section);
            break;
        }

        case 6:
            cout << "Exiting Program. Goodbye!" << endl;
            break;

        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 6);

    return 0;
}
