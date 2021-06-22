#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>

struct Music {

    std::string title, album, author;

    friend std::ostream& operator<<(std::ostream &flux, const Music &m) {
        
        flux << m.title << " | " << m.album << " | " << m.author;
        return flux;
        
    }

    friend bool operator==(const Music &l, const Music &r) { return l.title == r.title && l.album == r.album && l.author == r.author; }
    friend bool operator!=(const Music &l, const Music &r) { return !(l==r); }

};

std::string ToUpper(const std::string &s) {

    std::string upper;
    for (char c : s) upper.push_back(toupper(c));
    return upper;

}

std::vector<std::string> Split(std::string s, char c) {

    std::vector<std::string> result;

    for (std::size_t i{0}; i < s.size(); i++) {

        if (s[i] == c) {

            std::string t{s};
            result.push_back(t.erase(i,t.size()-1));
            s.erase(s.begin(),s.begin()+i+1);
            i = 0;

        }

    }

    if (!s.empty()) result.push_back(s);

    for (std::size_t i{0}; i < result.size(); i++) {

        for (std::size_t j{0}; j < result[i].size(); j++) if (result[i][j] == c) result[i].erase(result[i].begin()+j);

        if (result[i].empty()) result.erase(result.begin()+i);

    }

    return result;

}

std::string AssemblyVectorString(const std::vector<std::string> &v, char c) {

    std::string assembly;
    for (const std::string &s : v) assembly+=s+c;
    assembly.pop_back();

    return assembly;

}

std::string SepareVoidString(std::string &s) {

    while (!s.empty() && s[0] == ' ') s.erase(0, 1);
    while (!s.empty() && s[s.size()-1] == ' ') s.pop_back();
    return s;

}

Music ConvertStringToMusic(const std::string &s) {

    std::vector<std::string> infos{Split(s, '|')};
    for (std::size_t i{0}; i < infos.size(); i++) {
                
        SepareVoidString(infos[i]);

        if (infos[i].empty()) switch (i) {

            case 0:
            infos[i] = "Unknow title";
            break;

            case 1:
            infos[i] = "Unknow album";
            break;

            case 2:
            infos[i] = "Unknow author";
            break;

            default:
            break;

        }

        if (i == 2) break;

    }

    if (infos.size() == 1) return Music{infos[0], "Unknow album", "Unknow author"};
    if (infos.size() == 2) return Music{infos[0], infos[1], "Unknow author"};
    if (infos.size() >= 3) return Music{infos[0], infos[1], infos[2]};

    return Music{"Unknow title", "Unknow album", "Unknow author"};

}

int main(int argc, char* argv[]) {

    if (argc > 1) for (unsigned int i{1}; i < (unsigned int)argc; i++) std::cout << argv[i] << std::endl;

    std::string path{argv[0]};
    while (!path.empty() && path[path.size()-1] != '\\' && path[path.size()-1] != '/') path.pop_back();

    std::vector<Music> discographie;
    auto AddMusic = [&discographie](const Music &add) -> void {

        if (std::find(discographie.begin(), discographie.end(), add) != discographie.end()) std::cout << "Error : Duplicate music\n"; else {

            discographie.push_back(add);

            std::sort(discographie.begin(), discographie.end(), [](const Music &l, const Music &r) -> bool {

                if (l.title == "Unknow title" && r.title == "Unknow title") return true;
                if (l.title.empty() || r.title.empty()) return l.title.size()<r.title.size();

                std::size_t index{0};
                while (true) {

                    if (l.title[index]==r.title[index]) index++; else return l.title[index]<r.title[index];
                    if (index > l.title.size() || index > r.title.size()) return l.title.size()<r.title.size();

                }

            });

        }

    };

    std::cout << "Use \"help\" for help\n";

    while (true) {

        std::string command{""};
        while (command.empty()) {

            std::cout << "> ";
            std::getline(std::cin, command);
            if (command.empty()) std::cout << "Error : Unknow command !\n";

        }

        std::vector<std::string> args{Split(command, ' ')};

        if (ToUpper(args[0]) == "QUIT") break;
        else if (ToUpper(args[0]) == "ADD") AddMusic(ConvertStringToMusic(command.erase(0, args[0].size())));

        else if (ToUpper(args[0]) == "SAVE") {

            if (args.size() > 1) {

                std::ofstream save{path + args[1] + ".music"};
                if (save.fail()) std::cout << "Error : Can't open this file\n"; else {

                    for (const Music &m : discographie) {
                
                        // if (save.eof()) { std::cout << "Error : File close\n"; break; }
                        save << m;
                        if (m != discographie[discographie.size()-1]) save << std::endl;

                    }

                    if (!save.eof()) {

                        save.close();
                        std::cout << "File saved with sucess !\n";

                    }

                }

            } else std::cout << "Error : Not filename\n";


        }

        else if (ToUpper(args[0]) == "LOAD") {

            discographie.clear();

            if (args.size() > 1) {

                std::ifstream load{path + args[1] + ".music"};
                if (!load.good()) std::cout << "Error : Not file exist : " << path + args[1] + ".music" << "\n";
                else if (load.fail()) std::cout << "Error : Can't open this file\n"; else {

                    std::string line;
                    while (std::getline(load, line)) {

                        // if (load.eof()) { std::cout << "Error : File close\n"; break; }
                        AddMusic(ConvertStringToMusic(line));

                    }

                }

                load.close();

            } else std::cout << "Error : Not filename\n";

        }

        else if (ToUpper(args[0]) == "PRINT") {

            if (args.size() > 1) {

                if (discographie.empty()) std::cout << "Error : No musics\n"; else {

                    if (ToUpper(args[1]) == "TITLES") for (const Music &m : discographie) std::cout << "--> " << m << std::endl;
                    else if (ToUpper(args[1]) == "ALBUMS") std::cout << "Non implemente...\n";
                    else if (ToUpper(args[1]) == "AUTHORS") std::cout << "Non implemente...\n";
                    else std::cout << "Error : Not mode select : (title, album or author)\n";

                }

            } else std::cout << "Error : Not mode select : (title, album or author)\n";

        }

        else if (ToUpper(args[0]) == "HELP") std::cout <<
        "\nQuit : Quit app" <<
        "\nAdd <title> | <album> | <author> : add new music" <<
        "\nSave <filename> : Save musics to <filename>.music" <<
        "\nLoad <filename> : Load musics from <filename>.music" <<
        "\nPrint <mode> : Print musics" <<
        "\nHelp : Seriously ?!\n\n";

        else std::cout << "Error : Unknow command !\n";

    }

    // Ferme tout les flux

    return 0;

}

// Command : g++ -m32 main.cpp -o Discographie -s -std=c++17 -Wall -Wextra -pedantic-errors