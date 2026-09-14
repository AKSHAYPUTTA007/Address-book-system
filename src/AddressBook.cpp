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

const std::string AddressBook::DATA_FILE   = "data/contacts.txt";
const std::string AddressBook::CONFIG_FILE = "data/config.txt";

// ---- Constructor ----
AddressBook::AddressBook()
    : defaultExtension("+91"), hasLastDeleted(false) {}

// ============================================================
// PRIVATE HELPERS
// ============================================================

// ---- Validation ----

// Email must contain '@', something before it, '.' after it
bool AddressBook::isValidEmail(const std::string& email) const {
    size_t atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0) return false;
    size_t dotPos = email.find('.', atPos + 1);
    if (dotPos == std::string::npos) return false;
    if (dotPos == atPos + 1) return false;          // no domain name
    if (dotPos >= email.length() - 1) return false; // nothing after dot
    return true;
}

// Phone digit part must be 7-15 digits only
bool AddressBook::isValidPhoneDigits(const std::string& digits) const {
    if (digits.empty()) return false;
    int count = 0;
    for (char c : digits) {
        if (!std::isdigit(c)) return false;
        ++count;
    }
    return (count >= 7 && count <= 15);
}

// ---- Phone helpers ----

bool AddressBook::phoneExists(const std::string& phone) const {
    for (const auto& c : contacts)
        if (c.getPhone() == phone) return true;
    return false;
}

bool AddressBook::phoneExistsExcluding(const std::string& phone,
                                        int excludeIndex) const {
    for (int i = 0; i < (int)contacts.size(); ++i) {
        if (i == excludeIndex) continue;
        if (contacts[i].getPhone() == phone) return true;
    }
    return false;
}

int AddressBook::findIndexByPhone(const std::string& phone) const {
    for (int i = 0; i < (int)contacts.size(); ++i)
        if (contacts[i].getPhone() == phone) return i;
    return -1;
}

// ---- Table formatting ----

void AddressBook::printDivider() const {
    std::cout << "+"  << std::string(5,  '-')
              << "+"  << std::string(21, '-')
              << "+"  << std::string(18, '-')
              << "+"  << std::string(27, '-')
              << "+"  << std::string(13, '-')
              << "+"  << std::string(16, '-')
              << "+\n";
}

void AddressBook::printTableHeader() const {
    printDivider();
    std::cout << "| "
              << std::left
              << std::setw(4)  << "No"
              << "| "
              << std::setw(20) << "Name ([*]=Fav)"
              << "| "
              << std::setw(17) << "Phone"
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

// ---- Smart name search — similarity score (0.0 to 1.0) ----
double AddressBook::similarityScore(const std::string& query,
                                    const std::string& target) const {
    std::string q = query, t = target;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (t.find(q) != std::string::npos) return 1.0; // exact substring match

    int common = 0;
    std::string temp = t;
    for (char ch : q) {
        auto pos = temp.find(ch);
        if (pos != std::string::npos) {
            ++common;
            temp.erase(pos, 1);
        }
    }
    return (2.0 * common) / (q.size() + t.size());
}

// ---- Input helpers ----

// Reads a required non-empty trimmed string
std::string AddressBook::inputField(const std::string& prompt) const {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        size_t s = value.find_first_not_of(" \t\r\n");
        size_t e = value.find_last_not_of(" \t\r\n");
        if (s == std::string::npos) {
            std::cout << "  [!] This field cannot be empty. Try again.\n";
            continue;
        }
        return value.substr(s, e - s + 1);
    }
}

// Reads an optional field — returns "" if user just presses Enter
std::string AddressBook::inputOptionalField(const std::string& prompt) const {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    size_t s = value.find_first_not_of(" \t\r\n");
    if (s == std::string::npos) return "";
    size_t e = value.find_last_not_of(" \t\r\n");
    return value.substr(s, e - s + 1);
}

// Category selection menu
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
            default: std::cout << "  [!] Invalid. Enter 1-5.\n";
        }
    }
}

