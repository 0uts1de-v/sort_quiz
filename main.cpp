#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <sqlite3.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;

vector<pair<string, string>> dict;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   for (int i = 0; i < argc; i += 2) {
      dict.push_back(make_pair(argv[i], argv[i + 1]));
   }
   return SQLITE_OK;
}

void read_dict() {
    sqlite3 *db;
    const char *db_name = "ejdict.sqlite3";

    if (sqlite3_open_v2(db_name, &db, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        cerr << "DB open error." <<endl;
        exit(1);
    }
    cout << "Connected to the DB." << endl;
    cout << "Loading data from DB..." << flush;

    sqlite3_exec(db, "select word, mean from items;", callback, nullptr, nullptr);

    cout << "Done." << endl;

    if (sqlite3_close_v2(db) != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        cerr << "DB close error." << endl;
        exit(1);
    }
}

void quiz(int wlen) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<> dist(0, dict.size() - 1);

    int i = dist(mt);
    string word = dict.at(i).first;
    while (word.length() > wlen) {
        i = dist(mt);
        word = dict.at(i).first;
    }

    string mean = dict.at(i).second;
    string shuffled_word = word;
    sort(shuffled_word.begin(), shuffled_word.end());

    string iword;
    cout << "\"" << shuffled_word << "\"" << endl;
    for (int j = 0; j < 5; ++j) {
        cout << 5 - j << "> " << flush;
        cin >> iword;
        if (iword == "?") break;
        if (iword == word) {
            cout << "Correct!" << endl;
            break;
        }
        cout << "Wrong." << endl;
    }

    cout << word << ": " << mean << endl;
}

int main() {
    read_dict();

    int wlen;
    cout << "Max length: " << flush;
    cin >> wlen;
    while (wlen < 2) {
        cout << "Too short!\n>> " << flush;
        cin >> wlen;
    }

    bool flag = true;
    char ch;
    while (flag) {
        quiz(wlen);
        cout << "Continue?(Y/n)" << flush;
        cin.ignore();
        cin.get(ch);
        if (ch == 'n') {
            cout << "Bye~" << endl;
            break;
        }
    }
}
