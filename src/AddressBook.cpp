#include "../include/AddressBook.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <limits>

// ============================================================
// AddressBook Class Implementation
// ============================================================

// Data file path (relative to project root where binary runs)
const std::string AddressBook::DATA_FILE = "data/contacts.txt";

// ---- Constructor ----
AddressBook::AddressBook() {}

// ============================================================
// PRIVATE HELPERS
// ============================================================

// Returns true if a contact with the given phone already exists
bool AddressBook::phoneExists(const std::string& phone) const {
    for (const auto& c : contacts)
        if (c.getPhone() == phone) return true;
    return false;
}

// Returns true if phone exists in any contact EXCEPT the one at excludeIndex
bool AddressBook::phoneExistsExcluding(const std::string& phone,
                                        int excludeIndex) const {
    for (int i = 0; i < (int)contacts.size(); ++i) {
        if (i == excludeIndex) continue;
        if (contacts[i].getPhone() == phone) return true;
    }
    return false;
}

// Returns the index of the contact with the given phone, or -1
int AddressBook::findIndexByPhone(const std::string& phone) const {
    for (int i = 0; i < (int)contacts.size(); ++i)
        if (contacts[i].getPhone() == phone) return i;
    return -1;
}

// Table formatting helpers
void AddressBook::printDivider() const {
    std::cout << "+" << std::string(5, '-')
              << "+" << std::string(21, '-')
              << "+" << std::string(15, '-')
              << "+" << std::string(27, '-')
              << "+" << std::string(13, '-')
              << "+" << std::string(16, '-')
              << "+\n";
}

void AddressBook::printTableHeader() const {
    printDivider();
    std::cout << "| "
              << std::left
              << std::setw(4)  << "No"
              << "| "
              << std::setw(20) << "Name"
              << "| "
              << std::setw(14) << "Phone"
              << "| "
              << std::setw(26) << "Email"
              << "| "
              << std::setw(12) << "Category"
              << "| "
              << std::setw(15) << "Location"
              << "|\n";
    printDivider();
}

void AddressBook::printTableFooter() const {
    printDivider();
}

void AddressBook::printTableRow(int serial, const Contact& c) const {
    c.displayRow(serial);
}

// ---- Simple case-insensitive similarity score (0.0 – 1.0) ----
// Uses: common prefix length + character overlap heuristic
double AddressBook::similarityScore(const std::string& query,
                                    const std::string& target) const {
    // Convert both to lowercase
    std::string q = query, t = target;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (t.find(q) != std::string::npos) return 1.0; // exact substring

    // Count common characters
    int common = 0;
    std::string temp = t;
    for (char ch : q) {
        auto pos = temp.find(ch);
        if (pos != std::string::npos) {
            ++common;
            temp.erase(pos, 1);
        }
    }
    double score = (2.0 * common) / (q.size() + t.size());
    return score;
}

// Reads a trimmed non-empty string from stdin
std::string AddressBook::inputField(const std::string& prompt) const {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        // Trim leading/trailing spaces
        size_t start = value.find_first_not_of(" \t\r\n");
        size_t end   = value.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "  [!] This field cannot be empty. Try again.\n";
            continue;
        }
        value = value.substr(start, end - start + 1);
        if (value.empty()) {
            std::cout << "  [!] This field cannot be empty. Try again.\n";
            continue;
        }
        return value;
    }
}

// Displays a category menu and returns the selected category string
std::string AddressBook::inputCategory() const {
    std::cout << "\n  Categories:\n"
              << "  1. Family\n"
              << "  2. Friends\n"
              << "  3. College\n"
              << "  4. Work\n"
              << "  5. Other\n";
    int ch = 0;
    while (true) {
        std::cout << "  Select category (1-5): ";
        std::cin >> ch;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (ch) {
            case 1: return "Family";
            case 2: return "Friends";
            case 3: return "College";
            case 4: return "Work";
            case 5: return "Other";
            default:
                std::cout << "  [!] Invalid choice. Enter 1-5.\n";
        }
    }
}

// ============================================================
// FILE I/O
// ============================================================

