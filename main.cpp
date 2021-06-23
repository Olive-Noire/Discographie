#include <unordered_map>
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

    // friend std::istream& operator>>(std::istream& flux, const Music &m) {}

    friend bool operator==(const Music &l, const Music &r) { return l.title == r.title && l.album == r.album && l.author == r.author; }
    friend bool operator!=(const Music &l, const Music &r) { return !(l==r); }

};

struct Lang {

    std::vector<std::string> commands, descriptions, errors, tags, notifs;

    Lang& Load(const std::string &lang) {

        std::ifstream load{"Languages\\" + lang + ".lang"};
        if (!load.good() || load.fail()) throw std::logic_error{"Error : Can't load lang : " + lang}; else {

            commands.clear();
            descriptions.clear();
            errors.clear();
            tags.clear();
            notifs.clear();

            std::string line;
            std::size_t index{0};
            while (std::getline(load, line)) {

                if (line.empty()) index++; else switch (index) {

                    case 0: commands.push_back(line); break;
                    case 1: descriptions.push_back(line); break;
                    case 2: errors.push_back(line); break;
                    case 3: tags.push_back(line); break;
                    case 4: notifs.push_back(line); break;

                }

            }

        }

        return *this;

    }

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

bool AlphabeticPlace(const std::string &l, const std::string &r) {

    if (l == r) return true;

    std::size_t index{0};
    while (true) {

        if (!isalpha(l[index]) && !isalpha(r[index])) return l[index]<r[index];
        else if (!isalpha(l[index])) return false; else if (!isalpha(r[index])) return true;
        
        if (l[index]==r[index]) index++; else return l[index]<r[index];
        if (index >= l.size() || index >= r.size()) return l.size()<r.size();

    }

}

int main(int argc, char* argv[]) {

    if (argc > 1) for (unsigned int i{1}; i < (unsigned int)argc; i++) std::cout << argv[i] << std::endl;

    std::string path{argv[0]};
    while (!path.empty() && path[path.size()-1] != '\\' && path[path.size()-1] != '/') path.pop_back();

    Lang lang;
    lang.Load("fr-FR");

    std::vector<Music> discographie;
    auto AddMusic = [&discographie, &lang](const Music &add) -> void {

        if (std::find(discographie.begin(), discographie.end(), add) != discographie.end()) std::cout << lang.errors[1] << std::endl; else {

            discographie.push_back(add);

            std::sort(discographie.begin(), discographie.end(), [&lang](const Music &l, const Music &r) -> bool { return AlphabeticPlace(l.title, r.title); });

        }

    };

    auto ConvertStringToMusic = [&lang](const std::string &s) -> Music {

        std::vector<std::string> infos{Split(s, '|')};
        for (std::size_t i{0}; i < infos.size(); i++) {
                
        SepareVoidString(infos[i]);

            if (infos[i].empty()) switch (i) {

                case 0: infos[i] = lang.tags[0]; break;
                case 1:infos[i] = lang.tags[1]; break;
                case 2: infos[i] = lang.tags[2]; break;
                default: break;

            }

            if (i == 2) break;

        }

        if (infos.size() == 1) return Music{infos[0], lang.tags[4], lang.tags[5]};
        if (infos.size() == 2) return Music{infos[0], infos[1], lang.tags[5]};
        if (infos.size() >= 3) return Music{infos[0], infos[1], infos[2]};

        return Music{lang.tags[3], lang.tags[4], lang.tags[5]};

    };

    std::cout << lang.notifs[0] << std::endl;

    while (true) {

        std::string command{""};
        while (command.empty()) {

            std::cout << "> ";
            std::getline(std::cin, command);

            if (std::cin.fail()) { std::cin.ignore(std::numeric_limits<std::streamsize>::max()); std::cin.clear(); }
            if (std::cin.bad() || std::cin.eof() || command.empty()) std::cout << lang.errors[0] << std::endl;

        }

        std::vector<std::string> args{Split(SepareVoidString(command), ' ')};

        if (ToUpper(args[0]) == lang.commands[0]) break;
        else if (ToUpper(args[0]) == lang.commands[1]) AddMusic(ConvertStringToMusic(command.erase(0, args[0].size())));

        else if (ToUpper(args[0]) == lang.commands[2]) {

            if (args.size() > 1) {

                std::ofstream save{path + command.erase(0, args[0].size()+1) + ".music"};
                if (save.fail() || !save) std::cout << lang.errors[4] << std::endl; else {

                    for (const Music &m : discographie) {
                
                        save << m;
                        if (m != discographie[discographie.size()-1]) save << std::endl;
                        if (save.eof()) { std::cout << lang.errors[5] << std::endl; break; }

                    }

                    if (!save.eof()) {

                        save.close();
                        std::cout << lang.notifs[1] << " : " + path + command.erase(0, args[0].size()+1) + ".music\n";

                    }

                }

            } else std::cout << lang.errors[2] << std::endl;


        }

        else if (ToUpper(args[0]) == lang.commands[3]) {

            discographie.clear();

            if (args.size() > 1) {

                std::ifstream load{path + command.erase(0, args[0].size()+1) + ".music"};
                if (!load.good()) std::cout << lang.errors[3] << " : " + path + command.erase(0, args[0].size()+1) + ".music\n";
                else if (load.fail() || !load) std::cout << lang.errors[4] << std::endl; else {

                    std::string line;
                    while (std::getline(load, line)) AddMusic(ConvertStringToMusic(line));

                }

                load.close();

            } else std::cout << lang.errors[2] << std::endl;

        }

        else if (ToUpper(args[0]) == lang.commands[4]) {

            if (args.size() > 1) {

                if (discographie.empty()) std::cout << lang.errors[6] << std::endl; else {

                    if (ToUpper(args[1]) == lang.tags[0]) for (const Music &m : discographie) std::cout << "--> " << m << std::endl;

                    else if (ToUpper(args[1]) == lang.tags[1]) {

                        std::unordered_map<std::string, std::vector<Music>> albums;
                        std::vector<std::string> alphabeticNames;

                        for (const Music &m : discographie) {
                            
                            if (albums.find(m.album) == albums.end()) alphabeticNames.push_back(m.album);
                            albums[m.album].push_back(m);

                        }

                        std::sort(alphabeticNames.begin(), alphabeticNames.end(), AlphabeticPlace);

                        for (const std::string &s : alphabeticNames) {

                            std::cout << "--> " << s << " | " << albums[s][0].author << std::endl;
                            for (const Music &m : albums[s]) std::cout << "\t/--> " << m.title << std::endl;

                        }

                    }

                    else if (ToUpper(args[1]) == lang.tags[2]) {

                        std::unordered_map<std::string, std::vector<Music>> authors;
                        std::vector<std::string> alphabeticNamesAuthors;

                        for (const Music &m : discographie) {
                            
                            if (authors.find(m.author) == authors.end()) alphabeticNamesAuthors.push_back(m.author);
                            authors[m.author].push_back(m);

                        }

                        std::sort(alphabeticNamesAuthors.begin(), alphabeticNamesAuthors.end(), AlphabeticPlace);

                        for (const std::string &s : alphabeticNamesAuthors) {

                            std::cout << "--> " << s << std::endl;

                            std::unordered_map<std::string, std::vector<Music>> albums;
                            std::vector<std::string> alphabeticNamesAlbums;
                            
                            for (const Music &m : discographie) {

                                if (m.author == s) {

                                    if (albums.find(m.album) == albums.end()) alphabeticNamesAlbums.push_back(m.album);
                                    albums[m.album].push_back(m);

                                }

                            }

                            std::sort(alphabeticNamesAlbums.begin(), alphabeticNamesAlbums.end(), AlphabeticPlace);

                            for (const std::string &a : alphabeticNamesAlbums) {

                                std::cout << "\t/--> " << a << std::endl;
                                for (const Music &m : albums[a]) std::cout << "\t\t/--> " << m.title << std::endl;

                            }

                        }

                    }

                    else std::cout << lang.errors[7] << std::endl;

                }

            } else std::cout << lang.errors[7] << std::endl;

        }

        else if (ToUpper(args[0]) == lang.commands[5]) {

            if (args.size() > 1 && std::ifstream{"Languages\\" + args[1] + ".lang"}.good()) {
                
                lang.Load(args[1]); 
                std::cout << lang.notifs[2] << " : Languages\\" + args[1] + ".lang\n";
                
            } else std::cout << lang.errors[8] << " : Languages\\" + args[1] + ".lang\n";

        }

        else if (ToUpper(args[0]) == lang.commands[lang.commands.size()-1]) {
            
            std::cout << std::endl;
            for (const std::string &s : lang.descriptions) std::cout << s << std::endl;
            std::cout << std::endl;

        }

        else std::cout << lang.errors[0] << std::endl;

    }

    return 0;

}

// Command : g++ -m32 main.cpp -o Discographie -s -std=c++17 -Wall -Wextra -pedantic-errors

/*

CORRECTION:

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

std::string traitement_chaine(std::string const & chaine)
{
    std::string copie{ chaine };

    // D'abord on enlève les espaces au début...
    auto premier_non_espace{ std::find_if_not(std::begin(copie), std::end(copie), isspace) };
    copie.erase(std::begin(copie), premier_non_espace);

    // ...puis à la fin.
    std::reverse(std::begin(copie), std::end(copie));
    premier_non_espace = std::find_if_not(std::begin(copie), std::end(copie), isspace);
    copie.erase(std::begin(copie), premier_non_espace);
    std::reverse(std::begin(copie), std::end(copie));

    return copie;
}

struct Artiste
{
    std::string nom;
};

// On surcharge l'opérateur << pour être capable d'afficher le type Artiste.
std::ostream & operator<<(std::ostream & sortie, Artiste const & artiste)
{
    sortie << artiste.nom;
    return sortie;
}

struct Album
{
    std::string nom;
};

// On surcharge l'opérateur << pour être capable d'afficher le type Album.
std::ostream & operator<<(std::ostream & sortie, Album const & album)
{
    sortie << album.nom;
    return sortie;
}

struct Morceau
{
    std::string nom;
    Artiste compositeur;
    Album album;
};

// On surcharge l'opérateur << pour être capable d'afficher le type Morceau.
std::ostream & operator<<(std::ostream & sortie, Morceau const & morceau)
{
    sortie << morceau.nom << " | " << morceau.album << " | " << morceau.compositeur;
    return sortie;
}

// Conversion d'un flux d'entrée (std::cin, chaîne, fichier) en type Morceau.
std::istream & operator>>(std::istream & entree, Morceau & morceau)
{
    std::string mot{};
    std::ostringstream flux{};

    // Récupération du nom du morceau.
    while (entree >> mot && mot != "|")
    {
        flux << mot << " ";
    }

    std::string nom_morceau{ flux.str() };
    if (std::empty(nom_morceau))
    {
        nom_morceau = "Morceau inconnu";
    }
    morceau.nom = traitement_chaine(nom_morceau);
    flux.str(std::string{});

    // Récupération du nom de l'album.
    while (entree >> mot && mot != "|")
    {
        flux << mot << " ";
    }

    std::string nom_album{ flux.str() };
    if (std::empty(nom_album))
    {
        nom_album = "Album inconnu";
    }
    morceau.album.nom = traitement_chaine(nom_album);
    flux.str(std::string{});

    // Récupération du nom de l'artiste.
    while (entree >> mot)
    {
        flux << mot << " ";
    }

    std::string nom_artiste{ flux.str() };
    if (std::empty(nom_artiste))
    {
        nom_artiste = "Artiste inconnu";
    }
    morceau.compositeur.nom = traitement_chaine(nom_artiste);
    flux.str(std::string{});

    return entree;
}

// Test qu'une entrée complète est valide.
void test_creation_morceau_entree_complete()
{
    std::istringstream entree{ "Frica | Frica | Carla's Dreams" };
    Morceau morceau{};

    entree >> morceau;

    assert(morceau.nom == "Frica" && u8"Le nom du morceau doit être Frica.");
    assert(morceau.album.nom == "Frica" && u8"Le nom de l'album doit être Frica.");
    assert(morceau.compositeur.nom == "Carla's Dreams" && u8"Le nom du compositeur doit être Carla's Dreams.");
}

// Test d'une entrée complète avec beaucoup d'espaces.
void test_creation_morceau_entree_espaces_partout()
{
    std::istringstream entree{ "Levels       |  Levels   |     Avicii" };
    Morceau morceau{};

    entree >> morceau;

    assert(morceau.nom == "Levels" && u8"Le nom du morceau doit être Levels.");
    assert(morceau.album.nom == "Levels" && u8"Le nom de l'album doit être Levels.");
    assert(morceau.compositeur.nom == "Avicii" && u8"Le nom du compositeur doit être Avicii.");
}

// Test d'une entrée avec seulement le nom de la chanson est valide.
void test_creation_morceau_entree_chanson_artiste()
{
    std::istringstream entree{ "Subeme la radio | | Enrique Iglesias" };
    Morceau morceau{};

    entree >> morceau;

    assert(morceau.nom == "Subeme la radio" && u8"Le nom du morceau doit être Subeme la radio.");
    assert(morceau.album.nom == "Album inconnu" && u8"Le nom de l'album doit être Album inconnu.");
    assert(morceau.compositeur.nom == "Enrique Iglesias" && u8"Le nom du compositeur doit être Enrique Iglesias.");
}

// Test d'une entrée avec seulement le nom de la chanson.
void test_creation_morceau_entree_chanson_uniquement()
{
    std::istringstream entree{ "Duel of the fates | |" };
    Morceau morceau{};

    entree >> morceau;

    assert(morceau.nom == "Duel of the fates" && u8"Le nom du morceau doit être Duel of the Fates.");
    assert(morceau.album.nom == "Album inconnu" && u8"Le nom de l'album doit être Album inconnu.");
    assert(morceau.compositeur.nom == "Artiste inconnu" && u8"Le nom du compositeur doit être Artiste inconnu.");
}

// Test d'une entrée vide.
void test_creation_morceau_entree_vide()
{
    std::istringstream entree{ "| |" };
    Morceau morceau{};

    entree >> morceau;

    assert(morceau.nom == "Morceau inconnu" && u8"Le nom du morceau doit être Morceau inconnu.");
    assert(morceau.album.nom == "Album inconnu" && u8"Le nom de l'album doit être Album inconnu.");
    assert(morceau.compositeur.nom == "Artiste inconnu" && u8"Le nom du compositeur doit être Artiste inconnu.");
}

using Discographie = std::vector<Morceau>;

void tri_morceau(Discographie& discographie)
{
    std::sort(std::begin(discographie), std::end(discographie), [](Morceau const & lhs, Morceau const & rhs)
    {
        return lhs.nom < rhs.nom;
    });
}

void tri_album(Discographie& discographie)
{
    std::sort(std::begin(discographie), std::end(discographie), [](Morceau const & lhs, Morceau const & rhs)
    {
        if (lhs.album.nom < rhs.album.nom)
            return true;

        return lhs.album.nom == rhs.album.nom && lhs.nom < rhs.nom;
    });
}

void tri_artiste(Discographie& discographie)
{
    std::sort(std::begin(discographie), std::end(discographie), [](Morceau const & lhs, Morceau const & rhs)
    {
        if (lhs.compositeur.nom < rhs.compositeur.nom)
            return true;

        if (lhs.compositeur.nom == rhs.compositeur.nom)
        {
            if (lhs.album.nom < rhs.album.nom)
                return true;

            return lhs.album.nom == rhs.album.nom && lhs.nom < rhs.nom;
        }

        return false;
    });
}

// Affichage de la discographie par morceau, dans l'ordre alphabétique.
void affichage_morceau(Discographie const & discographie)
{
    for (Morceau const & morceau : discographie)
    {
        std::cout << "--> " << morceau << std::endl;
    }
}

// Affichage de la discographie par album, dans l'ordre alphabétique.
void affichage_album(Discographie const & discographie)
{
    Album album_precedent{};
    for (Morceau const & morceau : discographie)
    {
        if (morceau.album.nom != album_precedent.nom)
        {
            std::cout << "--> " << morceau.album << " | " << morceau.compositeur << std::endl;
        }
        std::cout << "\t/--> " << morceau.nom << std::endl;

        album_precedent = morceau.album;
    }
}

// Affichage de la discographie par artiste, dans l'ordre alphabétique.
void affichage_artiste(Discographie const & discographie)
{
    Artiste artiste_precedent{};
    Album album_precedent{};

    for (Morceau const & morceau : discographie)
    {
        if (morceau.compositeur.nom != artiste_precedent.nom)
        {
            std::cout << "--> " << morceau.compositeur << std::endl;
        }

        if (morceau.album.nom != album_precedent.nom)
        {
            std::cout << "\t/--> " << morceau.album << std::endl;
        }

        std::cout << "\t\t/--> " << morceau.nom << std::endl;

        artiste_precedent = morceau.compositeur;
        album_precedent = morceau.album;
    }
}

// L'ensemble des possibilités d'affichage de la discographie.
enum class Affichage { Artiste, Album, Morceau };

void affichage(Discographie& discographie, Affichage type_affichage)
{
    if (type_affichage == Affichage::Album)
    {
        tri_album(discographie);
        affichage_album(discographie);
    }
    else if (type_affichage == Affichage::Artiste)
    {
        tri_artiste(discographie);
        affichage_artiste(discographie);
    }
    else if (type_affichage == Affichage::Morceau)
    {
        tri_morceau(discographie);
        affichage_morceau(discographie);
    }
    else
    {
        // Par exemple si on met à jour l'énumération mais qu'on oublie d'ajouter la fonction correspondante.
        throw std::runtime_error("Commande d'affichage inconnue.");
    }
}

void enregistrement(Discographie const & discographie, std::string const & nom_fichier)
{
    std::ofstream fichier{ nom_fichier };
    if (!fichier)
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier en écriture.");
    }

    for (Morceau const & morceau : discographie)
    {
        fichier << morceau << std::endl;
    }
}

void chargement(Discographie & discographie, std::string const & nom_fichier)
{
    std::ifstream fichier{ nom_fichier };
    if (!fichier)
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier en lecture.");
    }

    std::string ligne{};
    while (std::getline(fichier, ligne))
    {
        Morceau morceau{};

        std::istringstream flux{ ligne };
        flux >> morceau;

        discographie.push_back(morceau);
    }
}

// Récupération de ce l'utilisateur écrit.
std::string recuperer_commande()
{
    std::cout << "> ";

    std::string commande{};
    std::getline(std::cin, commande);
    return commande;
}

// L'ensemble des actions qu'il est possible de faire.
enum class Commande { Afficher, Ajouter, Enregistrer, Charger, Quitter };

// Analyse du texte reçu en entrée.
std::tuple<Commande, std::string> analyser_commande(std::string const & commande_texte)
{
    // Pour traiter la chaîne comme étant un flux, afin d'en extraire les données.
    std::istringstream flux{ commande_texte };

    std::string premier_mot{};
    flux >> premier_mot;
    premier_mot = traitement_chaine(premier_mot);

    std::string instructions{};
    std::getline(flux, instructions);
    instructions = traitement_chaine(instructions);

    if (premier_mot == "afficher")
    {
        // Le mode d'affichage.
        return { Commande::Afficher, instructions };
    }
    else if (premier_mot == "ajouter")
    {
        // Les informations à ajouter.
        return { Commande::Ajouter, instructions };
    }
    else if (premier_mot == "enregistrer")
    {
        // Le fichier à écrire.
        return{ Commande::Enregistrer, instructions };
    }
    else if (premier_mot == "charger")
    {
        // Le fichier à lire.
        return { Commande::Charger, instructions };
    }
    else if (premier_mot == "quitter")
    {
        // Chaîne vide, car on quitte le programme sans autre instruction.
        return { Commande::Quitter, std::string {} };
    }
    else
    {
        // On a reçu du texte qui est incorrect.
        throw std::runtime_error("Commande invalide.");
    }
}

// La commande à exécuter ainsi que la suite des instructions.
bool executer_commande(Discographie & discographie, Commande commande, std::string const & instructions)
{
    if (commande == Commande::Afficher)
    {
        if (instructions == "artistes")
        {
            affichage(discographie, Affichage::Artiste);
        }
        else if (instructions == "albums")
        {
            affichage(discographie, Affichage::Album);
        }
        else if (instructions == "morceaux")
        {
            affichage(discographie, Affichage::Morceau);
        }
        else
        {
            // Si on se trouve ici, alors c'est qu'il y a une erreur dans les instructions d'affichage.
            throw std::runtime_error("Commande d'affichage inconnue.");
        }
    }
    else if (commande == Commande::Ajouter)
    {
        std::istringstream flux{ instructions };
        Morceau morceau{};
        flux >> morceau;
        discographie.push_back(morceau);
    }
    else if (commande == Commande::Charger)
    {
        chargement(discographie, instructions);
    }
    else if (commande == Commande::Enregistrer)
    {
        enregistrement(discographie, instructions);
    }
    else if (commande == Commande::Quitter)
    {
        // Plus rien à faire, on quitte.
        return false;
    }

    return true;
}

int main()
{
    // Lancement préalable des tests unitaires.
    test_creation_morceau_entree_complete();
    test_creation_morceau_entree_espaces_partout();
    test_creation_morceau_entree_chanson_artiste();
    test_creation_morceau_entree_chanson_uniquement();
    test_creation_morceau_entree_vide();

    // On préremplit la discographie.
    Artiste const dave_brubeck{ "Dave Brubeck " };
    Artiste const secret_garden{ "Secret Garden" };
    Artiste const indochine{ "Indochine" };

    Album const time_out{ "Time Out" };
    Album const songs_from_a_secret_garden{ "Songs from a Secret Garden" };
    Album const l_aventurier{ "L'aventurier" };
    Album const paradize{ "Paradize" };

    Morceau const take_five{ "Take Five", dave_brubeck, time_out };
    Morceau const blue_rondo_a_la_turk{ "Blue Rondo a la Turk", dave_brubeck, time_out };
    Morceau const nocturne{ "Nocturne", secret_garden, songs_from_a_secret_garden };
    Morceau const aventurier{ "L'aventurier", indochine, l_aventurier };
    Morceau const j_ai_demande_a_la_lune{ "J'ai demandé à la lune", indochine, paradize };

    Discographie discographie{ take_five, blue_rondo_a_la_turk, nocturne, aventurier, j_ai_demande_a_la_lune };

    bool continuer{ true };
    do
    {
        try
        {
            std::string entree{ recuperer_commande() };
            auto[commande, instructions] = analyser_commande(entree);
            instructions = traitement_chaine(instructions);
            continuer = executer_commande(discographie, commande, instructions);
        }
        catch (std::runtime_error const & exception)
        {
            std::cout << "Erreur : " << exception.what() << std::endl;
        }

    } while (continuer);

    return 0;
}


*/