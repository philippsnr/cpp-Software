#include <iostream>
#include <fstream>
#include <filesystem>
#include <getopt.h>
#include <jsoncpp/json/json.h>

using namespace std;

namespace fs = std::filesystem;

class Entry
{
public:
    string type;
    Entry(string _type) : type(_type) {}

    virtual string getBatchCommand()
    {
        return "";
    }
};
class ENV : public Entry
{
public:
    ENV(string _key, string _value) : Entry("ENV")
    {
        key = _key;
        value = _value;
    }
    string getBatchCommand()
    {
        return "set " + key + "=" + value;
    }

protected:
    string key;
    string value;
};
class EXE : public Entry
{
public:
    EXE(string _command) : Entry("EXE")
    {
        command = _command;
    }
    string getBatchCommand()
    {
        return command;
    }

protected:
    string command;
};
class PATH : public Entry
{
public:
    PATH(string _path) : Entry("PATH")
    {
        path = _path;
    }
    string getBatchCommand()
    {
        return path;
    }

protected:
    string path;
};

int JsonConverter(fs::path filePath);
int MakeBatch(string outputfile, bool hideshell, string application_path, vector<Entry *> entries);
int getOpt(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    getOpt(argc, argv);
    return EXIT_SUCCESS;
}

int getOpt(int argc, char *argv[])
{
    int opt;
    int option_index = 0;
    int asksforhelp = 0; // Um zu verhindern das er sagt "no fitting input ... " auch wenn man nur die optionen eingibt
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"json", no_argument, 0, 'j'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "hj", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'h':
            cout << "\nHow to use: provide the file path (absolut/relativ) or link to the JSON file as an argument.\nExample: ./jsondemo /Path/To/Your/File.json\n\nPut -j ore --json as an argument for an example of a json file\n\nProgramm developed by:\nNils Fleschhut,     TIT23, (fleschhut.nils@gmail.com) \nLinus Gerlach,      TIT23, (li.gerlach@freenet.de) \nPhillip Staudinger, TIK23, (philipp.eckhard.staudinger@gmail.com) \nJanne Nußbaum,      TIT23, (janu10.jn@gmail.com)\n"
                 << "\n"; // Req1-3 abgeschlossen
            asksforhelp = 1;
            break;
        case 'j':
            cout << "{\n \"outputfile\": \"(Name).bat\",\n \"hideshell\": (false/true),\n \"entries\": [\n	{\"type\": \"EXE\",\"command\": \"C:\\sicherung\\tools\\MinGW\\set_distro_paths.bat\"},\n	{\"type\": \"ENV\", \"key\": \"BOOST_INCLUDEDIR\", \"value\": \"C:\\sicherung\\tools\\MinGW\\include\"},\n	{\"type\": \"PATH\", \"path\": \"C:\\sicherung\\tools\\MinGW\\bin\"}\n ],\n \"application\": \"C:\\sicherung\\tools\\VSCode\"\n}\n";
            asksforhelp = 1;
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
            const fs::path filePath = fs::canonical(eingabe);
            cout << "-------------------------------------------------------------------\n";
            cout << "\"" << eingabe << "\" as input detected\n\nInitiating parsing \n\n";
            JsonConverter(filePath);
            cout << "-------------------------------------------------------------------\n";
            // Req 4-5 + 7 abgeschlossen
        }
        else if (asksforhelp == 0)
        {
            cout << i << ". input doesnt exist. -h or --help for further explanation\n\n";
        }
        // Req6 abgeschlossen
    }
    return EXIT_SUCCESS;
}

