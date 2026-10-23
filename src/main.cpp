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
#ifdef _WIN32
#include <windows.h>
#endif

#define KEYWORDMODE "-kwm"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

static std::size_t randnum(const std::size_t max);
std::vector<std::string> getlist(std::ifstream &wordlist);

int main(int argc, char *argv[]) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif
    std::ifstream intro("intro.txt");
    if (!intro.is_open()) {
        std::cerr << "Failed to open intro.txt. Please check that intro.txt exists and wasn't moved." << std::endl;
        return 1;
    }
    std::string introduction((std::istreambuf_iterator<char>(intro)), std::istreambuf_iterator<char>());
    std::cout << introduction;
    std::string filename;
    if (argc > 1 && std::string(argv[1]) == KEYWORDMODE) {
        filename = "keywordlist.txt";
    } else {
        filename = "wordlist.txt";
    }
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
    std::string word = wordlist[randnum(wordlist.size())];
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
    if (!guessed) {
        std::cout << "\nThe answer was: " << word << std::endl;
    }
    return 0;
}

static std::size_t randnum(const std::size_t max) {
    std::random_device randev;
    std::mt19937 gen(randev());
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
