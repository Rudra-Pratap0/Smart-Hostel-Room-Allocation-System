#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

// Structure to store student details
struct Student {
    string name;
    float cgpa;
    float distance; // in km
    int year;       // 1, 2, 3, or 4
    bool medicalCondition; // true for Yes, false for No
};

// Structure to represent a Hostel Room
struct Room {
    int roomNumber;
    int capacity;
    vector<Student> occupants;
};

// Function prototypes
void displayMenu();
void addStudent(vector<Student>& students);
void displayStudents(const vector<Student>& students);
void mergeSort(vector<Student>& arr, int left, int right);
void merge(vector<Student>& arr, int left, int mid, int right);
bool comparePriority(const Student& a, const Student& b);
void allocateRoomsGreedy(const vector<Student>& students);
void saveToFile(const vector<Student>& students, const string& filename);
void loadFromFile(vector<Student>& students, const string& filename);

int main() {
    vector<Student> students;
    int choice;
    const string filename = "students_data.txt";
    
    // Attempt to load existing data
    loadFromFile(students, filename);

    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addStudent(students);
                break;
            case 2:
                if (students.empty()) {
                    cout << "No students registered yet.\n";
                } else {
                    displayStudents(students);
                }
                break;
            case 3:
                if (students.empty()) {
                    cout << "No students to sort.\n";
                } else {
                    // Sorting using Merge Sort (O(N log N))
                    mergeSort(students, 0, students.size() - 1);
                    cout << "\nStudents have been ranked based on priority successfully!\n";
                    displayStudents(students);
                }
                break;
            case 4:
                if (students.empty()) {
                    cout << "No students available for allocation. Please add students first.\n";
                } else {
                    // Make sure they are sorted before allocation
                    vector<Student> sortedStudents = students;
                    mergeSort(sortedStudents, 0, sortedStudents.size() - 1);
                    allocateRoomsGreedy(sortedStudents);
                }
                break;
            case 5:
                saveToFile(students, filename);
                break;
            case 6:
                cout << "Saving data and exiting. Goodbye!\n";
                saveToFile(students, filename);
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}

void displayMenu() {
    cout << "\n============================================\n";
    cout << "   Smart Hostel Room Allocation System\n";
    cout << "============================================\n";
    cout << "1. Add Student Details\n";
    cout << "2. Display Student List\n";
    cout << "3. Rank Students (Merge Sort)\n";
    cout << "4. Allocate Rooms (Greedy Algorithm)\n";
    cout << "5. Save Data to File\n";
    cout << "6. Exit\n";
    cout << "============================================\n";
}

void addStudent(vector<Student>& students) {
    Student s;
    cout << "\n--- Enter Student Details ---\n";
    cout << "Name: ";
    cin.ignore();
    getline(cin, s.name);
    
    cout << "CGPA (0.0 to 10.0): ";
    cin >> s.cgpa;
    
    cout << "Distance from home (in km): ";
    cin >> s.distance;
    
    cout << "Academic Year (1, 2, 3, 4): ";
    cin >> s.year;
    
    char med;
    cout << "Medical Condition (Y/N): ";
    cin >> med;
    s.medicalCondition = (med == 'Y' || med == 'y');
    
    students.push_back(s);
    cout << "Student added successfully!\n";
}

void displayStudents(const vector<Student>& students) {
    cout << "\n------------------------------------------------------------------------------------------\n";
    cout << left << setw(20) << "Name" 
         << setw(10) << "CGPA" 
         << setw(15) << "Distance(km)" 
         << setw(10) << "Year" 
         << setw(15) << "Medical Cond." << "\n";
    cout << "------------------------------------------------------------------------------------------\n";
    for (const auto& s : students) {
        cout << left << setw(20) << s.name 
             << setw(10) << s.cgpa 
             << setw(15) << s.distance 
             << setw(10) << s.year 
             << setw(15) << (s.medicalCondition ? "Yes" : "No") << "\n";
    }
    cout << "------------------------------------------------------------------------------------------\n";
}