// Reads contacts.txt and loads all contacts into RAM
void AddressBook::loadFromFile() {
    contacts.clear();
    std::ifstream file(DATA_FILE);
    if (!file.is_open()) {
        // File might not exist on first run — that's fine
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip blank lines
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|'))
            fields.push_back(token);

        if (fields.size() != 5) continue; // malformed line

        // Skip if phone duplicate (shouldn't happen in well-formed files)
        if (phoneExists(fields[1])) continue;

        contacts.emplace_back(fields[0], fields[1],
                               fields[2], fields[3], fields[4]);
    }
    file.close();
}

// Overwrites contacts.txt with every contact currently in RAM
void AddressBook::saveToFile() const {
    std::ofstream file(DATA_FILE, std::ios::trunc);
    if (!file.is_open()) {
        std::cout << "\n  [ERROR] Could not open data file for saving!\n";
        return;
    }
    for (const auto& c : contacts) {
        file << c.getName()     << "|"
             << c.getPhone()    << "|"
             << c.getEmail()    << "|"
             << c.getAddress()  << "|"
             << c.getCategory() << "\n";
    }
    file.close();
}

// ============================================================
// FEATURE 1 — ADD CONTACT
// ============================================================
void AddressBook::addContact() {
    std::cout << "\n========================================\n";
    std::cout << "           ADD NEW CONTACT\n";
    std::cout << "========================================\n";

    std::string name     = inputField("  Name     : ");
    std::string phone    = inputField("  Phone    : ");

    if (phoneExists(phone)) {
        std::cout << "\n  [!] A contact with phone " << phone
                  << " already exists. Duplicate rejected.\n";
        return;
    }

    std::string email    = inputField("  Email    : ");
    std::string address  = inputField("  Address  : ");
    std::string category = inputCategory();

    contacts.emplace_back(name, phone, email, address, category);
    std::cout << "\n  [+] Contact added successfully!\n";
}

// ============================================================
// FEATURE 2 — DISPLAY CONTACTS
// ============================================================

void AddressBook::displayAll() const {
    if (contacts.empty()) {
        std::cout << "\n  No contacts found.\n";
        return;
    }
    std::cout << "\n";
    printTableHeader();
    int i = 1;
    for (const auto& c : contacts)
        printTableRow(i++, c);
    printTableFooter();
    std::cout << "  Total: " << contacts.size() << " contact(s)\n";
}

void AddressBook::displayByCategory() const {
    std::string cat = inputField("\n  Enter Category: ");
    std::string catLow = cat;
    std::transform(catLow.begin(), catLow.end(), catLow.begin(), ::tolower);

    std::cout << "\n  Showing contacts in category: " << cat << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string cLow = c.getCategory();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        if (cLow == catLow) {
            printTableRow(++count, c);
        }
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found in category \"" << cat << "\".\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::displayByLocation() const {
    std::string loc = inputField("\n  Enter Location: ");
    std::string locLow = loc;
    std::transform(locLow.begin(), locLow.end(), locLow.begin(), ::tolower);

    std::cout << "\n  Showing contacts in location: " << loc << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string aLow = c.getAddress();
        std::transform(aLow.begin(), aLow.end(), aLow.begin(), ::tolower);
        if (aLow.find(locLow) != std::string::npos) {
            printTableRow(++count, c);
        }
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found in location \"" << loc << "\".\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::displayByCategoryAndLocation() const {
    std::string cat = inputField("\n  Enter Category : ");
    std::string loc = inputField("  Enter Location : ");

    std::string catLow = cat, locLow = loc;
    std::transform(catLow.begin(), catLow.end(), catLow.begin(), ::tolower);
    std::transform(locLow.begin(), locLow.end(), locLow.begin(), ::tolower);

    std::cout << "\n  Category: " << cat << "  |  Location: " << loc << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string cLow = c.getCategory();
        std::string aLow = c.getAddress();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        std::transform(aLow.begin(), aLow.end(), aLow.begin(), ::tolower);
        if (cLow == catLow && aLow.find(locLow) != std::string::npos) {
            printTableRow(++count, c);
        }
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts matched.\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::displayMenu() {
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "         DISPLAY CONTACTS\n";
        std::cout << "========================================\n";
        std::cout << "  1. All Contacts\n";
        std::cout << "  2. By Category\n";
        std::cout << "  3. By Location\n";
        std::cout << "  4. By Category + Location\n";
        std::cout << "  5. Back\n";
        std::cout << "  Enter Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: displayAll();                  break;
            case 2: displayByCategory();           break;
            case 3: displayByLocation();           break;
            case 4: displayByCategoryAndLocation();break;
            case 5: return;
            default: std::cout << "\n  [!] Invalid choice.\n";
        }
    }
}

