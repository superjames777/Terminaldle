#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <array>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#ifdef _WIN32
#include <windows.h>
#endif

#define KEYWORDMODE "-kwm"
#define STATS "-stats"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

using json = nlohmann::json;

static std::size_t randnum(const std::size_t max);
std::vector<std::string> getlist(std::ifstream &wordlist);

int main(int argc, char *argv[]) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif
    std::ifstream statsfile("stats.json");
    json stats = json::parse(statsfile);
    std::string filename;
    if (argc > 1 && std::string(argv[1]) == KEYWORDMODE) {
        filename = "keywordlist.txt";
    } else if (argc > 1 && std::string(argv[1]) == STATS) {
        std::cout << "Winrate: " << stats["winrate"].get<int>() << "%" << std::endl;
        std::cout << "Games: " << stats["games"].get<int>() << std::endl;
        std::cout << "Wins: " << stats["wins"].get<int>() << std::endl;
        std::cout << "Favorite Opener: " << stats["favopener"].get<std::string>() << std::endl;
        std::cout << "Favorite Mode: " << stats["favmode"].get<std::string>() << std::endl;
        return 0;
    } else {
        filename = "wordlist.txt";
    }
    std::ifstream intro("intro.txt");
    if (!intro.is_open()) {
        std::cerr << "Failed to open intro.txt. Please check that intro.txt exists and wasn't moved." << std::endl;
        return 1;
    }
    std::string introduction((std::istreambuf_iterator<char>(intro)), std::istreambuf_iterator<char>());
    std::cout << introduction;
    std::ifstream wlist(filename);
    if (!wlist.is_open()) {
        std::cerr << "Failed to open word list. Please check that wordlist.txt & keywordlist.txt still exist and have not been moved." << std::endl;
        return 1;
    }
    std::vector<std::string> wordlist = getlist(wlist);
    if (wordlist.empty()) {
        std::cerr << "Word list is empty. Please check that the word list you are attempting to use contains the required words." << std::endl;
        return 1;
    }
    bool playing = true;
    std::string previousWord;
    while (playing) {
        std::string word = wordlist[randnum(wordlist.size())];
        while (wordlist.size() > 1 && word == previousWord) {
            word = wordlist[randnum(wordlist.size())];
        }
        previousWord = word;
        int guessnum = 1;
        bool guessed = false;
        while (guessnum < 7) {
            std::cout << "\nGuess: " << guessnum << std::endl;
            std::string guess;
            std::cin >> guess;
            for (char &c : guess) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            }
            if (guess.length() != 5) {
                std::cout << "Guess must be 5 letters.\n";
                continue;
            } else if (std::find(wordlist.begin(), wordlist.end(), guess) == wordlist.end()) {
                std::cout << "Not a valid word.\n";
                continue;
            } else {
                if (guess == word) {
                    for (std::size_t i = 0; i < word.length(); i++) {
                        std::cout << GREEN << guess[i] << RESET;
                        std::cout << std::flush;
                        if (i + 1 < word.length()) {
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                    }
                    std::cout << "\nCorrect!\nThe word was: " << word << "\nGuesses taken: " << guessnum << std::endl;
                    guessed = true;
                    break;
                }
                std::vector<bool> green(word.length(), false);
                std::array<std::size_t, 256> remaining{};
                for (std::size_t i = 0; i < word.length(); i++) {
                    if (guess[i] == word[i]) {
                        green[i] = true;
                    } else {
                        remaining[static_cast<unsigned char>(word[i])]++;
                    }
                }
                for (std::size_t i = 0; i < word.length(); i++) {
                    if (green[i]) {
                        std::cout << GREEN << guess[i] << RESET;
                    } else {
                        std::size_t &count = remaining[static_cast<unsigned char>(guess[i])];
                        if (count > 0) {
                            std::cout << YELLOW << guess[i] << RESET;
                            count--;
                        } else {
                            std::cout << guess[i];
                        }
                    }
                    std::cout << std::flush;
                    if (i + 1 < word.length()) {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
                std::cout << '\n';
            }
            guessnum++;
        }
        stats["games"] = int(stats["games"]) + 1;
        if (!guessed) {
            std::cout << "\nThe answer was: " << word << std::endl;
        } else {
            stats["wins"] = int(stats["wins"]) + 1;
        }
        stats["winrate"] = double(double(stats["wins"]) / double(stats["games"])) * 100;
        std::string result;
        std::cout << "Would you like play another round? (y/n): ";
        std::cin >> result;
        if (result == "y" || result == "Y") {
            continue;
        } else if (result == "n" || result == "N") {
            playing = false;
            break;
        } else {
            bool answered = false;
            while (!answered) {
                std::string redoresult;
                std::cout << "\nInvalid input please enter y or n: ";
                std::cin >> redoresult;
                if (redoresult == "y" || redoresult == "Y") {
                    answered = true;
                } else if (redoresult == "n" || redoresult == "N") {
                    answered = true;
                    playing = false;
                    break;
                }
            }  
        }
    }
    std::ofstream out("stats.json");
    out << std::setw(4) << stats << std::endl;
    return 0;
}

static std::size_t randnum(const std::size_t max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<std::size_t> distrib(0, max - 1);
    return distrib(gen);
}

std::vector<std::string> getlist(std::ifstream &wordlist) {
    std::vector<std::string> list;
    std::string line;
    while (std::getline(wordlist, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            list.push_back(line);
        }
    }
    return list;
}
