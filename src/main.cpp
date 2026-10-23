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
    if (!statsfile.is_open()) {
        std::cerr << "Failed to open stats.json. Please check that stats.json exists and wasn't moved." << std::endl;
        return 1;
    }
    json stats = json::parse(statsfile);
    std::ifstream openersfile("openers.json");
    if (!openersfile.is_open()) {
        std::cerr << "Failed to open openers.json. Please check that openers.json exists and wasn't moved." << std::endl;
        return 1;
    }
    json openers = json::parse(openersfile);
    std::ifstream modeusefile("modeuse.json");
    if (!modeusefile.is_open()) {
        std::cerr << "Failed to open modeuse.json. Please check that modeuse.json exists and wasn't moved." << std::endl;
        return 1;
    }
    json modeuse = json::parse(modeusefile);
    std::string answerFilename;
    std::string guessFilename;
    if (argc > 1 && std::string(argv[1]) == KEYWORDMODE) {
        modeuse["keyword"] = int(modeuse["keyword"]) + 1;
        answerFilename = "keywordlist.txt";
        guessFilename = "keywordlist.txt";
    } else if (argc > 1 && std::string(argv[1]) == STATS) {
        std::cout << "Winrate: " << stats["winrate"].get<int>() << "%" << std::endl;
        std::cout << "Games: " << stats["games"].get<int>() << std::endl;
        std::cout << "Wins: " << stats["wins"].get<int>() << std::endl;
        std::cout << "Favorite Opener: " << stats["favopener"].get<std::string>() << std::endl;
        std::cout << "Favorite Mode: " << stats["favmode"].get<std::string>() << std::endl;
        return 0;
    } else {
        modeuse["standard"] = int(modeuse["standard"]) + 1;
        answerFilename = "answerlist.txt";
        guessFilename = "wordlist.txt";
    }
    std::ifstream intro("intro.txt");
    if (!intro.is_open()) {
        std::cerr << "Failed to open intro.txt. Please check that intro.txt exists and wasn't moved." << std::endl;
    } else {
        std::string introduction((std::istreambuf_iterator<char>(intro)), std::istreambuf_iterator<char>());
        std::cout << introduction;
    }
    std::ifstream answerFile(answerFilename);
    if (!answerFile.is_open()) {
        std::cerr << "Failed to open " << answerFilename << ". Please check that " << answerFilename << " exists and wasn't moved." << std::endl;
        return 1;
    }
    std::ifstream guessFile(guessFilename);
    if (!guessFile.is_open()) {
        std::cerr << "Failed to open " << guessFilename << ". Please check that " << guessFilename << " exists and wasn't moved." << std::endl;
        return 1;
    }
    std::vector<std::string> answerlist = getlist(answerFile);
    std::vector<std::string> wordlist = getlist(guessFile);
    if (answerlist.empty() || wordlist.empty()) {
        std::cerr << "Answer list or guess list is empty. Please check that the selected lists contain the required words." << std::endl;
        return 1;
    }
    bool playing = true;
    std::string previousWord;
    while (playing) {
        std::string word = answerlist[randnum(answerlist.size())];
        while (answerlist.size() > 1 && word == previousWord) {
            word = answerlist[randnum(answerlist.size())];
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
                if (openers.contains(guess) && guessnum == 1) {
                    openers[guess] = int(openers[guess]) + 1;
                }
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
    auto favopen = std::max_element(openers.begin(), openers.end(), [](const json &a, const json &b) {return a.get<int>() < b.get<int>();});
    std::string opener = favopen.key();
    stats["favopener"] = opener;
    auto standard = modeuse.begin();
    auto keyword = std::next(standard);
    std::string favmode = (standard->get<int>() > keyword->get<int>()) ? standard.key() : keyword.key();
    stats["favmode"] = favmode;
    std::ofstream saveopeners("openers.json");
    if (!saveopeners.is_open()) {
        std::cerr << "Failed to save openers.json." << std::endl;
        return 1;
    }
    saveopeners << std::setw(4) << openers << std::endl;
    std::ofstream savemodeuse("modeuse.json");
    if (!savemodeuse.is_open()) {
        std::cerr << "Failed to save modeuse.json." << std::endl;
        return 1;
    }
    savemodeuse << std::setw(4) << modeuse << std::endl;
    std::ofstream savestats("stats.json");
    if (!savestats.is_open()) {
        std::cerr << "Failed to save stats.json." << std::endl;
        return 1;
    }
    savestats << std::setw(4) << stats << std::endl;
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