int JsonConverter(fs::path filePath)
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
        cerr << reader.getFormattedErrorMessages();
        cerr << "\n\n";
        return EXIT_FAILURE;
    }
    cout << "JSON successfully parsed! \n\n";
    for (const auto key : root.getMemberNames())
    {
        if (key != "outputfile" && key != "hideshell" && key != "entries" && key != "application")
        {
            const string unknownKey = key;
            // JSON-Datei erneut öffnen, um die Zeile des unbekannten Schlüssels zu finden
            file.clear();            // Stream-Flags zurücksetzen
            file.seekg(0, ios::beg); // Zurück zum Anfang der Datei

            string line;
            int lineNumber = 0;
            bool foundKey = false;
            while (getline(file, line))
            {
                lineNumber++;
                if (line.find(unknownKey) != string::npos)
                {
                    foundKey = true;
                    break;
                }
            }
            cout << "Unknown Key: " << unknownKey << "(LINE " << lineNumber << ")" << endl;
            return EXIT_FAILURE;
        }
    }
    // Req17 abgeschlossen
    cout << "Initiating reading JSON \n\n";
    // Werte aus JSON lesen
    // Outputfile überprüfen
    string outputfile = root["outputfile"].asString();
    // const string outputfileSUB = outputfile.substr(outputfile.length() - 4);
    //!(outputfile.length() >= 4 && outputfileSUB == ".bat")#
    if (outputfile.find("/") != string::npos || outputfile.find(92) != string::npos || outputfile.find("<") != string::npos || outputfile.find(">") != string::npos || outputfile.find(":") != string::npos || outputfile.find("*") != string::npos || outputfile.find("?") != string::npos || outputfile.find("\"") != string::npos || outputfile.find("|") != string::npos)
    {
        string choice;
        cout << "The name of outputfile contains symbol that might cause problems on windows! \n";
        cout << "Do you want to continue (1) or exit (2)? \n";
        do
        {
            cout << "Your choice: ";
            cin >> choice;
        } while (choice != "1" && choice != "2");
        if (choice == "2")
        {
            return EXIT_FAILURE;
        }
    }
    if (outputfile.find(".bat") == string::npos)
    {
        string choice;
        cout << "Error with outputfilename \nHas to have '.bat'. Use -h for more Information" << '\n';
        do
        {
            cout << "\nDo you want us to add \".bat\" or exit the program?" << '\n';
            cout << "Type \"1\" for adding \".bat\" or type \"2\" for exit" << '\n';
            cout << "Your choice: ";
            cin >> choice;
            cout << endl;
        } while (choice != "1" && choice != "2");
        if (choice == "1")
        {
            cout << "We will add \".bat\" to your given outputfile \n\n";
            outputfile = outputfile.append(".bat");
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    // Überprüfen ob Batch mit diesem Name schon existiert
    if (fs::exists(outputfile))
    {
        string choice;
        cout << "There is already a batch-file with this filename. Do you want to overwrite or exit?\n";
        do
        {
            cout << "Type \"1\" for overwrite or type \"2\" for exit \n";
            cout << "Your choice: ";
            cin >> choice;
            cout << endl
                 << endl;
        } while (choice != "1" && choice != "2");
        if (choice == "2")
        {
            return EXIT_FAILURE;
        }
    }
    // Req8 abgeschlossen
    //  hideshell überprüfen
    if (root["hideshell"].isBool() == false)
    {
        cerr << "Error: Wrong/no Value for key \"hideshell\" \n Use -h for Help" << '\n';
        return EXIT_FAILURE;
    }
    const bool hideshell = root["hideshell"].asBool(); // Req9 abgeschlossen
    const string application_path = root["application"].asString();
    if (application_path != "null" && application_path != "0")
    {
        const int index = application_path.rfind(92); /* 92 (ASCII-Code) = \ */
        const int length = application_path.length();
        const string application_name = application_path.substr(index + 1, length - index);
        cout << "application name: " << application_name << endl;
    }
    // Req16 abgeschlossen
    //  Einträge aus JSON lesen
    const Json::Value entries_json = root["entries"];
    if (entries_json.isArray())
    {
        for (const Json::Value entry_json : entries_json)
        {
            string type = entry_json["type"].asString();
            if (type == "EXE")
            {
                if (entry_json["command"].isNull())
                {
                    cerr << "Entry with type \"EXE\" found without key \"command\"\n";
                    return EXIT_FAILURE;
                }
                string command = entry_json["command"].asString();
                entries.push_back(new EXE(command));
            }
            else if (type == "ENV")
            {
                if (entry_json["key"].isNull())
                {
                    cerr << "Entry with type \"ENV\" found without key \"key\"\n";
                    return EXIT_FAILURE;
                }
                if (entry_json["value"].isNull())
                {
                    cerr << "Entry with type \"ENV\" found without key \"value\"\n";
                    return EXIT_FAILURE;
                }
                string key = entry_json["key"].asString();
                string value = entry_json["value"].asString();
                entries.push_back(new ENV(key, value));
            }
            else if (type == "PATH")
            {
                if (entry_json["path"].isNull())
                {
                    cerr << "Entry with type \"PATH\" found without key \"path\"\n";
                    return EXIT_FAILURE;
                }
                string path = entry_json["path"].asString();
                entries.push_back(new PATH(path));
            }
            else if (type.empty())
            {
                cerr << "Entry found with no type. Use -h for Help" << '\n';
                return EXIT_FAILURE;
            }
            else
            {
                cerr << "Unknown type: " << type << "Use -h for Help" << '\n';
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        cerr << "Error: entries: Not an Array. \n Use -h for Help" << endl;
        return EXIT_FAILURE;
    }
    // Req10-15 abgeschlossen
    cout << "JSON successfully read! \n\n";
    cout << "Initiating Batch creation \n\n";

    MakeBatch(outputfile, hideshell, application_path, entries);

    // Iteriere über den Vektor und lösche die Objekte
    for (auto entry : entries)
    {
        delete entry;
    }
    // Lösche alle Einträge im Vektor
    entries.clear();
    return EXIT_SUCCESS;
}

int MakeBatch(string outputfile, bool hideshell, string application_path, vector<Entry *> entries)
{
    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    ofstream batchFile(outputfile);

    if (!batchFile.is_open())
    {
        cerr << "Error while opening the batch" << endl;
        return EXIT_FAILURE;
    }
    batchFile << "@echo on\r\n";                    // Req24
    batchFile << "C:\\Windows\\System32\\cmd.exe "; // Req18 abgeschlossen
    if (hideshell == 0)
    {
        batchFile << "/k";
    }
    else
    {
        batchFile << "/c";
    }
    // Req19 abgeschlossen
    batchFile << " \"";
    for (const auto entry : entries)
    {
        if (entry->type == "EXE")
        {
            batchFile << "\"" << entry->getBatchCommand() << "\""
                      << " && ";
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
    // Req20-23 abgeschlossen
    if (application_path != "null" && application_path != "0")
    {
        if (entries.empty() == false)
        {
            batchFile << " && ";
        }
        batchFile << "\"" << application_path << "\"";
    }
    // Req25 abgeschlossen
    batchFile << "\" \r\n"; // Req24
    batchFile << "@echo off";
    // Datei schließen
    batchFile.close();
    cout << "Batch successfully created! \n\n";

    return EXIT_SUCCESS;
}
