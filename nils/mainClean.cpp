// Erstellen des Programm
// mkdir build
// cd build
// cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
// ninja
// Alternativ diesen Ordner mit VSCode oeffnen
// #include <config.h>
// #include <pathtest.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <getopt.h>
using namespace std;

namespace fs = std::filesystem;

#include <jsoncpp/json/json.h>

class Entry
{
public:
    string type;
    Entry(string _type) : type(_type) {}

    virtual string getBatchCommand()
    {
        return "";
    }
    virtual string getData()
    {
        return "";
    }
};
class ENV : public Entry
{
public:
    string key;
    string value;
    ENV(string _key, string _value) : Entry("ENV")
    {
        key = _key;
        value = _value;
    }
    string getBatchCommand()
    {
        return "set " + key + "=" + value;
    }
    string getData()
    {
        return key + " , value: " + value;
    }
};
class EXE : public Entry
{
public:
    string command;
    EXE(string _command) : Entry("EXE")
    {
        command = _command;
    }

    string getBatchCommand()
    {
        return command;
    }
    string getData()
    {
        return command;
    }
};
class PATH : public Entry
{
public:
    string path;
    PATH(string _path) : Entry("PATH")
    {
        path = _path;
    }
    string getBatchCommand()
    {
        return path;
    }
    string getData()
    {
        return path;
    }
};

int JsonReader(fs::path filePath)
{
    // Vector für Einträge aus entries
    vector<Entry *> entries;

    // JSON-Datei öffnen
    ifstream file(filePath);
    Json::Reader reader;
    Json::Value root;

    // JSON parsen
    if (!reader.parse(file, root))
    {
        cerr << "Error while parsing the JSON-file" << endl;
        return EXIT_FAILURE;
    }

    // Werte aus JSON lesen
    //Outputfile überprüfen
    string outputfile = root["outputfile"].asString();
    if (outputfile.find(".bat") == 0)
    {
        int choice = 0;
        cout << "Error with outputfilename. (LINE 2) \n Has to have '.bat'. Use -h for more Information" << '\n';
        do{
        cout << "Do you want us to add \".bat\" or exit the program ?" << '\n';
        cout << "Type \"1\" for adding \".bat\" or type \"2\" for exit" << '\n';
        cout << "Your choice: ";
        cin >> choice;
        cout << endl;
        }while(choice != 1 && choice != 2);
        if (choice == 1)
        {
            cout << "We will add \".bat\" to your given outputfile \n";
            outputfile = outputfile.append(".bat");
        }
        else 
        {
            return EXIT_FAILURE;
        }
    }
    //hideshell überprüfen
    const string hideshell_error_check = root["hideshell"].asString();
    if (hideshell_error_check != "false" || hideshell_error_check != "true")
    {
        cerr << "Error with variable hideshell: Wrong Value. (LINE ) \n Use -h for Help" << '\n';
        return EXIT_FAILURE;
    }

    const bool hideshell = root["hideshell"].asBool();
    const string application_path = root["application"].asString();
    if (application_path == "null" || application_path == "0")
    {

    }
    else
    {
    // Name der Applikation rausfinden
    const int index = application_path.rfind(92); /* 92 (ASCII-Code) = \ */
    const int length = application_path.length();
    const string application_name = application_path.substr(index + 1, length - index);
    }
    // Einträge aus JSON lesen
    const Json::Value entries_json = root["entries"];
    if (entries_json.isArray())
    {
        for (const Json::Value entry_json : entries_json)
        {
            string type = entry_json["type"].asString();
            if (type == "EXE")
            {
                string command = entry_json["command"].asString();
                entries.push_back(new EXE(command));
            }
            else if (type == "ENV")
            {
                string key = entry_json["key"].asString();
                string value = entry_json["value"].asString();
                entries.push_back(new ENV(key, value));
            }
            else if (type == "PATH")
            {
                string path = entry_json["path"].asString();
                entries.push_back(new PATH(path));
            }
            else
            {
                cerr << "Unknown type: " << type << ". (LINE X \n Use -h for Help" << '\n';
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        cerr << "Error: entries: Not an Array. \n Use -h for Help" << endl;
        return EXIT_FAILURE;
    }

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    ofstream batchFile(outputfile);

    if (!batchFile.is_open())
    {
        cerr << "Error while opening the batch" << endl;
        return EXIT_FAILURE;
    }
    batchFile << "@echo on\r\n";
    batchFile << "C:\\Windows\\System32\\cmd.exe ";
    if (hideshell == 0)
    {
        batchFile << "/k";
    }
    else // DABLEIBEN IST \C
    {
        batchFile << "/c";
    }
    batchFile << " \"";
    for (const auto entry : entries)
    {
        if (entry->type == "EXE")
        {
            batchFile << entry->getBatchCommand() << " && ";
        }
    }
    for (const auto entry : entries)
    {
        if (entry->type == "ENV")
        {
            batchFile << entry->getBatchCommand() << " && ";
        }
    }
    int prufer = 0;
    for (const auto entry : entries)
    {
        if (entry->type == "PATH")
        {
            if (prufer == 0)
            {
                batchFile << "set path=";
                prufer = 1;
            }
            batchFile << entry->getBatchCommand() << ";";
        }
    }
    if (prufer == 1)
    {
        batchFile << "%PATH%";
    }
    batchFile << "\" \r\n";
    batchFile << "@echo off";
    // Datei schließen
    batchFile.close();
    // Iteriere über den Vektor und lösche die Objekte
    for (auto entry : entries)
    {
        delete entry;
    }
    // Lösche alle Einträge im Vektor
    entries.clear();

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int opt;
    int option_index = 0;
    int correct_input = 0;
    int aksforhelp = 0;
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "h", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'h':
            cout << "How to use: provide the file path (absolut/relativ) or link to the JSON file as an argument.\nExample: ./getopttest /Path/To/Your/File.json\nProgramm developed by:\nNils Fleschhut (fleschhut.nils@gmail.com) \nLinus Gerlach (li.gerlach@freenet.de) \nPhillip Staudinger (philipp.eckhard.staudinger@gmail.com) \nJanne Nußbaum(janu10.jn@gmail.com) "
                 << "\n";
            aksforhelp = 1;
            break;
        default:
            cout << "-h or --help for help\n";
            break;
        }
    }
    for (int i = 1; i < argc; i++)
    {
        if (fs::exists(argv[i]))
        {
            string eingabe = argv[i];
            fs::path filePath = fs::canonical(eingabe);
            cout << eingabe << " as input detected. Initiating parsing \n\n\n";
            JsonReader(filePath);
            correct_input = 1;
        }
    }
    if (correct_input == 0 && aksforhelp == 0)
    {
        cout << "no fitting input detectet. -h or --help for further explanation\n";
    }
    return EXIT_SUCCESS;
}