// Custom comparator defining the priority logic
bool comparePriority(const Student& a, const Student& b) {
    // Priority 1: Higher CGPA first
    // Note: Small floating point differences can be tricky, but we'll use standard > operator
    if (a.cgpa != b.cgpa) {
        return a.cgpa > b.cgpa;
    }
    // Priority 2: Medical condition first
    if (a.medicalCondition != b.medicalCondition) {
        return a.medicalCondition > b.medicalCondition; // true > false
    }
    // Priority 3: Farther distance first
    if (a.distance != b.distance) {
        return a.distance > b.distance;
    }
    // Priority 4: Senior students first
    return a.year > b.year;
}

// Merge Sort implementation
void merge(vector<Student>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<Student> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        // Use our custom priority comparator
        if (comparePriority(L[i], R[j])) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<Student>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Greedy Room Allocation
void allocateRoomsGreedy(const vector<Student>& students) {
    int numRooms, capacityPerRoom;
    cout << "\n--- Room Allocation Settings ---\n";
    cout << "Enter total number of available rooms: ";
    cin >> numRooms;
    cout << "Enter capacity per room: ";
    cin >> capacityPerRoom;

    vector<Room> rooms(numRooms);
    for (int i = 0; i < numRooms; ++i) {
        rooms[i].roomNumber = i + 1;
        rooms[i].capacity = capacityPerRoom;
    }

    int studentIdx = 0;
    int totalStudents = students.size();

    // The greedy algorithm iterates through available room slots and assigns 
    // the currently highest-priority unassigned student to the available slot.
    for (int i = 0; i < numRooms; ++i) {
        while (rooms[i].occupants.size() < rooms[i].capacity && studentIdx < totalStudents) {
            rooms[i].occupants.push_back(students[studentIdx]);
            studentIdx++;
        }
        if (studentIdx >= totalStudents) break; // All students allocated
    }

    // Display Allocation Results
    cout << "\n============================================\n";
    cout << "           Room Allocation Results\n";
    cout << "============================================\n";
    for (int i = 0; i < numRooms; ++i) {
        cout << "Room " << rooms[i].roomNumber << " (Capacity: " 
             << rooms[i].occupants.size() << "/" << rooms[i].capacity << ")\n";
        if (rooms[i].occupants.empty()) {
            cout << "  [Empty]\n";
        } else {
            for (const auto& s : rooms[i].occupants) {
                cout << "  -> " << s.name << " (CGPA: " << s.cgpa << ")\n";
            }
        }
        cout << "--------------------------------------------\n";
    }

    // Display Waitlist (Unallocated Students)
    if (studentIdx < totalStudents) {
        cout << "\n[!] WAITLIST - The following students could not be allocated rooms:\n";
        for (int i = studentIdx; i < totalStudents; ++i) {
            cout << "  -> " << students[i].name << " (CGPA: " << students[i].cgpa << ")\n";
        }
    } else {
        cout << "\n[+] All students were successfully allocated rooms!\n";
    }
}

void saveToFile(const vector<Student>& students, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error opening file for writing.\n";
        return;
    }
    for (const auto& s : students) {
        outFile << s.name << "," 
                << s.cgpa << "," 
                << s.distance << "," 
                << s.year << "," 
                << s.medicalCondition << "\n";
    }
    outFile.close();
    cout << "Data saved to " << filename << " successfully.\n";
}

void loadFromFile(vector<Student>& students, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        // File might not exist yet, which is fine
        return;
    }
    
    students.clear();
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        
        Student s;
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        
        if (pos1 != string::npos && pos2 != string::npos && 
            pos3 != string::npos && pos4 != string::npos) {
            s.name = line.substr(0, pos1);
            s.cgpa = stof(line.substr(pos1 + 1, pos2 - pos1 - 1));
            s.distance = stof(line.substr(pos2 + 1, pos3 - pos2 - 1));
            s.year = stoi(line.substr(pos3 + 1, pos4 - pos3 - 1));
            s.medicalCondition = stoi(line.substr(pos4 + 1));
            students.push_back(s);
        }
    }
    inFile.close();
    cout << "Loaded " << students.size() << " student records from file.\n";
}