// ============================================================
// FEATURE 3 — ADVANCED SEARCH
// ============================================================

void AddressBook::searchByName() const {
    std::string query = inputField("\n  Enter Name (or partial): ");
    std::string queryLow = query;
    std::transform(queryLow.begin(), queryLow.end(), queryLow.begin(), ::tolower);

    // Build match list: {score, index}
    std::vector<std::pair<double, int>> matches;
    for (int i = 0; i < (int)contacts.size(); ++i) {
        std::string nameLow = contacts[i].getName();
        std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
        double score = similarityScore(queryLow, nameLow);
        if (score > 0.4)
            matches.push_back({score, i});
    }

    if (matches.empty()) {
        std::cout << "\n  No contacts matched \"" << query << "\".\n";
        return;
    }

    // Sort by score descending
    std::sort(matches.begin(), matches.end(),
              [](const auto& a, const auto& b){ return a.first > b.first; });

    // Display match table
    std::cout << "\n  Search Results for \"" << query << "\":\n\n";
    std::cout << "+" << std::string(7,'-')
              << "+" << std::string(21,'-')
              << "+" << std::string(15,'-')
              << "+" << std::string(9,'-') << "+\n";
    std::cout << "| "
              << std::left << std::setw(6)  << "Match"
              << "| "
              << std::setw(20) << "Name"
              << "| "
              << std::setw(14) << "Phone"
              << "| "
              << std::setw(8) << "Score"
              << "|\n";
    std::cout << "+" << std::string(7,'-')
              << "+" << std::string(21,'-')
              << "+" << std::string(15,'-')
              << "+" << std::string(9,'-') << "+\n";

    int serial = 1;
    for (const auto& m : matches) {
        int pct = (int)std::round(m.first * 100);
        std::cout << "| "
                  << std::left << std::setw(6)  << serial++
                  << "| "
                  << std::setw(20) << contacts[m.second].getName().substr(0,19)
                  << "| "
                  << std::setw(14) << contacts[m.second].getPhone().substr(0,13)
                  << "| "
                  << std::setw(5)  << pct << "%   "
                  << "|\n";
    }
    std::cout << "+" << std::string(7,'-')
              << "+" << std::string(21,'-')
              << "+" << std::string(15,'-')
              << "+" << std::string(9,'-') << "+\n";

    // Let user pick one to view full details
    std::cout << "\n  Enter serial number to view full details (0 to skip): ";
    int pick;
    std::cin >> pick;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (pick >= 1 && pick <= (int)matches.size())
        contacts[matches[pick - 1].second].display();
}

void AddressBook::searchByPhone() const {
    std::string phone = inputField("\n  Enter Phone Number: ");
    int idx = findIndexByPhone(phone);
    if (idx == -1)
        std::cout << "\n  No contact found with phone: " << phone << "\n";
    else
        contacts[idx].display();
}

void AddressBook::searchByEmail() const {
    std::string email = inputField("\n  Enter Email Address: ");
    std::string emailLow = email;
    std::transform(emailLow.begin(), emailLow.end(), emailLow.begin(), ::tolower);

    bool found = false;
    for (const auto& c : contacts) {
        std::string eLow = c.getEmail();
        std::transform(eLow.begin(), eLow.end(), eLow.begin(), ::tolower);
        if (eLow == emailLow) {
            c.display();
            found = true;
        }
    }
    if (!found)
        std::cout << "\n  No contact found with email: " << email << "\n";
}

