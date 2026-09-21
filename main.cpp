/*
    ADDRESS BOOK SYSTEM
    Subject  : Object-Oriented Programming (C++)
    Language : C++
    Storage  : contacts.txt (permanent)

    OOP Concepts:
      - Struct             : Contact struct stores one contact's data
      - Class              : AddressBook class manages the contact list
      - Constructor        : loads contacts from file when program starts
      - Member Functions   : add, display, search, update, delete, save

    Teacher Requirements:
      - Storing contacts using structures  -> struct Contact
      - Add, Search, Delete operations    -> member functions
      - String handling                   -> string fields, file parsing
      - Menu-driven system                -> do-while + switch
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// -------------------------------------------------------
// STRUCT : Contact
// Holds the information of one contact
// -------------------------------------------------------
struct Contact {
    string name;
    string phone;
    string email;
    string address;
    string category;
};

// -------------------------------------------------------
// CLASS : AddressBook
// Stores and manages a list of Contact structs using vector.
// Vector automatically grows — no fixed size limit.
// -------------------------------------------------------
class AddressBook {
private:
    vector<Contact> contacts;   // list of Contact structs (no size limit)

    // Find contact index by phone, returns -1 if not found
    int findByPhone(string phone) {
        for (int i = 0; i < (int)contacts.size(); i++)
            if (contacts[i].phone == phone)
                return i;
        return -1;
    }

    // Convert string to lowercase (for case-insensitive search)
    string toLower(string s) {
        for (int i = 0; i < (int)s.length(); i++)
            s[i] = tolower(s[i]);
        return s;
    }

    // Print one contact's details
    void printContact(int i) {
        cout << "  ---------------------------------\n";
        cout << "  Name     : " << contacts[i].name     << "\n";
        cout << "  Phone    : " << contacts[i].phone    << "\n";
        cout << "  Email    : " << contacts[i].email    << "\n";
        cout << "  Address  : " << contacts[i].address  << "\n";
        cout << "  Category : " << contacts[i].category << "\n";
        cout << "  ---------------------------------\n";
    }

public:
    // Returns total number of contacts (used in menu header)
    int getCount() { return (int)contacts.size(); }

    // Constructor: runs on startup, loads contacts from file
    AddressBook() {
        loadFromFile();
    }

    // Read contacts from contacts.txt into the vector
    void loadFromFile() {
        ifstream file("contacts.txt");
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            Contact c;
            stringstream ss(line);
            getline(ss, c.name,     '|');
            getline(ss, c.phone,    '|');
            getline(ss, c.email,    '|');
            getline(ss, c.address,  '|');
            getline(ss, c.category, '|');
            contacts.push_back(c);   // add to vector
        }
        file.close();
    }

    // Write all contacts from vector back to contacts.txt
    void saveToFile() {
        ofstream file("contacts.txt");
        for (int i = 0; i < (int)contacts.size(); i++)
            file << contacts[i].name     << "|"
                 << contacts[i].phone    << "|"
                 << contacts[i].email    << "|"
                 << contacts[i].address  << "|"
                 << contacts[i].category << "\n";
        file.close();
        cout << "\n  Contacts saved. Goodbye!\n";
    }

    // ADD : Add a new contact to the vector
    void addContact() {
        cout << "\n  === Add Contact ===\n";
        Contact c;
        cout << "  Name     : "; getline(cin, c.name);
        cout << "  Phone    : "; getline(cin, c.phone);
        if (c.name.empty() || c.phone.empty()) {
            cout << "  Name and Phone cannot be empty!\n";
            return;
        }
        if (findByPhone(c.phone) != -1) {
            cout << "  Phone already exists! Contact not added.\n";
            return;
        }
        cout << "  Email    : "; getline(cin, c.email);
        cout << "  Address  : "; getline(cin, c.address);
        cout << "  Category : "; getline(cin, c.category);
        contacts.push_back(c);   // vector grows automatically
        cout << "  Contact added! Total: " << contacts.size() << "\n";
    }

    // DISPLAY : Show all contacts sorted A-Z by name
    void displayContacts() {
        if (contacts.empty()) { cout << "\n  No contacts found.\n"; return; }
        // Sort contacts alphabetically by name (A to Z)
        sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
            string na = a.name, nb = b.name;
            for (char& c : na) c = tolower(c);
            for (char& c : nb) c = tolower(c);
            return na < nb;
        });
        cout << "\n  === All Contacts (" << contacts.size() << ") — Sorted A-Z ===\n";
        for (int i = 0; i < (int)contacts.size(); i++) {
            cout << "\n  [" << i + 1 << "]";
            printContact(i);
        }
    }

    // SEARCH : Search by name (partial) or phone (exact)
    void searchContact() {
        cout << "\n  === Search Contact ===\n";
        cout << "  1. Search by Name\n";
        cout << "  2. Search by Phone\n";
        cout << "  Enter choice: ";
        int opt;
        if (!(cin >> opt)) { cin.clear(); cin.ignore(1000, '\n'); return; }
        cin.ignore(1000, '\n');

        if (opt == 1) {
            // Search by name — partial, case-insensitive
            cout << "  Enter name to search: ";
            string query; getline(cin, query);
            if (query.empty()) { cout << "  Search query cannot be empty!\n"; return; }
            int found = 0;
            for (int i = 0; i < (int)contacts.size(); i++) {
                if (toLower(contacts[i].name).find(toLower(query)) != string::npos) {
                    cout << "\n  Result " << ++found << ":";
                    printContact(i);
                }
            }
            if (found == 0) cout << "  No match found for: " << query << "\n";
            else cout << "  " << found << " contact(s) found.\n";

        } else if (opt == 2) {
            // Search by phone — exact match
            cout << "  Enter phone to search: ";
            string phone; getline(cin, phone);
            int idx = findByPhone(phone);
            if (idx == -1) cout << "  No contact found with phone: " << phone << "\n";
            else printContact(idx);

        } else {
            cout << "  Invalid option.\n";
        }
    }

    // UPDATE : Edit a contact by phone number
    void updateContact() {
        cout << "\n  === Update Contact ===\n";
        cout << "  Enter phone to update: ";
        string phone; getline(cin, phone);
        int idx = findByPhone(phone);
        if (idx == -1) { cout << "  Contact not found!\n"; return; }
        cout << "\n  Current details:"; printContact(idx);
        cout << "  Enter new values (press Enter to keep current):\n\n";
        string val;
        cout << "  Name     [" << contacts[idx].name     << "]: "; getline(cin, val); if (!val.empty()) contacts[idx].name = val;
        cout << "  Phone    [" << contacts[idx].phone    << "]: "; getline(cin, val);
        if (!val.empty()) {
            if (val != phone && findByPhone(val) != -1) cout << "  Phone taken. Keeping old.\n";
            else contacts[idx].phone = val;
        }
        cout << "  Email    [" << contacts[idx].email    << "]: "; getline(cin, val); if (!val.empty()) contacts[idx].email = val;
        cout << "  Address  [" << contacts[idx].address  << "]: "; getline(cin, val); if (!val.empty()) contacts[idx].address = val;
        cout << "  Category [" << contacts[idx].category << "]: "; getline(cin, val); if (!val.empty()) contacts[idx].category = val;
        cout << "  Contact updated!\n";
    }

    // DELETE : Remove a contact by phone number
    void deleteContact() {
        cout << "\n  === Delete Contact ===\n";
        cout << "  Enter phone to delete: ";
        string phone; getline(cin, phone);
        int idx = findByPhone(phone);
        if (idx == -1) { cout << "  Contact not found!\n"; return; }
        printContact(idx);
        cout << "  Confirm delete? (y/n): ";
        char c; cin >> c; cin.ignore(1000, '\n');
        if (c == 'y' || c == 'Y') {
            contacts.erase(contacts.begin() + idx);   // vector removes + shifts automatically
            cout << "  Contact deleted! Remaining: " << contacts.size() << "\n";
        } else {
            cout << "  Cancelled.\n";
        }
    }

    // STATISTICS : Count contacts per category
    void showStats() {
        if (contacts.empty()) { cout << "\n  No contacts.\n"; return; }
        cout << "\n  === Statistics ===\n";
        cout << "  Total Contacts : " << contacts.size() << "\n\n";
        // Count each category by looping through all contacts
        vector<string> cats = {"Family", "Friends", "College", "Work", "Other"};
        for (const string& cat : cats) {
            int count = 0;
            for (int i = 0; i < (int)contacts.size(); i++)
                if (contacts[i].category == cat) count++;
            if (count > 0)
                cout << "  " << cat << "\t: " << count << "\n";
        }
    }
};

// -------------------------------------------------------
// MAIN : Menu-driven system
// -------------------------------------------------------
int main() {
    AddressBook book;

    int choice;
    do {
        cout << "\n===================================\n";
        cout << "  ADDRESS BOOK  [" << book.getCount() << " contacts]\n";
        cout << "===================================\n";
        cout << "  1. Add Contact\n";
        cout << "  2. Display All Contacts\n";
        cout << "  3. Search Contact\n";
        cout << "  4. Update Contact\n";
        cout << "  5. Delete Contact\n";
        cout << "  6. Statistics\n";
        cout << "  7. Exit\n";
        cout << "===================================\n";
        cout << "  Enter choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        } else {
            cin.ignore(1000, '\n');
        }

        switch (choice) {
            case 1: book.addContact();      break;
            case 2: book.displayContacts(); break;
            case 3: book.searchContact();   break;
            case 4: book.updateContact();   break;
            case 5: book.deleteContact();   break;
            case 6: book.showStats();       break;
            case 7: book.saveToFile();      break;
            default: cout << "  Invalid! Enter 1-7.\n";
        }
    } while (choice != 7);

    return 0;
}