// Phone input: country code + digit validation
std::string AddressBook::inputPhoneWithExtension() const {
    while (true) {
        // --- Country Code ---
        std::cout << "  Country Code [" << defaultExtension << "]: ";
        std::string ext;
        std::getline(std::cin, ext);

        // Trim
        size_t s = ext.find_first_not_of(" \t\r\n");
        if (s == std::string::npos) {
            ext = defaultExtension;   // user pressed Enter → use default
        } else {
            size_t e = ext.find_last_not_of(" \t\r\n");
            ext = ext.substr(s, e - s + 1);
        }

        if (ext.empty() || ext[0] != '+') {
            std::cout << "  [!] Country code must start with '+'. "
                      << "Example: +91, +1, +44\n";
            continue;
        }

        // --- Phone Digits ---
        std::string digits;
        bool digitsOk = false;
        while (!digitsOk) {
            std::cout << "  Phone Number (digits only): ";
            std::getline(std::cin, digits);

            size_t ds = digits.find_first_not_of(" \t\r\n");
            if (ds == std::string::npos) {
                std::cout << "  [!] Phone number cannot be empty.\n";
                continue;
            }
            size_t de = digits.find_last_not_of(" \t\r\n");
            digits = digits.substr(ds, de - ds + 1);

            if (!isValidPhoneDigits(digits)) {
                std::cout << "  [!] Phone must contain 7 to 15 digits only "
                          << "(no spaces or special characters).\n";
                continue;
            }
            digitsOk = true;
        }

        return ext + " " + digits;
    }
}

// ============================================================
// CONFIG FILE I/O
// ============================================================

void AddressBook::loadConfig() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return; // use default if config not found

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("extension=") == 0) {
            defaultExtension = line.substr(10); // after "extension="
            // Trim
            size_t s = defaultExtension.find_first_not_of(" \t\r\n");
            if (s != std::string::npos)
                defaultExtension = defaultExtension.substr(s);
            size_t e = defaultExtension.find_last_not_of(" \t\r\n");
            if (e != std::string::npos)
                defaultExtension = defaultExtension.substr(0, e + 1);
        }
    }
    file.close();
}

void AddressBook::saveConfig() const {
    std::ofstream file(CONFIG_FILE, std::ios::trunc);
    if (!file.is_open()) return;
    file << "extension=" << defaultExtension << "\n";
    file.close();
}

// ============================================================
// DATA FILE I/O
// ============================================================

// Format: name|phone|email|address|category|favourite|birthday
// Backward compatible: 5-field lines load with favourite=0, birthday=""
void AddressBook::loadFromFile() {
    contacts.clear();
    std::ifstream file(DATA_FILE);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|'))
            fields.push_back(token);

        // Must have at least 5 fields
        if (fields.size() < 5) continue;

        // Pad missing fields for old format
        while (fields.size() < 7) fields.push_back("");

        if (phoneExists(fields[1])) continue; // skip duplicate phones

        bool fav   = (fields[5] == "1");
        std::string bday = fields[6];

        contacts.emplace_back(fields[0], fields[1], fields[2],
                               fields[3], fields[4], fav, bday);
    }
    file.close();
}

// Overwrites contacts.txt with all 7 fields
void AddressBook::saveToFile() const {
    std::ofstream file(DATA_FILE, std::ios::trunc);
    if (!file.is_open()) {
        std::cout << "\n  [ERROR] Could not open data file for saving!\n";
        return;
    }
    for (const auto& c : contacts) {
        file << c.getName()                     << "|"
             << c.getPhone()                    << "|"
             << c.getEmail()                    << "|"
             << c.getAddress()                  << "|"
             << c.getCategory()                 << "|"
             << (c.getFavourite() ? "1" : "0")  << "|"
             << c.getBirthday()                 << "\n";
    }
    file.close();
}

// ---- Count ----
int AddressBook::getCount() const {
    return (int)contacts.size();
}

