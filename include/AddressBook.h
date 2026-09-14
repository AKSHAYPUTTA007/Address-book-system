#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <vector>
#include <string>
#include "Contact.h"

// ============================================================
// AddressBook Class
// Manages a collection of Contact objects in RAM.
// Handles all operations + permanent file storage.
// ============================================================
class AddressBook {
private:
    std::vector<Contact> contacts;
    std::string          defaultExtension;    // e.g. "+91"
    Contact              lastDeletedContact;  // for undo feature
    bool                 hasLastDeleted;

    static const std::string DATA_FILE;
    static const std::string CONFIG_FILE;

    // ---- Validation ----
    bool isValidEmail(const std::string& email) const;
    bool isValidPhoneDigits(const std::string& digits) const;

    // ---- Phone helpers ----
    bool phoneExists(const std::string& phone) const;
    bool phoneExistsExcluding(const std::string& phone,
                               int excludeIndex) const;
    int  findIndexByPhone(const std::string& phone) const;

    // ---- Table formatting ----
    void printTableHeader() const;
    void printTableRow(int serial, const Contact& c) const;
    void printTableFooter() const;
    void printDivider() const;

    // ---- Smart name search ----
    double similarityScore(const std::string& query,
                           const std::string& target) const;

    // ---- Input helpers ----
    std::string inputField(const std::string& prompt) const;
    std::string inputOptionalField(const std::string& prompt) const;
    std::string inputCategory() const;
    std::string inputPhoneWithExtension() const;

public:
    // Constructor
    AddressBook();

    // ---- Config & File I/O ----
    void loadConfig();
    void saveConfig() const;
    void loadFromFile();
    void saveToFile() const;

    // ---- Count (used by main menu header) ----
    int getCount() const;

    // ---- Core Menu Dispatchers ----
    void addContact();
    void displayMenu();
    void searchMenu();
    void updateContact();
    void deleteContact();
    void undoDelete();
    void sortMenu();
    void importContacts();
    void exportBackup() const;
    void showStatistics() const;
    void showSettings();

    // ---- Display Sub-operations ----
    void displayAll() const;
    void displayByCategory() const;
    void displayByLocation() const;
    void displayByCategoryAndLocation() const;
    void displayFavourites() const;
    void displayRecentlyAdded() const;
    void toggleFavourite();

    // ---- Search Sub-operations ----
    void searchByName() const;
    void searchByPhone() const;
    void searchByEmail() const;
    void searchByCategory() const;
    void searchByLocation() const;
};

#endif // ADDRESSBOOK_H