void AddressBook::searchByCategory() const {
    std::string cat = inputField("\n  Enter Category: ");
    std::string catLow = cat;
    std::transform(catLow.begin(), catLow.end(), catLow.begin(), ::tolower);

    std::cout << "\n  Contacts in category: " << cat << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string cLow = c.getCategory();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        if (cLow == catLow)
            printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found.\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::searchByLocation() const {
    std::string loc = inputField("\n  Enter Location: ");
    std::string locLow = loc;
    std::transform(locLow.begin(), locLow.end(), locLow.begin(), ::tolower);

    std::cout << "\n  Contacts in location: " << loc << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string aLow = c.getAddress();
        std::transform(aLow.begin(), aLow.end(), aLow.begin(), ::tolower);
        if (aLow.find(locLow) != std::string::npos)
            printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found.\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::searchMenu() {
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "         SEARCH CONTACTS\n";
        std::cout << "========================================\n";
        std::cout << "  1. By Name\n";
        std::cout << "  2. By Phone Number\n";
        std::cout << "  3. By Email\n";
        std::cout << "  4. By Category\n";
        std::cout << "  5. By Location\n";
        std::cout << "  6. Back\n";
        std::cout << "  Enter Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: searchByName();     break;
            case 2: searchByPhone();    break;
            case 3: searchByEmail();    break;
            case 4: searchByCategory(); break;
            case 5: searchByLocation(); break;
            case 6: return;
            default: std::cout << "\n  [!] Invalid choice.\n";
        }
    }
}

// ============================================================
// FEATURE 4 — UPDATE CONTACT
// ============================================================
void AddressBook::updateContact() {
    std::cout << "\n========================================\n";
    std::cout << "           UPDATE CONTACT\n";
    std::cout << "========================================\n";

    std::string phone = inputField("  Enter Phone Number to Update: ");
    int idx = findIndexByPhone(phone);
    if (idx == -1) {
        std::cout << "\n  [!] No contact found with phone: " << phone << "\n";
        return;
    }

    std::cout << "\n  Current Details:";
    contacts[idx].display();

    std::cout << "\n  Enter new details (press Enter to keep existing):\n\n";

    // Helper lambda to read optional override
    auto optField = [&](const std::string& prompt,
                        const std::string& current) -> std::string {
        std::cout << prompt << " [" << current << "]: ";
        std::string val;
        std::getline(std::cin, val);
        // Trim
        size_t s = val.find_first_not_of(" \t\r\n");
        if (s == std::string::npos) return current;
        size_t e = val.find_last_not_of(" \t\r\n");
        return val.substr(s, e - s + 1);
    };

    std::string newName  = optField("  Name    ", contacts[idx].getName());
    std::string newPhone = optField("  Phone   ", contacts[idx].getPhone());

    // Validate new phone uniqueness
    if (newPhone != contacts[idx].getPhone() &&
        phoneExistsExcluding(newPhone, idx)) {
        std::cout << "\n  [!] Phone " << newPhone
                  << " belongs to another contact. Update cancelled.\n";
        return;
    }

    std::string newEmail   = optField("  Email   ", contacts[idx].getEmail());
    std::string newAddress = optField("  Address ", contacts[idx].getAddress());

    // Category: show menu or keep existing
    std::cout << "\n  Change category? (y/n): ";
    char ch;
    std::cin >> ch;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string newCategory = contacts[idx].getCategory();
    if (ch == 'y' || ch == 'Y')
        newCategory = inputCategory();

    // Validate name/email not empty (they kept existing so they're fine)
    if (newName.empty() || newPhone.empty() ||
        newEmail.empty() || newAddress.empty()) {
        std::cout << "\n  [!] Fields cannot be empty. Update cancelled.\n";
        return;
    }

    contacts[idx].setName(newName);
    contacts[idx].setPhone(newPhone);
    contacts[idx].setEmail(newEmail);
    contacts[idx].setAddress(newAddress);
    contacts[idx].setCategory(newCategory);

    std::cout << "\n  [+] Contact updated successfully!\n";
    std::cout << "\n  Updated Details:";
    contacts[idx].display();
}

// ============================================================
// FEATURE 5 — DELETE CONTACT
// ============================================================
void AddressBook::deleteContact() {
    std::cout << "\n========================================\n";
    std::cout << "           DELETE CONTACT\n";
    std::cout << "========================================\n";

    std::string phone = inputField("  Enter Phone Number to Delete: ");
    int idx = findIndexByPhone(phone);
    if (idx == -1) {
        std::cout << "\n  [!] No contact found with phone: " << phone << "\n";
        return;
    }

    contacts[idx].display();

    std::cout << "\n  Delete this contact?\n";
    std::cout << "  1. Yes\n";
    std::cout << "  2. No\n";
    std::cout << "  Enter Choice: ";
    int ch;
    std::cin >> ch;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ch == 1) {
        contacts.erase(contacts.begin() + idx);
        std::cout << "\n  [-] Contact deleted successfully!\n";
    } else {
        std::cout << "\n  Deletion cancelled.\n";
    }
}

