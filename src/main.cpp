#include <iostream>
#include <limits>
#include "../include/AddressBook.h"

// ============================================================
// Address Book System — Entry Point
//
// Startup  : Loads all contacts from data/contacts.txt into RAM
// Runtime  : All operations work on the in-RAM vector<Contact>
// Shutdown : Saves all contacts back to data/contacts.txt
// ============================================================

void printMainMenu() {
    std::cout << "\n";
    std::cout << "=====================================\n";
    std::cout << "       ADDRESS BOOK SYSTEM\n";
    std::cout << "=====================================\n";
    std::cout << "  1. Add Contact\n";
    std::cout << "  2. Display Contacts\n";
    std::cout << "  3. Search Contacts\n";
    std::cout << "  4. Update Contact\n";
    std::cout << "  5. Delete Contact\n";
    std::cout << "  6. Import Contacts\n";
    std::cout << "  7. Export Backup\n";
    std::cout << "  8. Statistics\n";
    std::cout << "  9. Exit\n";
    std::cout << "=====================================\n";
    std::cout << "  Enter Choice: ";
}

int main() {
    AddressBook book;

    // --- STARTUP: Load contacts from file into RAM ---
    std::cout << "\n  [*] Loading contacts from file...\n";
    book.loadFromFile();
    std::cout << "  [*] Contacts loaded. Welcome!\n";

    int choice = 0;

    while (true) {
        printMainMenu();

        if (!(std::cin >> choice)) {
            // Handle non-integer input gracefully
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n  [!] Invalid input. Please enter a number (1-9).\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: book.addContact();    break;
            case 2: book.displayMenu();   break;
            case 3: book.searchMenu();    break;
            case 4: book.updateContact(); break;
            case 5: book.deleteContact(); break;
            case 6: book.importContacts();break;
            case 7: book.exportBackup();  break;
            case 8: book.showStatistics();break;
            case 9:
                // --- SHUTDOWN: Save all contacts back to file ---
                std::cout << "\n  [*] Saving contacts to file...\n";
                book.saveToFile();
                std::cout << "  [*] All contacts saved. Goodbye!\n\n";
                return 0;
            default:
                std::cout << "\n  [!] Invalid choice. Enter 1-9.\n";
        }
    }
}