// ============================================================
// FEATURE 1 — ADD CONTACT
// ============================================================
void AddressBook::addContact() {
    std::cout << "\n========================================\n";
    std::cout << "           ADD NEW CONTACT\n";
    std::cout << "========================================\n";

    // --- Name ---
    std::string name = inputField("  Name     : ");

    // Duplicate name WARNING (not rejection)
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (const auto& c : contacts) {
        std::string cLow = c.getName();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        if (cLow == nameLow) {
            std::cout << "\n  [!] Warning: A contact named \""
                      << name << "\" already exists.\n";
            std::cout << "  Continue adding? (y/n): ";
            char ch;
            std::cin >> ch;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (ch != 'y' && ch != 'Y') {
                std::cout << "  Cancelled.\n";
                return;
            }
            break;
        }
    }

    // --- Phone with extension ---
    std::string phone;
    while (true) {
        phone = inputPhoneWithExtension();
        if (phoneExists(phone)) {
            std::cout << "\n  [!] A contact with phone \"" << phone
                      << "\" already exists. Duplicate rejected.\n";
            std::cout << "  Try a different number? (y/n): ";
            char ch;
            std::cin >> ch;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (ch != 'y' && ch != 'Y') { std::cout << "  Cancelled.\n"; return; }
        } else {
            break;
        }
    }

    // --- Email with validation ---
    std::string email;
    while (true) {
        email = inputField("  Email    : ");
        if (isValidEmail(email)) break;
        std::cout << "  [!] Invalid email. Must be like: name@domain.com\n";
    }

    // --- Address & Category ---
    std::string address  = inputField("  Address  : ");
    std::string category = inputCategory();

    // --- Birthday (optional) ---
    std::cout << "\n  Birthday (DD/MM/YYYY, press Enter to skip): ";
    std::string birthday = inputOptionalField("");

    contacts.emplace_back(name, phone, email, address, category, false, birthday);
    std::cout << "\n  [+] Contact added successfully!\n";
}

// ============================================================
// FEATURE 2 — DISPLAY CONTACTS
// ============================================================

void AddressBook::displayAll() const {
    if (contacts.empty()) { std::cout << "\n  No contacts found.\n"; return; }
    std::cout << "\n";
    printTableHeader();
    int i = 1;
    for (const auto& c : contacts) printTableRow(i++, c);
    printTableFooter();
    std::cout << "  Total: " << contacts.size() << " contact(s)\n";
}

void AddressBook::displayByCategory() const {
    std::string cat = inputField("\n  Enter Category: ");
    std::string catLow = cat;
    std::transform(catLow.begin(), catLow.end(), catLow.begin(), ::tolower);

    std::cout << "\n  Contacts in category: " << cat << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string cLow = c.getCategory();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        if (cLow == catLow) printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found in \"" << cat << "\".\n";
    else
        std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::displayByLocation() const {
    std::string loc = inputField("\n  Enter Location: ");
    std::string locLow = loc;
    std::transform(locLow.begin(), locLow.end(), locLow.begin(), ::tolower);

    std::cout << "\n  Contacts in location: " << loc << "\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts) {
        std::string aLow = c.getAddress();
        std::transform(aLow.begin(), aLow.end(), aLow.begin(), ::tolower);
        if (aLow.find(locLow) != std::string::npos) printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0)
        std::cout << "  No contacts found in \"" << loc << "\".\n";
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
        std::string cLow = c.getCategory(), aLow = c.getAddress();
        std::transform(cLow.begin(), cLow.end(), cLow.begin(), ::tolower);
        std::transform(aLow.begin(), aLow.end(), aLow.begin(), ::tolower);
        if (cLow == catLow && aLow.find(locLow) != std::string::npos)
            printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0) std::cout << "  No contacts matched.\n";
    else            std::cout << "  Total: " << count << " contact(s)\n";
}

void AddressBook::displayFavourites() const {
    std::cout << "\n  Favourite Contacts [*]:\n\n";
    printTableHeader();
    int count = 0;
    for (const auto& c : contacts)
        if (c.getFavourite()) printTableRow(++count, c);
    printTableFooter();
    if (count == 0)
        std::cout << "  No favourite contacts yet. Use option 6 to mark one.\n";
    else
        std::cout << "  Total: " << count << " favourite(s)\n";
}