// ============================================================
// FEATURE 6 — IMPORT CONTACTS
// ============================================================
void AddressBook::importContacts() {
    std::cout << "\n========================================\n";
    std::cout << "          IMPORT CONTACTS\n";
    std::cout << "========================================\n";

    std::string path = inputField("  Enter file path: ");
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "\n  [!] Could not open file: " << path << "\n";
        return;
    }

    int imported = 0, skipped = 0, invalid = 0;
    std::string line;
    while (std::getline(file, line)) {
        // Skip blank lines
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|'))
            fields.push_back(token);

        if (fields.size() != 5) {
            ++invalid;
            continue;
        }

        // Skip any field that is empty
        bool hasEmpty = false;
        for (const auto& f : fields)
            if (f.find_first_not_of(" \t\r\n") == std::string::npos)
                hasEmpty = true;
        if (hasEmpty) { ++invalid; continue; }

        if (phoneExists(fields[1])) {
            ++skipped;
            continue;
        }

        contacts.emplace_back(fields[0], fields[1],
                               fields[2], fields[3], fields[4]);
        ++imported;
    }
    file.close();

    std::cout << "\n  ---- Import Summary ----\n";
    std::cout << "  Imported : " << imported << "\n";
    std::cout << "  Skipped  : " << skipped  << " (duplicate phones)\n";
    std::cout << "  Invalid  : " << invalid  << " (malformed lines)\n";
}

// ============================================================
// FEATURE 7 — EXPORT BACKUP
// ============================================================
void AddressBook::exportBackup() const {
    std::cout << "\n========================================\n";
    std::cout << "          EXPORT BACKUP\n";
    std::cout << "========================================\n";

    std::string path = inputField("  Enter destination path: ");
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        std::cout << "\n  [!] Could not create file: " << path << "\n";
        return;
    }

    for (const auto& c : contacts) {
        file << c.getName()     << "|"
             << c.getPhone()    << "|"
             << c.getEmail()    << "|"
             << c.getAddress()  << "|"
             << c.getCategory() << "\n";
    }
    file.close();
    std::cout << "\n  [+] Backup created successfully at: " << path << "\n";
}

// ============================================================
// FEATURE 8 — STATISTICS
// ============================================================
void AddressBook::showStatistics() const {
    std::cout << "\n========================================\n";
    std::cout << "           STATISTICS\n";
    std::cout << "========================================\n";

    std::cout << "  Total Contacts : " << contacts.size() << "\n\n";

    // Category count
    std::map<std::string, int> catCount;
    std::map<std::string, int> locCount;

    for (const auto& c : contacts) {
        catCount[c.getCategory()]++;
        locCount[c.getAddress()]++;
    }

    // --- Category breakdown ---
    std::cout << "  Category Count:\n";
    std::cout << "  " << std::string(30, '-') << "\n";
    // Print known categories first, then any others
    std::vector<std::string> knownCats = {"College","Friends","Family","Work","Other"};
    for (const auto& cat : knownCats) {
        if (catCount.count(cat))
            std::cout << "  " << std::left << std::setw(12) << cat
                      << " : " << catCount[cat] << "\n";
    }
    for (const auto& kv : catCount) {
        bool isKnown = false;
        for (const auto& k : knownCats)
            if (k == kv.first) { isKnown = true; break; }
        if (!isKnown)
            std::cout << "  " << std::left << std::setw(12) << kv.first
                      << " : " << kv.second << "\n";
    }

    // --- Top Locations ---
    // Sort locations by count descending
    std::vector<std::pair<int, std::string>> locSorted;
    for (const auto& kv : locCount)
        locSorted.push_back({kv.second, kv.first});
    std::sort(locSorted.begin(), locSorted.end(),
              [](const auto& a, const auto& b){ return a.first > b.first; });

    std::cout << "\n  Top Locations:\n";
    std::cout << "  " << std::string(30, '-') << "\n";
    int shown = 0;
    for (const auto& kv : locSorted) {
        std::cout << "  " << std::left << std::setw(16) << kv.second
                  << " : " << kv.first << "\n";
        if (++shown >= 10) break; // show at most top 10
    }
    std::cout << "\n";
}
