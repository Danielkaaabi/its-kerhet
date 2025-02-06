#include <iostream>
#include <regex>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h> // Kräver OpenSSL-bibliotek

using namespace std;

unordered_map<string, string> users; // För lagring av användarnamn och lösenord i minnet

std::string md5(const std::string &str) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

bool isValidEmail(const string& email) {
    const regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    return regex_match(email, pattern);
}

bool isValidPassword(const string& password) {
    const regex pattern(R"((?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,})");
    return regex_match(password, pattern);
}

void loadUsersFromFile() {
    ifstream file("users.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        size_t delimiterPos = line.find(":");
        if (delimiterPos != string::npos) {
            string email = line.substr(0, delimiterPos);
            string hashPassword = line.substr(delimiterPos + 1);
            users[email] = hashPassword;
        }
    }
    file.close();
}

void saveUserToFile(const string& email, const string& hashPassword) {
    ofstream file("users.txt", ios::app);
    if (file) {
        file << email << ":" << hashPassword << "\n";
        file.close();
    }
}

void createUser() {
    string email, password;

    cout << "Ange e-postadress: ";
    cin >> email;

    if (!isValidEmail(email)) {
        cout << "Ogiltig e-postadress. Försök igen.\n";
        return;
    }

    if (users.find(email) != users.end()) {
        cout << "Användaren finns redan. Försök med en annan e-postadress.\n";
        return;
    }

    cout << "Ange lösenord: ";
    cin >> password;

    if (!isValidPassword(password)) {
        cout << "Lösenordet måste uppfylla följande regler:\n"
             << "- Minst 8 tecken\n"
             << "- Minst en stor bokstav\n"
             << "- Minst en liten bokstav\n"
             << "- Minst en siffra\n"
             << "- Minst ett specialtecken (@$!%*?&)\n";
        return;
    }

    string hashPassword = md5(password);
    users[email] = hashPassword;
    saveUserToFile(email, hashPassword);
    cout << "Användaren har skapats framgångsrikt!\n";
}

void testLogin() {
    string email, password;

    cout << "Ange e-postadress: ";
    cin >> email;

    ifstream file("users.txt");
    if (!file) {
        cout << "Fel: Kunde inte öppna användardatabasen.\n";
        return;
    }

    bool userFound = false;
    string line, storedEmail, storedHashPassword;
    while (getline(file, line)) {
        size_t delimiterPos = line.find(":");
        if (delimiterPos != string::npos) {
            storedEmail = line.substr(0, delimiterPos);
            storedHashPassword = line.substr(delimiterPos + 1);
            if (storedEmail == email) {
                userFound = true;
                break;
            }
        }
    }
    file.close();

    if (!userFound) {
        cout << "Fel: Ingen användare med denna e-postadress hittades.\n";
        return;
    }

    cout << "Ange lösenord: ";
    cin >> password;
    string hashPassword = md5(password);

    if (hashPassword == storedHashPassword) {
        cout << "OK Det gick att logga in.\n";
    } else {
        cout << "Fel: Lösenordet är felaktigt. Försök igen.\n";
    }
}

int main() {
    loadUsersFromFile(); // Ladda befintliga användare från fil

    int choice;

    do {
        cout << "\n--- MENY ---\n";
        cout << "1. Skapa användare\n";
        cout << "2. Test login\n";
        cout << "3. Avsluta\n";
        cout << "Välj ett alternativ: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createUser();
                break;
            case 2:
                testLogin();
                break;
            case 3:
                cout << "Avslutar programmet.\n";
                break;
            default:
                cout << "Ogiltigt val. Försök igen.\n";
        }
    } while (choice != 3);

    return 0;
}

