#include <fstream>
#include <iostream>
#include "Leaderboard.h"

void Leaderboard::read_from_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    string playerName;
    time_t date;
    unsigned long score;
    while (file >> score >> date >> playerName) {
        LeaderboardEntry* newEntry = new LeaderboardEntry(score, date, playerName);
        insert_new_entry(newEntry);
    }

    file.close();
}

void Leaderboard::write_to_file(const string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    LeaderboardEntry* currentEntry = head_leaderboard_entry;
    while (currentEntry != nullptr) {
        file << currentEntry->score << " " << currentEntry->last_played << " " << currentEntry->player_name << endl;
        currentEntry = currentEntry->next_leaderboard_entry;
    }

    file.close();
}

void Leaderboard::print_leaderboard() {
    cout << "Leaderboard:" << endl;
    cout << "-----------" << endl;

    int rank = 1;
    LeaderboardEntry* temp = head_leaderboard_entry;

    while (temp != nullptr) {
        cout << rank << ". " << temp->player_name << " " << temp->score << " ";

        // Format the last_played time
        struct tm* timeInfo;
        char buffer[80]; // For storing the formatted time
        timeInfo = localtime(&temp->last_played);
        strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", timeInfo);
        cout << buffer << endl;

        temp = temp->next_leaderboard_entry;
        rank++;
    }
}

void Leaderboard::insert_new_entry(LeaderboardEntry* new_entry) {
    if (head_leaderboard_entry == nullptr) {
        head_leaderboard_entry = new_entry;
        return;
    }

    LeaderboardEntry* prevEntry = nullptr;
    LeaderboardEntry* tempEntry = head_leaderboard_entry;

    // Traverse the current entries to find the position to insert the new entry
    while (tempEntry != nullptr && tempEntry->score >= new_entry->score) {
        prevEntry = tempEntry;
        tempEntry = tempEntry->next_leaderboard_entry;
    }

    if (prevEntry == nullptr) {
        // If the new entry's score is the highest, insert it at the beginning
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        // Insert the new entry in the correct position
        prevEntry->next_leaderboard_entry = new_entry;
        new_entry->next_leaderboard_entry = tempEntry;
    }

    // Keep only the top MAX_LEADERBOARD_SIZE (10) entries
    int entryCount = 0;
    tempEntry = head_leaderboard_entry;
    while (entryCount < 10 - 1 && tempEntry->next_leaderboard_entry != nullptr) {
        tempEntry = tempEntry->next_leaderboard_entry;
        ++entryCount;
    }

    // Remove entries beyond the top MAX_LEADERBOARD_SIZE = 10
    if (entryCount >= 10 - 1) {
        LeaderboardEntry* nextEntry = tempEntry->next_leaderboard_entry;
        tempEntry->next_leaderboard_entry = nullptr;

        // Free memory for the removed entries
        while (nextEntry != nullptr) {
            LeaderboardEntry* toDelete = nextEntry;
            nextEntry = nextEntry->next_leaderboard_entry;
            delete toDelete;
        }
    }
}


Leaderboard::~Leaderboard() {
    LeaderboardEntry* currentEntry = head_leaderboard_entry;
    while (currentEntry != nullptr) {
        LeaderboardEntry* temp = currentEntry;
        currentEntry = currentEntry->next_leaderboard_entry;
        delete temp;
    }
}


