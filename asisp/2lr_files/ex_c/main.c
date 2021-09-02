
// ЛР2 Работа с файлами и каталогами ОС UNIX
// 10.Написать программу, находящую глубину вложенности,
// количество файлов, дату создания для всех подкаталогов
// начиная с заданного (первый аргумент командной строки).
// Вывести на консоль полный путь, размер, дату создания,
// права доступа, номер индексного дескриптора.

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

void showDirContent(char *path);

int main(int argc, char *argv[])
{
    printf("%s\n", NORMAL_COLOR);
    showDirContent(argv[1]);
    printf("%s\n", NORMAL_COLOR);
    return 0;
}

void showDirContent(char *path)
{
    DIR *d = opendir(path); // open the path
    if (d == NULL)
        return;                        // if was not able return
    struct dirent *dir;                // for the directory entries
    while ((dir = readdir(d)) != NULL) // if we were able to read somehting from the directory
    {
        char cwd[255];
        getcwd(cwd, sizeof(cwd));
        if (dir->d_type != DT_DIR) // if the type is not directory just print it with blue
            printf("%s%s\n", BLUE, dir->d_name);
        else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) // if it is a directory
        {
            printf("%s%s%s\n", GREEN, cwd, dir->d_name); // print its name in green
            char d_path[255];                          // here I am using sprintf which is safer than strcat
            sprintf(d_path, "%s/%s", path, dir->d_name);
            showDirContent(d_path); // recall with the new path
        }
    }
    closedir(d);
    return 0;
}