void AddressBook::displayRecentlyAdded() const {
    if (contacts.empty()) { std::cout << "\n  No contacts.\n"; return; }
    std::cout << "\n  Recently Added (Last 5):\n\n";
    printTableHeader();
    int start = (int)contacts.size() - 5;
    if (start < 0) start = 0;
    int serial = 1;
    for (int i = (int)contacts.size() - 1; i >= start; --i)
        printTableRow(serial++, contacts[i]);
    printTableFooter();
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
        std::cout << "  5. Favourites [*]\n";
        std::cout << "  6. Recently Added\n";
        std::cout << "  7. Back\n";
        std::cout << "  Enter Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: displayAll();                  break;
            case 2: displayByCategory();           break;
            case 3: displayByLocation();           break;
            case 4: displayByCategoryAndLocation();break;
            case 5: displayFavourites();           break;
            case 6: displayRecentlyAdded();        break;
            case 7: return;
            default: std::cout << "\n  [!] Invalid choice.\n";
        }
    }
}

// ============================================================
// FEATURE 3 — SEARCH CONTACTS
// ============================================================

void AddressBook::searchByName() const {
    std::string query = inputField("\n  Enter Name (or partial): ");
    std::string queryLow = query;
    std::transform(queryLow.begin(), queryLow.end(), queryLow.begin(), ::tolower);

    std::vector<std::pair<double, int>> matches;
    for (int i = 0; i < (int)contacts.size(); ++i) {
        std::string nLow = contacts[i].getName();
        std::transform(nLow.begin(), nLow.end(), nLow.begin(), ::tolower);
        double score = similarityScore(queryLow, nLow);
        if (score > 0.4) matches.push_back({score, i});
    }

    if (matches.empty()) {
        std::cout << "\n  No contacts matched \"" << query << "\".\n";
        return;
    }

    std::sort(matches.begin(), matches.end(),
              [](const auto& a, const auto& b){ return a.first > b.first; });

    std::cout << "\n  Search Results for \"" << query << "\":\n\n";
    std::cout << "+" << std::string(7,'-') << "+" << std::string(21,'-')
              << "+" << std::string(18,'-') << "+" << std::string(9,'-') << "+\n";
    std::cout << "| " << std::left << std::setw(6) << "Match"
              << "| " << std::setw(20) << "Name"
              << "| " << std::setw(17) << "Phone"
              << "| " << std::setw(8)  << "Score"   << "|\n";
    std::cout << "+" << std::string(7,'-') << "+" << std::string(21,'-')
              << "+" << std::string(18,'-') << "+" << std::string(9,'-') << "+\n";

    int serial = 1;
    for (const auto& m : matches) {
        int pct = (int)std::round(m.first * 100);
        std::cout << "| " << std::left << std::setw(6)  << serial++
                  << "| "             << std::setw(20) << contacts[m.second].getName().substr(0,19)
                  << "| "             << std::setw(17) << contacts[m.second].getPhone().substr(0,16)
                  << "| "             << std::setw(5)  << pct << "%   |\n";
    }
    std::cout << "+" << std::string(7,'-') << "+" << std::string(21,'-')
              << "+" << std::string(18,'-') << "+" << std::string(9,'-') << "+\n";

    std::cout << "\n  Enter serial number to view full details (0 to skip): ";
    int pick;
    std::cin >> pick;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (pick >= 1 && pick <= (int)matches.size())
        contacts[matches[pick - 1].second].display();
}

void AddressBook::searchByPhone() const {
    std::string phone = inputField("\n  Enter Phone Number (with code, e.g. +91 9876543210): ");
    int idx = findIndexByPhone(phone);
    if (idx == -1)
        std::cout << "\n  No contact found with phone: " << phone << "\n";
    else
        contacts[idx].display();
}

