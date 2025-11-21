#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ----- Colors -----
const string RESET  = "\033[0m";
const string RED    = "\033[31m";
const string GREEN  = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE   = "\033[34m";
const string CYAN   = "\033[36m";
const string BOLD   = "\033[1m";

// ----- DEMO DATASET (mock AUC locations) -----
vector<string> allLocations = {
    "SSE CP26",
    "HUSS CP26",
    "Library Entrance",
    "Plaza",
    "Food Court",
    "Pepsi Station",
    "SU Help Desk",
    "SSE Main Entrance",
    "Sports Court",
    "Masjid",
    "COB 150",
    "COB 150 Ground Floor",
    "COB 150 First Floor"
};

// ----- DEMO CATEGORIES -----
vector<string> FOOD = {"Food Court", "Pepsi Station"};
vector<string> DRINKS = {"Pepsi Station"};
vector<string> SUPERMARKET = {"Plaza"};
vector<string> PRAYER = {"Masjid"};

// ----- Utility: lowercase -----
string lower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// ----- Find matching locations (mock logic) -----
vector<string> findMatchingLocations(const string& query) {
    vector<string> result;
    string q = lower(query);

    for (const string& loc : allLocations) {
        if (lower(loc).find(q) != string::npos)
            result.push_back(loc);
    }
    return result;
}

// ----- Disambiguate selection -----
string disambiguate(const vector<string>& options) {
    if (options.size() == 1)
        return options[0];

    cout << YELLOW << "Did you mean:\n" << RESET;
    for (int i = 0; i < options.size(); i++)
        cout << "  " << (i+1) << ") " << options[i] << "\n";

    cout << BLUE << "Choose: " << RESET;
    int choice;
    cin >> choice;
    cin.ignore();
    return options[choice - 1];
}

// ----- Mock Dijkstra path -----
string runDijkstra(const string& from, const string& to) {
    string output;
    output += BOLD + CYAN + "Path:\n" + RESET;

    // Fake paths for testing
    if (from == "SSE CP26" && to == "Library Entrance") {
        output += "SSE CP26 -> Plaza -> SU Help Desk -> Library Entrance\n";
    }
    else if (to == "Food Court") {
        output += from + string(" -> Plaza -> Food Court\n");
    }
    else {
        output += from + string(" -> Plaza -> ") + to + "\n";
    }

    output += "\n" + GREEN + "Estimated Time: " + RESET + "5 minutes\n";
    return output;
}

// ----- Mock nearest feature -----
string findNearest(const string& from, const string& category) {
    string target;

    if (category == "food") target = FOOD[0];
    else if (category == "drinks") target = DRINKS[0];
    else if (category == "supermarket") target = SUPERMARKET[0];
    else if (category == "prayer") target = PRAYER[0];

    string out;
    out += CYAN + string("From: ") + RESET + from + "\n";
    out += CYAN + string("Nearest ") + category + ": " + RESET + target + "\n";
    out += BOLD + CYAN + "\nPath:\n" + RESET;

    out += from + string(" -> Plaza -> ") + target + "\n";
    out += GREEN + "Estimated time: " + RESET + "4 minutes\n";

    return out;
}

// ----- Clear Screen -----
void clearScreen() {
    // system("clear"); // Linux
    // system("CLS");   // Windows
}

// ==============================
//          MAIN PROGRAM
// ==============================
int main() {
    string currentLocation = "";

    // Startup
    while (true) {
        clearScreen();
        cout << CYAN << BOLD
             << "====================================\n"
             << "        AUC Campus Navigator        \n"
             << "====================================\n" 
             << RESET;

        cout << BLUE << "Where are you now? (nearest class/room/spot)\n> " << RESET;

        string input;
        getline(cin, input);

        vector<string> matches = findMatchingLocations(input);

        if (matches.empty()) {
            cout << RED << "No matching locations found. Try again.\n" << RESET;
            continue;
        }

        currentLocation = disambiguate(matches);
        break;
    }

    // Main Loop
    while (true) {
        clearScreen();

        cout << CYAN << BOLD
             << "====================================\n"
             << RESET;

        cout << BOLD << "Current Location: " 
             << YELLOW << currentLocation << RESET << "\n";

        cout << CYAN << BOLD
             << "====================================\n"
             << RESET;

        cout << GREEN
             << "1) Go To...\n"
             << "2) Change My Current Location\n"
             << "3) Where is the nearest...\n"
             << "4) Exit\n"
             << RESET << "> ";

        int choice;
        cin >> choice;
        cin.ignore();

        // Option 1: Go To
        if (choice == 1) {
            cout << BLUE << "Where would you like to go?\n> " << RESET;
            string dest;
            getline(cin, dest);

            vector<string> options = findMatchingLocations(dest);

            if (options.empty()) {
                cout << RED << "Destination not found.\n" << RESET;
                cin.get();
                continue;
            }

            string destination = disambiguate(options);

            cout << CYAN << BOLD << "\n----- Shortest Path -----\n" << RESET;
            cout << runDijkstra(currentLocation, destination);

            cout << GREEN << "Press Enter to continue..." << RESET;
            cin.get();
        }

        // Option 2
        else if (choice == 2) {
            cout << BLUE << "Enter your new location:\n> " << RESET;
            string newLoc;
            getline(cin, newLoc);

            vector<string> options = findMatchingLocations(newLoc);

            if (options.empty()) {
                cout << RED << "Location not found.\n" << RESET;
                cin.get();
                continue;
            }

            currentLocation = disambiguate(options);
        }

        // Option 3
        else if (choice == 3) {
            cout << GREEN
                 << "Find nearest:\n"
                 << "1) Food\n"
                 << "2) Drinks\n"
                 << "3) Supermarket\n"
                 << "4) Prayer Rooms\n"
                 << RESET << "> ";

            int cat;
            cin >> cat;
            cin.ignore();

            string category =
                cat == 1 ? "food" :
                cat == 2 ? "drinks" :
                cat == 3 ? "supermarket" :
                           "prayer";

            cout << CYAN << BOLD << "\n----- Nearest Location -----\n" << RESET;
            cout << findNearest(currentLocation, category);

            cout << GREEN << "Press Enter to continue..." << RESET;
            cin.get();
        }

        // Exit
        else if (choice == 4) {
            cout << GREEN << "Goodbye!\n" << RESET;
            break;
        }
    }

    return 0;
}
