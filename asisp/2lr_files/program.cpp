// ЛР2 Работа с файлами и каталогами ОС UNIX
// 10.Написать программу, находящую глубину вложенности,
// количество файлов, дату создания для всех подкаталогов
// начиная с заданного (первый аргумент командной строки).
// Вывести на консоль полный путь, размер, дату создания,
// права доступа, номер индексного дескриптора.
#include <string>
#include <cstring>
#include <iostream>
#include <experimental/filesystem>
#include <stdio.h>
#include <dirent.h>
#include <algorithm>
#include <sys/stat.h>
#include <time.h>

#define WHITE "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define RED "\033[0;31m"
#define YELLOW "\033[1;33m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"

using namespace std;
namespace fs = std::experimental::filesystem;

int getFilesCount(string path);
void printDirEntryInfo(fs::directory_entry dirEntry);
string getPermissions(mode_t perm);

int main(int argc, char **argv)
{
    std::string path = argv[1];
    string fullMainPath = fs::absolute(path.c_str());
    string parentMainPath(fullMainPath);
    parentMainPath = parentMainPath.substr(0, parentMainPath.find_last_of('/'));
    parentMainPath = parentMainPath.substr(0, parentMainPath.find_last_of('/'));
    using recDirIter = fs::recursive_directory_iterator;
    for (const auto &dirEntry : recDirIter(parentMainPath))
    {
        string curFullPath = fs::absolute(dirEntry.path().c_str());
        char *pathArr = &curFullPath[0];
        DIR *d = opendir(pathArr); // open the path
        if (d == NULL)
            continue; // if was not able return

        curFullPath += '/';
        if (!curFullPath.compare(0, fullMainPath.size(), fullMainPath) == 0)
            continue;
        printDirEntryInfo(dirEntry);
    }
}

void printDirEntryInfo(fs::directory_entry dirEntry)
{
    string fullPath = fs::absolute(dirEntry.path().c_str());
    struct dirent *direntStruct; // for the directory entries

    struct stat fileStatus;
    stat(fullPath.c_str(), &fileStatus);

    int nestingLevel = -1 + std::count_if(
                                dirEntry.path().begin(),
                                dirEntry.path().end(),
                                [](auto &dirEntry) { return true; });

    struct tm *timeinfo = gmtime(&fileStatus.st_ctime);
    char buffer[80];
    strftime(buffer, 80, "%F/%a/%T", timeinfo);
    string creationTime(buffer);

    std::cout << '\n'
              << BLUE << fullPath << std::endl
              << GREEN << nestingLevel << "LVL "
              << RED << getFilesCount(fullPath) << "FILES "
              << YELLOW << fileStatus.st_size << "B "
              << WHITE << creationTime << ' '
              << PURPLE << getPermissions(fileStatus.st_mode) << ' '
              << CYAN << fileStatus.st_ino << "ID " << std::endl;
}

int getFilesCount(string path)
{
    char *pathArr = &path[0];
    DIR *d = opendir(pathArr); // open the path
    if (d == NULL)
        return 0; // if was not able return
    int filesCount = 0;
    struct dirent *dirEntry; // for the directory entries

    while ((dirEntry = readdir(d)) != NULL)
        if (dirEntry->d_type == 8)
            // The values returned are specific to the operating system.
            // Under Unix, value 8 is a regular file and 4 is a directory.
            filesCount++;

    closedir(d); // finally close the directory

    return filesCount;
}

string getPermissions(mode_t perm)
{
    char bits[9];
    bits[0] = (perm & S_IRUSR) ? 'r' : '-';
    bits[1] = (perm & S_IWUSR) ? 'w' : '-';
    bits[2] = (perm & S_IXUSR) ? 'x' : '-';

    bits[3] = (perm & S_IRGRP) ? 'r' : '-';
    bits[4] = (perm & S_IWGRP) ? 'w' : '-';
    bits[5] = (perm & S_IXGRP) ? 'x' : '-';

    bits[6] = (perm & S_IROTH) ? 'r' : '-';
    bits[7] = (perm & S_IWOTH) ? 'w' : '-';
    bits[8] = (perm & S_IXOTH) ? 'x' : '-';

    return std::string(bits);
}