void AddressBook::searchByEmail() const {
    std::string email = inputField("\n  Enter Email Address: ");
    std::string eLowQ = email;
    std::transform(eLowQ.begin(), eLowQ.end(), eLowQ.begin(), ::tolower);

    bool found = false;
    for (const auto& c : contacts) {
        std::string eLow = c.getEmail();
        std::transform(eLow.begin(), eLow.end(), eLow.begin(), ::tolower);
        if (eLow == eLowQ) { c.display(); found = true; }
    }
    if (!found) std::cout << "\n  No contact found with email: " << email << "\n";
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
        if (cLow == catLow) printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0) std::cout << "  No contacts found.\n";
    else            std::cout << "  Total: " << count << " contact(s)\n";
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
        if (aLow.find(locLow) != std::string::npos) printTableRow(++count, c);
    }
    printTableFooter();
    if (count == 0) std::cout << "  No contacts found.\n";
    else            std::cout << "  Total: " << count << " contact(s)\n";
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
    std::cout << "\n  Enter new values (press Enter to keep existing):\n\n";

    // Helper: press Enter = keep current
    auto optField = [&](const std::string& prompt,
                        const std::string& current) -> std::string {
        std::cout << prompt << " [" << current << "]: ";
        std::string val;
        std::getline(std::cin, val);
        size_t s = val.find_first_not_of(" \t\r\n");
        if (s == std::string::npos) return current;
        return val.substr(s, val.find_last_not_of(" \t\r\n") - s + 1);
    };

    std::string newName = optField("  Name     ", contacts[idx].getName());

    // Phone change: separate flow
    std::cout << "\n  Change phone number? (y/n): ";
    char chPhone;
    std::cin >> chPhone;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string newPhone = contacts[idx].getPhone();
    if (chPhone == 'y' || chPhone == 'Y') {
        while (true) {
            newPhone = inputPhoneWithExtension();
            if (newPhone != contacts[idx].getPhone() &&
                phoneExistsExcluding(newPhone, idx)) {
                std::cout << "  [!] Phone " << newPhone
                          << " belongs to another contact. Try again? (y/n): ";
                char retry;
                std::cin >> retry;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (retry != 'y' && retry != 'Y') {
                    std::cout << "  Update cancelled.\n";
                    return;
                }
            } else {
                break;
            }
        }
    }

    // Email change with validation
    std::string newEmail = contacts[idx].getEmail();
    std::cout << "  Email     [" << newEmail << "]: ";
    std::string emailInput;
    std::getline(std::cin, emailInput);
    size_t es = emailInput.find_first_not_of(" \t\r\n");
    if (es != std::string::npos) {
        emailInput = emailInput.substr(es, emailInput.find_last_not_of(" \t\r\n") - es + 1);
        if (!isValidEmail(emailInput)) {
            std::cout << "  [!] Invalid email format. Keeping old email.\n";
        } else {
            newEmail = emailInput;
        }
    }

    std::string newAddress  = optField("  Address  ", contacts[idx].getAddress());

    // Category
    std::cout << "\n  Change category? (y/n): ";
    char chCat;
    std::cin >> chCat;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string newCategory = contacts[idx].getCategory();
    if (chCat == 'y' || chCat == 'Y') newCategory = inputCategory();

    // Birthday
    std::string newBirthday = optField("  Birthday ", contacts[idx].getBirthday());

    // Apply
    if (newName.empty()) { std::cout << "  [!] Name cannot be empty. Cancelled.\n"; return; }
    contacts[idx].setName(newName);
    contacts[idx].setPhone(newPhone);
    contacts[idx].setEmail(newEmail);
    contacts[idx].setAddress(newAddress);
    contacts[idx].setCategory(newCategory);
    contacts[idx].setBirthday(newBirthday);

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
    std::cout << "  1. Yes\n  2. No\n";
    std::cout << "  Enter Choice: ";
    int ch;
    std::cin >> ch;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ch == 1) {
        lastDeletedContact = contacts[idx];  // save for undo
        hasLastDeleted     = true;
        contacts.erase(contacts.begin() + idx);
        std::cout << "\n  [-] Contact deleted. (Use option 11 to undo)\n";
    } else {
        std::cout << "\n  Deletion cancelled.\n";
    }
}

