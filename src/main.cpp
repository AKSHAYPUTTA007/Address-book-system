#include <iostream>
#include <limits>
#include "../include/AddressBook.h"

// ============================================================
// Address Book System — Entry Point
//
// Startup  : Load config (default extension) + contacts from file
// Runtime  : All operations work on the in-RAM vector<Contact>
// Shutdown : Save contacts + config back to file on exit
// ============================================================

void printMainMenu(int contactCount) {
    std::cout << "\n";
    std::cout << "=====================================\n";
    std::cout << "       ADDRESS BOOK SYSTEM\n";
    std::cout << "       Contacts: " << contactCount << "\n";
    std::cout << "=====================================\n";
    std::cout << "  1.  Add Contact\n";
    std::cout << "  2.  Display Contacts\n";
    std::cout << "  3.  Search Contacts\n";
    std::cout << "  4.  Update Contact\n";
    std::cout << "  5.  Delete Contact\n";
    std::cout << "  6.  Mark / Unmark Favourite\n";
    std::cout << "  7.  Sort Contacts\n";
    std::cout << "  8.  Import Contacts\n";
    std::cout << "  9.  Export Backup\n";
    std::cout << " 10.  Statistics\n";
    std::cout << " 11.  Undo Last Delete\n";
    std::cout << " 12.  Settings\n";
    std::cout << " 13.  Exit\n";
    std::cout << "=====================================\n";
    std::cout << "  Enter Choice: ";
}

int main() {
    AddressBook book;

    // --- STARTUP: Load config (default extension), then contacts ---
    std::cout << "\n  [*] Loading settings...\n";
    book.loadConfig();
    std::cout << "  [*] Loading contacts from file...\n";
    book.loadFromFile();
    std::cout << "  [*] Ready! " << book.getCount() << " contact(s) loaded.\n";

    int choice = 0;

    while (true) {
        printMainMenu(book.getCount());

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n  [!] Invalid input. Enter a number (1-13).\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case  1: book.addContact();    break;
            case  2: book.displayMenu();   break;
            case  3: book.searchMenu();    break;
            case  4: book.updateContact(); break;
            case  5: book.deleteContact(); break;
            case  6: book.toggleFavourite(); break;
            case  7: book.sortMenu();      break;
            case  8: book.importContacts();break;
            case  9: book.exportBackup();  break;
            case 10: book.showStatistics();break;
            case 11: book.undoDelete();    break;
            case 12: book.showSettings();  break;
            case 13:
                std::cout << "\n  [*] Saving contacts to file...\n";
                book.saveToFile();
                book.saveConfig();
                std::cout << "  [*] All data saved. Goodbye!\n\n";
                return 0;
            default:
                std::cout << "\n  [!] Invalid choice. Enter 1-13.\n";
        }
    }
}
