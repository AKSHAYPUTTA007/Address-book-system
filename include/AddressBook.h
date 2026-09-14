#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <vector>
#include <string>
#include "Contact.h"

// ============================================================
// AddressBook Class
// Manages a collection of Contact objects.
// Responsible for all operations: Add, Display, Search,
// Update, Delete, Import, Export, Statistics, File I/O.
// ============================================================
class AddressBook {
private:
    std::vector<Contact> contacts;          // In-RAM storage
    static const std::string DATA_FILE;     // Path to contacts.txt

    // ---- Private Helpers ----
    bool phoneExists(const std::string& phone) const;
    bool phoneExistsExcluding(const std::string& phone,
                              int excludeIndex) const;
    int  findIndexByPhone(const std::string& phone) const;
    void printTableHeader() const;
    void printTableRow(int serial, const Contact& c) const;
    void printTableFooter() const;
    void printDivider() const;

    // Similarity scoring for smart name search
    double similarityScore(const std::string& query,
                           const std::string& target) const;

    // Input helpers
    std::string inputField(const std::string& prompt) const;
    std::string inputCategory() const;

public:
    // Constructor
    AddressBook();

    // ---- File I/O ----
    void loadFromFile();
    void saveToFile() const;

    // ---- Core Features ----
    void addContact();
    void displayMenu();
    void searchMenu();
    void updateContact();
    void deleteContact();
    void importContacts();
    void exportBackup() const;
    void showStatistics() const;

    // ---- Display Sub-operations ----
    void displayAll() const;
    void displayByCategory() const;
    void displayByLocation() const;
    void displayByCategoryAndLocation() const;

    // ---- Search Sub-operations ----
    void searchByName() const;
    void searchByPhone() const;
    void searchByEmail() const;
    void searchByCategory() const;
    void searchByLocation() const;
};

#endif // ADDRESSBOOK_H