// ============================================================
// FEATURE — MARK / UNMARK FAVOURITE
// ============================================================
void AddressBook::toggleFavourite() {
    std::cout << "\n========================================\n";
    std::cout << "      MARK / UNMARK FAVOURITE\n";
    std::cout << "========================================\n";

    std::string phone = inputField("  Enter Phone Number: ");
    int idx = findIndexByPhone(phone);
    if (idx == -1) {
        std::cout << "\n  [!] No contact found with phone: " << phone << "\n";
        return;
    }

    contacts[idx].setFavourite(!contacts[idx].getFavourite());
    std::string status = contacts[idx].getFavourite()
                         ? "marked as Favourite [*]"
                         : "removed from Favourites";
    std::cout << "\n  [+] \"" << contacts[idx].getName()
              << "\" has been " << status << ".\n";
}

// ============================================================
// FEATURE — SORT CONTACTS
// ============================================================
void AddressBook::sortMenu() {
    if (contacts.empty()) {
        std::cout << "\n  No contacts to sort.\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "           SORT CONTACTS\n";
    std::cout << "========================================\n";
    std::cout << "  1. By Name  (A → Z)\n";
    std::cout << "  2. By Name  (Z → A)\n";
    std::cout << "  3. By Category\n";
    std::cout << "  4. Favourites First\n";
    std::cout << "  5. Back\n";
    std::cout << "  Enter Choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
        case 1:
            std::sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b){
                    std::string na = a.getName(), nb = b.getName();
                    std::transform(na.begin(), na.end(), na.begin(), ::tolower);
                    std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
                    return na < nb;
                });
            std::cout << "\n  [+] Sorted by Name (A → Z).\n";
            break;
        case 2:
            std::sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b){
                    std::string na = a.getName(), nb = b.getName();
                    std::transform(na.begin(), na.end(), na.begin(), ::tolower);
                    std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
                    return na > nb;
                });
            std::cout << "\n  [+] Sorted by Name (Z → A).\n";
            break;
        case 3:
            std::sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b){
                    return a.getCategory() < b.getCategory();
                });
            std::cout << "\n  [+] Sorted by Category.\n";
            break;
        case 4:
            std::stable_sort(contacts.begin(), contacts.end(),
                [](const Contact& a, const Contact& b){
                    return (int)a.getFavourite() > (int)b.getFavourite();
                });
            std::cout << "\n  [+] Favourites moved to top.\n";
            break;
        case 5:
            return;
        default:
            std::cout << "\n  [!] Invalid choice.\n";
    }
}

// ============================================================
// FEATURE — UNDO LAST DELETE
// ============================================================
void AddressBook::undoDelete() {
    if (!hasLastDeleted) {
        std::cout << "\n  [!] Nothing to undo. No contact has been deleted.\n";
        return;
    }
    contacts.push_back(lastDeletedContact);
    std::cout << "\n  [+] Contact restored:\n";
    lastDeletedContact.display();
    hasLastDeleted = false;
}

// ============================================================
// FEATURE — IMPORT CONTACTS
// ============================================================
void AddressBook::importContacts() {
    std::cout << "\n========================================\n";
    std::cout << "          IMPORT CONTACTS\n";
    std::cout << "========================================\n";

    std::string path = inputField("  Enter file path: ");
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "\n  [!] Could not open: " << path << "\n";
        return;
    }

    int imported = 0, skipped = 0, invalid = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|'))
            fields.push_back(token);

        if (fields.size() < 5) { ++invalid; continue; }

        bool hasEmpty = false;
        for (int i = 0; i < 5; ++i)
            if (fields[i].find_first_not_of(" \t\r\n") == std::string::npos)
                hasEmpty = true;
        if (hasEmpty) { ++invalid; continue; }

        if (phoneExists(fields[1])) { ++skipped; continue; }

        while (fields.size() < 7) fields.push_back("");
        bool fav = (fields[5] == "1");
        contacts.emplace_back(fields[0], fields[1], fields[2],
                               fields[3], fields[4], fav, fields[6]);
        ++imported;
    }
    file.close();

    std::cout << "\n  ---- Import Summary ----\n";
    std::cout << "  Imported : " << imported << "\n";
    std::cout << "  Skipped  : " << skipped  << " (duplicate phones)\n";
    std::cout << "  Invalid  : " << invalid  << " (malformed lines)\n";
}

