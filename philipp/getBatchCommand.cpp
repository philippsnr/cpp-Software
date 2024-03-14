#include <iostream>

using namespace std;


string getBatchCommandENV(string key, string value)
{
    return "set " + key + "=" + value;
}

string getBatchCommandEXE(string command)
{
    return "";
}

string getBatchCommandPATH(string path)
{
    return "set PATH=" + path + ";%PATH%";
}

int main(int argc, char const *argv[])
{
    cout << getBatchCommandENV("BOOST_INCLUDEDIR", "C:\\sicherung\\tools\\MinGW\\include");
    return 0;
}
