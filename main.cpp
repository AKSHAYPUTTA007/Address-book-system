/*
    ADDRESS BOOK SYSTEM
    Subject  : Object-Oriented Programming (C++)
    Language : C++
    Storage  : contacts.txt (permanent)

    OOP Concepts:
      - Struct             : Contact struct stores one contact's data
      - Class              : AddressBook class manages the contact array
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
// Stores and manages an array of Contact structs
// -------------------------------------------------------
const int MAX = 100;

class AddressBook {
private:
    Contact contacts[MAX];   // array of structs
    int count;               // number of contacts stored

    // Find contact index by phone, returns -1 if not found
    int findByPhone(string phone) {
        for (int i = 0; i < count; i++)
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
    // Constructor: runs on startup, loads contacts from file
    AddressBook() {
        count = 0;
        loadFromFile();
    }

    // Read contacts from contacts.txt into the array
    void loadFromFile() {
        ifstream file("contacts.txt");
        if (!file.is_open()) return;
        string line;
        while (getline(file, line) && count < MAX) {
            if (line.empty()) continue;
            stringstream ss(line);
            getline(ss, contacts[count].name,     '|');
            getline(ss, contacts[count].phone,    '|');
            getline(ss, contacts[count].email,    '|');
            getline(ss, contacts[count].address,  '|');
            getline(ss, contacts[count].category, '|');
            count++;
        }
        file.close();
    }

    // Write all contacts from array back to contacts.txt
    void saveToFile() {
        ofstream file("contacts.txt");
        for (int i = 0; i < count; i++)
            file << contacts[i].name     << "|"
                 << contacts[i].phone    << "|"
                 << contacts[i].email    << "|"
                 << contacts[i].address  << "|"
                 << contacts[i].category << "\n";
        file.close();
        cout << "\n  Contacts saved. Goodbye!\n";
    }

    // ADD : Add a new contact to the array
    void addContact() {
        if (count >= MAX) { cout << "  Address book is full!\n"; return; }
        cout << "\n  === Add Contact ===\n";
        Contact c;
        cout << "  Name     : "; getline(cin, c.name);
        cout << "  Phone    : "; getline(cin, c.phone);
        if (findByPhone(c.phone) != -1) {
            cout << "  Phone already exists! Contact not added.\n";
            return;
        }
        cout << "  Email    : "; getline(cin, c.email);
        cout << "  Address  : "; getline(cin, c.address);
        cout << "  Category : "; getline(cin, c.category);
        contacts[count++] = c;
        cout << "  Contact added! Total: " << count << "\n";
    }

    // DISPLAY : Show all contacts
    void displayContacts() {
        if (count == 0) { cout << "\n  No contacts found.\n"; return; }
        cout << "\n  === All Contacts (" << count << ") ===\n";
        for (int i = 0; i < count; i++) {
            cout << "\n  [" << i + 1 << "]";
            printContact(i);
        }
    }

    // SEARCH : Find contacts by name (partial, case-insensitive)
    void searchContact() {
        cout << "\n  === Search Contact ===\n";
        cout << "  Enter name to search: ";
        string query; getline(cin, query);
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (toLower(contacts[i].name).find(toLower(query)) != string::npos) {
                cout << "\n  Result " << ++found << ":";
                printContact(i);
            }
        }
        if (found == 0) cout << "  No match found for: " << query << "\n";
        else cout << "  " << found << " contact(s) found.\n";
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
        char c; cin >> c; cin.ignore();
        if (c == 'y' || c == 'Y') {
            for (int i = idx; i < count - 1; i++)
                contacts[i] = contacts[i + 1];   // shift left
            count--;
            cout << "  Contact deleted! Remaining: " << count << "\n";
        } else {
            cout << "  Cancelled.\n";
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
        cout << "      ADDRESS BOOK SYSTEM\n";
        cout << "===================================\n";
        cout << "  1. Add Contact\n";
        cout << "  2. Display All Contacts\n";
        cout << "  3. Search Contact\n";
        cout << "  4. Update Contact\n";
        cout << "  5. Delete Contact\n";
        cout << "  6. Exit\n";
        cout << "===================================\n";
        cout << "  Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: book.addContact();      break;
            case 2: book.displayContacts(); break;
            case 3: book.searchContact();   break;
            case 4: book.updateContact();   break;
            case 5: book.deleteContact();   break;
            case 6: book.saveToFile();      break;
            default: cout << "  Invalid! Enter 1-6.\n";
        }
    } while (choice != 6);

    return 0;
}
