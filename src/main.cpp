/*
 * =====================================================
 *         ADDRESS BOOK SYSTEM
 * =====================================================
 * Subject : Object-Oriented Programming (C++)
 * Type    : Console-Based, Single File
 *
 * OOP Concepts Used:
 *   1. Struct            - Contact struct holds one contact's data
 *   2. Class             - AddressBook class manages all contacts
 *   3. Constructor       - loads contacts from file on startup
 *   4. Member Functions  - add, display, search, update, delete
 *
 * Teacher Requirements:
 *   o Storing contacts using structures  (struct Contact)
 *   o Add, search, delete operations     (member functions)
 *   o String handling                    (string fields, file parsing)
 *   o Menu-driven system                 (do-while loop + switch)
 * =====================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// =====================================================
// STRUCT : Contact
// A structure that holds the information of ONE contact.
// All fields are strings.
// =====================================================
struct Contact {
    string name;
    string phone;
    string email;
    string address;
    string category;
};

// =====================================================
// CLASS : AddressBook
// Manages a fixed-size array of Contact structs.
// Stores up to 100 contacts.
// =====================================================
const int MAX = 100;   // maximum number of contacts

class AddressBook {
private:
    Contact contacts[MAX];  // array of Contact structs
    int count;              // how many contacts are currently stored

    // ---- Private Helper Functions ----

    // Converts a string to lowercase (used in search)
    string toLower(string s) {
        for (int i = 0; i < (int)s.length(); i++)
            s[i] = tolower(s[i]);
        return s;
    }

    // Finds a contact by phone number. Returns index or -1 if not found
    int findByPhone(string phone) {
        for (int i = 0; i < count; i++)
            if (contacts[i].phone == phone)
                return i;
        return -1;
    }

    // Prints the details of one contact
    void printContact(int i) {
        string line(38, '-');
        cout << "  " << line << "\n";
        cout << "  Name     : " << contacts[i].name     << "\n";
        cout << "  Phone    : " << contacts[i].phone    << "\n";
        cout << "  Email    : " << contacts[i].email    << "\n";
        cout << "  Address  : " << contacts[i].address  << "\n";
        cout << "  Category : " << contacts[i].category << "\n";
        cout << "  " << line << "\n";
    }

public:
    // =====================================================
    // CONSTRUCTOR
    // Automatically runs when the program starts.
    // Sets count to 0 and loads contacts from the file.
    // =====================================================
    AddressBook() {
        count = 0;
        loadFromFile();
    }

    // =====================================================
    // LOAD FROM FILE
    // Reads contacts.txt and fills the array.
    // File format: name|phone|email|address|category
    // =====================================================
    void loadFromFile() {
        ifstream file("data/contacts.txt");
        if (!file.is_open()) return;   // file may not exist on first run

        string line;
        while (getline(file, line) && count < MAX) {
            if (line.empty()) continue;    // skip blank lines

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

    // =====================================================
    // SAVE TO FILE
    // Writes all contacts from the array back to contacts.txt.
    // Called when the user exits the program.
    // =====================================================
    void saveToFile() {
        ofstream file("data/contacts.txt");
        for (int i = 0; i < count; i++) {
            file << contacts[i].name     << "|"
                 << contacts[i].phone    << "|"
                 << contacts[i].email    << "|"
                 << contacts[i].address  << "|"
                 << contacts[i].category << "\n";
        }
        file.close();
    }

    // =====================================================
    // FEATURE 1 : ADD CONTACT
    // Reads a new contact from the user and stores it
    // in the array. Rejects duplicate phone numbers.
    // =====================================================
    void addContact() {
        if (count >= MAX) {
            cout << "\n  [!] Address book is full (max " << MAX << " contacts).\n";
            return;
        }

        cout << "\n  ===== Add New Contact =====\n";
        Contact c;

        cout << "  Name     : "; getline(cin, c.name);
        cout << "  Phone    : "; getline(cin, c.phone);

        // Check for duplicate phone number
        if (findByPhone(c.phone) != -1) {
            cout << "\n  [!] A contact with this phone number already exists!\n";
            return;
        }

        cout << "  Email    : "; getline(cin, c.email);
        cout << "  Address  : "; getline(cin, c.address);
        cout << "  Category : "; getline(cin, c.category);

        contacts[count] = c;    // store the struct in the array
        count++;

        cout << "\n  [+] Contact added successfully! Total: " << count << "\n";
    }

    // =====================================================
    // FEATURE 2 : DISPLAY ALL CONTACTS
    // Shows every contact stored in the array.
    // =====================================================
    void displayContacts() {
        if (count == 0) {
            cout << "\n  No contacts found.\n";
            return;
        }

        cout << "\n  ===== All Contacts (" << count << ") =====\n";
        for (int i = 0; i < count; i++) {
            cout << "\n  Contact " << i + 1 << ":\n";
            printContact(i);
        }
    }

    // =====================================================
    // FEATURE 3 : SEARCH CONTACT
    // Searches by name. Partial and case-insensitive.
    // Example: typing "ak" will find "Akshay" and "Akarsh"
    // =====================================================
    void searchContact() {
        cout << "\n  ===== Search Contact =====\n";
        cout << "  Enter name to search: ";
        string query;
        getline(cin, query);

        int found = 0;
        for (int i = 0; i < count; i++) {
            // toLower makes search case-insensitive
            if (toLower(contacts[i].name).find(toLower(query)) != string::npos) {
                cout << "\n  Result " << found + 1 << ":\n";
                printContact(i);
                found++;
            }
        }

        if (found == 0)
            cout << "\n  No contact found with name: \"" << query << "\"\n";
        else
            cout << "\n  Found " << found << " contact(s).\n";
    }

    // =====================================================
    // FEATURE 4 : UPDATE CONTACT
    // Finds a contact by phone number and updates its fields.
    // Press Enter to keep the current value.
    // =====================================================
    void updateContact() {
        cout << "\n  ===== Update Contact =====\n";
        cout << "  Enter phone number to update: ";
        string phone;
        getline(cin, phone);

        int idx = findByPhone(phone);
        if (idx == -1) {
            cout << "\n  [!] No contact found with phone: " << phone << "\n";
            return;
        }

        cout << "\n  Current Details:\n";
        printContact(idx);
        cout << "  Enter new values (press Enter to keep current):\n\n";

        string input;

        cout << "  Name     [" << contacts[idx].name << "]: ";
        getline(cin, input);
        if (!input.empty()) contacts[idx].name = input;

        cout << "  Phone    [" << contacts[idx].phone << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            if (input != contacts[idx].phone && findByPhone(input) != -1)
                cout << "  [!] Phone already taken. Keeping old number.\n";
            else
                contacts[idx].phone = input;
        }

        cout << "  Email    [" << contacts[idx].email << "]: ";
        getline(cin, input);
        if (!input.empty()) contacts[idx].email = input;

        cout << "  Address  [" << contacts[idx].address << "]: ";
        getline(cin, input);
        if (!input.empty()) contacts[idx].address = input;

        cout << "  Category [" << contacts[idx].category << "]: ";
        getline(cin, input);
        if (!input.empty()) contacts[idx].category = input;

        cout << "\n  [+] Contact updated successfully!\n";
    }

    // =====================================================
    // FEATURE 5 : DELETE CONTACT
    // Finds contact by phone, asks confirmation, then
    // removes it by shifting array elements left.
    // =====================================================
    // Returns the number of contacts stored (used in main)
    int getCount() {
        return count;
    }

    void deleteContact() {
        cout << "\n  ===== Delete Contact =====\n";
        cout << "  Enter phone number to delete: ";
        string phone;
        getline(cin, phone);

        int idx = findByPhone(phone);
        if (idx == -1) {
            cout << "\n  [!] No contact found with phone: " << phone << "\n";
            return;
        }

        cout << "\n  Contact found:\n";
        printContact(idx);
        cout << "  Are you sure you want to delete? (y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore();

        if (confirm == 'y' || confirm == 'Y') {
            // Shift all elements after idx one position to the left
            for (int i = idx; i < count - 1; i++)
                contacts[i] = contacts[i + 1];
            count--;
            cout << "\n  [-] Contact deleted. Remaining: " << count << "\n";
        } else {
            cout << "\n  Deletion cancelled.\n";
        }
    }
};

// =====================================================
// MAIN FUNCTION
// Entry point. Creates the AddressBook object,
// shows the menu, and calls the right function.
// Menu-driven using do-while loop and switch.
// =====================================================
int main() {
    AddressBook book;   // object created, constructor loads file

    cout << "\n  [*] Address Book ready. " << book.getCount() << " contact(s) loaded.\n";

    int choice;

    do {
        cout << "\n=====================================\n";
        cout << "       ADDRESS BOOK SYSTEM\n";
        cout << "=====================================\n";
        cout << "  1. Add Contact\n";
        cout << "  2. Display All Contacts\n";
        cout << "  3. Search Contact\n";
        cout << "  4. Update Contact\n";
        cout << "  5. Delete Contact\n";
        cout << "  6. Exit\n";
        cout << "=====================================\n";
        cout << "  Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: book.addContact();      break;
            case 2: book.displayContacts(); break;
            case 3: book.searchContact();   break;
            case 4: book.updateContact();   break;
            case 5: book.deleteContact();   break;
            case 6:
                book.saveToFile();
                cout << "\n  [*] Contacts saved to file. Goodbye!\n\n";
                break;
            default:
                cout << "\n  [!] Invalid choice. Enter 1 to 6.\n";
        }

    } while (choice != 6);

    return 0;
}
