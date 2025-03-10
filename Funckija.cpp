#include <iostream>
#include <fstream>
#include <windows.h>
#include "json.hpp"


using json = nlohmann::json;
using namespace std;
json loadJSON(const string& filename) {
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Nevar atvērt failu!");
        }
        json data = json::parse(file);
        return data;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return json();
    }
}

void displayItem(const json& item) {
    cout << "Kategorija: " << item["Kategorija"] << endl;
    cout << "Nosaukums: " << item["Nosaukums"] << endl;
    cout << "Zīmols: " << item["Zīmols"] << endl;
    cout << "IzlaišanasGads: " << item["IzlaišanasGads"] << endl;
    cout << "ReģistrēšanasDatums: " << item["ReģistrēšanasDatums"] << endl;
    cout << "Noliktava: " << item["Noliktava"] << endl;
    
    // Perifērijas
    if (item.contains("Svars")) {
        cout << "Svars: " << item["Svars"] << endl;
        cout << "Savienojums: " << item["Savienojums"] << endl;
    }
    
    cout << "Informācija: " << item["Informācija"] << endl;
    cout << "------------------------" << endl;
}

void setUTF8Console() { // Latvijas alfabētam un simboliem
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

bool containsIgnoreCase(const string& str, const string& substring) {
    string strLower = str;
    string subLower = substring;
    // Pārveido abas virknes uz mazajiem burtiem, lai veiktu reģistrjutīgu salīdzināšanu
    transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
    transform(subLower.begin(), subLower.end(), subLower.begin(), ::tolower);
    return strLower.find(subLower) != string::npos;
}
void searchItems(const json& data, 
                const int kategorija,
                const string& apakskategorija, 
                const string& zimols, 
                const string& nosaukums, 
                const int noliktava) {
    
    bool foundItems = false;
    int totalItems = 0; 

    if (kategorija == 0) { // Meklē visās kategorijās
        // Perifēriju meklēšana
        if (data.contains("Perifērijas")) {
            for (auto it = data["Perifērijas"].begin(); it != data["Perifērijas"].end(); ++it) {
                const json& item = it.value();
                
                bool matches = true;
                if (!apakskategorija.empty() && !containsIgnoreCase(item["Kategorija"], apakskategorija)) matches = false;
                if (!zimols.empty() && !containsIgnoreCase(item["Zīmols"], zimols)) matches = false;
                if (!nosaukums.empty() && !containsIgnoreCase(item["Nosaukums"], nosaukums)) matches = false;
                if (noliktava != -1 && item["Noliktava"] != noliktava) matches = false;
                
                if (matches) {
                    foundItems = true;
                    displayItem(item);
                    totalItems++; 
                }
            }
        }
        
        // Komponentu meklēšana
        if (data.contains("Komponentes")) {
            for (auto it = data["Komponentes"].begin(); it != data["Komponentes"].end(); ++it) {
                const json& item = it.value();
                
                bool matches = true;
                if (!apakskategorija.empty() && !containsIgnoreCase(item["Kategorija"], apakskategorija)) matches = false;
                if (!zimols.empty() && !containsIgnoreCase(item["Zīmols"], zimols)) matches = false;
                if (!nosaukums.empty() && !containsIgnoreCase(item["Nosaukums"], nosaukums)) matches = false;
                if (noliktava != -1 && item["Noliktava"] != noliktava) matches = false;
                
                if (matches) {
                    foundItems = true;
                    displayItem(item);
                    totalItems++;
                }
            }
        }
        
        // Mēbeļu meklēšana
        if (data.contains("Mēbeles")) {
            for (auto it = data["Mēbeles"].begin(); it != data["Mēbeles"].end(); ++it) {
                const json& item = it.value();
                
                bool matches = true;
                if (!apakskategorija.empty() && !containsIgnoreCase(item["Kategorija"], apakskategorija)) matches = false;
                if (!zimols.empty() && !containsIgnoreCase(item["Zīmols"], zimols)) matches = false;
                if (!nosaukums.empty() && !containsIgnoreCase(item["Nosaukums"], nosaukums)) matches = false;
                if (noliktava != -1 && item["Noliktava"] != noliktava) matches = false;
                
                if (matches) {
                    foundItems = true;
                    displayItem(item);
                    totalItems++;
                }
            }
        }
    } else { // Ja ir norādīta kategorija, tad meklē tikai tajā kategorijā
        const json* searchCategory = nullptr;
        if (kategorija == 1) {
            searchCategory = &data["Perifērijas"];
        } else if (kategorija == 3) {
            searchCategory = &data["Komponentes"];
        } else if (kategorija == 2) {
            searchCategory = &data["Mēbeles"];
        }

        if (!searchCategory) {
            cout << "Kategorija nav atrasta!" << endl;
            return;
        }

        // Meklēšana izvēlētajā kategorijā
        for (auto it = searchCategory->begin(); it != searchCategory->end(); ++it) {
            const json& item = it.value();
            bool matches = true;
            if (!apakskategorija.empty() && !containsIgnoreCase(item["Kategorija"], apakskategorija)) matches = false;
            if (!zimols.empty() && !containsIgnoreCase(item["Zīmols"], zimols)) matches = false;
            if (!nosaukums.empty() && !containsIgnoreCase(item["Nosaukums"], nosaukums)) matches = false;
            if (noliktava != -1 && item["Noliktava"] != noliktava) matches = false;
            
            if (matches) {
                foundItems = true;
                displayItem(item);
                totalItems++; 
            }
        }
    }
    
    if (!foundItems) {
        cout << "Nav atrasts neviens priekšmets ar norādītajiem kritērijiem." << endl;
    } else {
        cout << "\nKopā atrasti " << totalItems << " priekšmeti." << endl;
    }
}

// Pārveido lietotāja ievadīto kategoriju uz int
int getKategorija(const string& choice) {
    if (choice.empty()) return 0;  // 0 nozīmē visas kategorijas
    try {
        int num = stoi(choice);
        if (num >= 0 && num <= 3) {
            return num;  // Atgriež skaitli tieši
        }
        return 0;  // Noklusējuma gadījumā visas kategorijas
    } catch (...) {
        return 0;  // Noklusējuma gadījumā visas kategorijas
    }
}
// Pārveido lietotāja ievadīto noliktavu uz skaitli
int getNoliktava(const string& input) {
    if (input.empty()) return -1;
    try {
        int num = stoi(input);
        if (num == 1 || num == 2 || num == -1) {
            return num;
        }
        return -1;
    } catch (...) {
        return -1;
    }
}

int main() {
    setUTF8Console();
    json data = loadJSON("items.json");
    
    while(true) {
        cout << "\n=== Meklēšanas Sistēma ===" << endl;
        cout << "Izvēlieties Kategoriju (1 - Perifērija, 2 - Mēbele, 3 - Komponente, Enter - Visas, 0 - Iziet): ";
        
        string kategorijaInput;
        getline(cin, kategorijaInput);

        if (kategorijaInput == "0") {
            cout << "Programma tiek aizvērta..." << endl;
            break;
        }

        int kategorija = getKategorija(kategorijaInput);
        
        cout << "Ievadiet apakškategoriju vai atstājiet tukšu: ";
        string apakskategorija;
        getline(cin, apakskategorija);
        
        cout << "Ievadiet zīmolu vai atstājiet tukšu: ";
        string zimols;
        getline(cin, zimols);
        
        cout << "Ievadiet nosaukumu vai atstājiet tukšu: ";
        string nosaukums;
        getline(cin, nosaukums);
        
        cout << "Izvēlieties noliktavu (1, 2, vai Enter visām): ";
        string noliktavaInput;
        getline(cin, noliktavaInput);
        int noliktava = getNoliktava(noliktavaInput);
        
        cout << "\nMeklēšanas rezultāti:" << endl;
        cout << "------------------------" << endl;
        
        searchItems(data, kategorija, apakskategorija, zimols, nosaukums, noliktava);
        
        cout << "\nVai vēlaties meklēt vēlreiz? (j/n): ";
        string again;
        getline(cin, again);
        if (again != "j" && again != "J") {
            cout << "Programma tiek aizvērta..." << endl;
            break;
        }
    }
    
    return 0;
}