// ============================================================
// FEATURE — EXPORT BACKUP
// ============================================================
void AddressBook::exportBackup() const {
    std::cout << "\n========================================\n";
    std::cout << "          EXPORT BACKUP\n";
    std::cout << "========================================\n";

    std::string path = inputField("  Enter destination path: ");
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        std::cout << "\n  [!] Could not create: " << path << "\n";
        return;
    }
    for (const auto& c : contacts) {
        file << c.getName()                    << "|"
             << c.getPhone()                   << "|"
             << c.getEmail()                   << "|"
             << c.getAddress()                 << "|"
             << c.getCategory()                << "|"
             << (c.getFavourite() ? "1" : "0") << "|"
             << c.getBirthday()                << "\n";
    }
    file.close();
    std::cout << "\n  [+] Backup created at: " << path << "\n";
}

// ============================================================
// FEATURE — STATISTICS
// ============================================================
void AddressBook::showStatistics() const {
    std::cout << "\n========================================\n";
    std::cout << "           STATISTICS\n";
    std::cout << "========================================\n";

    int favCount = 0;
    for (const auto& c : contacts)
        if (c.getFavourite()) ++favCount;

    std::cout << "  Total Contacts   : " << contacts.size() << "\n";
    std::cout << "  Favourites       : " << favCount        << "\n\n";

    std::map<std::string, int> catCount, locCount;
    for (const auto& c : contacts) {
        catCount[c.getCategory()]++;
        locCount[c.getAddress()]++;
    }

    std::cout << "  Category Count:\n";
    std::cout << "  " << std::string(30, '-') << "\n";
    std::vector<std::string> knownCats = {"College","Friends","Family","Work","Other"};
    for (const auto& cat : knownCats)
        if (catCount.count(cat))
            std::cout << "  " << std::left << std::setw(12) << cat
                      << " : " << catCount[cat] << "\n";
    for (const auto& kv : catCount) {
        bool known = false;
        for (const auto& k : knownCats) if (k == kv.first) { known=true; break; }
        if (!known)
            std::cout << "  " << std::left << std::setw(12) << kv.first
                      << " : " << kv.second << "\n";
    }

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
        if (++shown >= 10) break;
    }
    std::cout << "\n";
}

// ============================================================
// FEATURE — SETTINGS (Change Default Extension)
// ============================================================
void AddressBook::showSettings() {
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "             SETTINGS\n";
        std::cout << "========================================\n";
        std::cout << "  Default Country Code : " << defaultExtension << "\n\n";
        std::cout << "  1. Change Default Country Code\n";
        std::cout << "  2. Back\n";
        std::cout << "  Enter Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 2) return;

        if (choice == 1) {
            while (true) {
                std::cout << "  Enter new country code (e.g. +1, +44, +91): ";
                std::string ext;
                std::getline(std::cin, ext);
                size_t s = ext.find_first_not_of(" \t\r\n");
                if (s == std::string::npos || ext[s] != '+') {
                    std::cout << "  [!] Must start with '+'. Try again.\n";
                    continue;
                }
                ext = ext.substr(s, ext.find_last_not_of(" \t\r\n") - s + 1);
                defaultExtension = ext;
                saveConfig();
                std::cout << "\n  [+] Default country code changed to: "
                          << defaultExtension << "\n";
                break;
            }
        } else {
            std::cout << "\n  [!] Invalid choice.\n";
        }
    }
}
