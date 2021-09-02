// 10.Разработать программу синхронизации двух каталогов, например, Dir1 и Dir2.
// Пользователь задаёт имена Dir1 и Dir2.
// В результате работы программы файлы, имеющиеся в Dir1, но отсутствующие в Dir2,
// должны скопироваться в Dir2 вместе с правами доступа.
// Процедуры копирования  должны запускаться в отдельном процессе для каждого копируемого файла.
// Каждый процесс выводит на экран свой pid, полный путь к копируемому файлу
// и число скопированных байт.
// Число одновременно работающих процессов не должно превышать N (вводится пользователем).
// Скопировать несколько файлов из каталога /etc в свой домашний каталог.
// Проверить работу программы для каталога /etc и домашнего каталога.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER 64 * 1024 * 1024

char *script_name = NULL;
int MAX_PROCESSES = 0;
int status = 0;
int process_count = 0;

void print_error(const int pid, const char *scr_name, const char *msg_err, const char *f_name)
{
    fprintf(stderr, "%d %s: %s %s\n", pid, scr_name, msg_err, (f_name) ? f_name : NULL);
}

void print_result(const char *dir, int bytes_count)
{
    printf("%d %s %d\n", getpid(), dir, bytes_count);
}

void copy_files(const char *current_dir1, const char *current_dir2, int *bytes_count)
{
    mode_t mode;
    struct stat file_information;
    if (access(current_dir1, R_OK) == -1)
    {
        print_error(getpid(), script_name, strerror(errno), current_dir1);
        return;
    }
    if ((stat(current_dir1, &file_information) == -1))
    {
        print_error(getpid(), script_name, strerror(errno), current_dir1);
        return;
    }

    mode = file_information.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);

    int file_source = open(current_dir1, O_RDONLY);
    int file_destination = open(current_dir2, O_WRONLY | O_CREAT, mode);

    if (file_source == -1)
    {
        print_error(getpid(), script_name, "ERROR file opening.", current_dir1);
        return;
    }
    if (file_destination == -1)
    {
        print_error(getpid(), script_name, "ERROR file opening.", current_dir2);
        return;
    }

    *bytes_count = 0;
    char *buff;

    if ((buff = (char *)malloc(BUFFER)) == NULL)
    {
        print_error(getpid(), script_name, "ERROR out of memory", NULL);
        return;
    }

    ssize_t read_val, write_val;
    while (((read_val = read(file_source, buff, BUFFER)) != 0) && (read_val != -1))
    {
        write_val = write(file_destination, buff, (size_t)read_val);
        // printing out ERRORS
        if (write_val == -1)
        {
            print_error(getpid(), script_name, "ERROR file closing.", current_dir2);
            if (close(file_source) == -1)
            {
                print_error(getpid(), script_name, "ERROR file closing.", current_dir1);
                return;
            }
            if (close(file_destination) == -1)
            {
                print_error(getpid(), script_name, "ERROR file closing.", current_dir2);
                return;
            }
            free(buff);
            return;
        }
        *bytes_count += write_val;
    }

    if (close(file_source) == -1)
    {
        print_error(getpid(), script_name, "ERROR file closing.", current_dir1);
        return;
    }
    if (close(file_destination) == -1)
    {
        print_error(getpid(), script_name, "EROR file closing.", current_dir2);
        return;
    }
}

void process_directory(char *dir1, char *dir2)
{
    DIR *copied_dir = opendir(dir1);
    if (!copied_dir)
    {
        print_error(getpid(), script_name, strerror(errno), dir1);
        return;
    }
    char *current_dir1 = (char *)alloca(strlen(dir1) + NAME_MAX + 3);
    current_dir1[0] = 0;
    strcat(strcat(current_dir1, dir1), "/");
    size_t current_len1 = strlen(current_dir1);

    char *current_dir2 = (char *)alloca(strlen(dir2) + NAME_MAX + 3);
    current_dir2[0] = 0;
    strcat(strcat(current_dir2, dir2), "/");
    size_t current_len2 = strlen(current_dir2);

    struct dirent *selected_dir;
    struct stat buf1, buf2;
    errno = 0;

    while ((selected_dir = readdir(copied_dir)) != NULL)
    {
        current_dir1[current_len1] = 0;
        strcat(current_dir1, selected_dir->d_name);
        current_dir2[current_len2] = 0;
        strcat(current_dir2, selected_dir->d_name);

        if ((lstat(current_dir1, &buf1) == -1))
        {
            print_error(getpid(), script_name, strerror(errno), current_dir1);
            continue;
        }
        if (S_ISDIR(buf1.st_mode)) // isDir
        {
            if ((strcmp(selected_dir->d_name, ".") != 0) && (strcmp(selected_dir->d_name, "..") != 0))
            {
                if (stat(current_dir2, &buf2) != 0)
                    mkdir(current_dir2, buf1.st_mode);
                process_directory(current_dir1, current_dir2);
            }
        }
        else if (S_ISREG(buf1.st_mode)) // isFile
        {
            if (process_count > MAX_PROCESSES)
            {
                int status;
                if (wait(&status) != 0)
                    --process_count;
            }

            if (stat(current_dir2, &buf2) != 0)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    int bytes_count = 0;
                    copy_files(current_dir1, current_dir2, &bytes_count);
                    if (bytes_count != 0)
                        print_result(current_dir1, bytes_count);
                    exit(EXIT_SUCCESS);
                }
                else if (pid < 0)
                    print_error(getpid(), script_name, "ERORR Process formation error.", NULL);
                process_count++;
            }
            else
                print_error(getpid(), script_name, "ERROR This file already exists.", current_dir2);
        }
    }
    if (closedir(copied_dir) == -1)
        print_error(getpid(), script_name, strerror(errno), dir1);
}

int main(int argc, char *argv[])
{
    script_name = basename(argv[0]);

    char *dir1 = realpath(argv[1], NULL);
    char *dir2 = realpath(argv[2], NULL);

    if (dir1 == NULL)
    {
        print_error(getpid(), script_name, "ERROR dir opening.", argv[1]);
        return EXIT_FAILURE;
    }
    if (dir2 == NULL)
    {
        print_error(getpid(), script_name, "ERROR dir opening.", argv[2]);
        return EXIT_FAILURE;
    }

    MAX_PROCESSES = atoi(argv[3]);
    if (MAX_PROCESSES == 0)
    {
        print_error(getpid(), script_name, "ERROR Invalid value of MAX_PROCESSES ", NULL);
        return EXIT_FAILURE;
    }

    process_directory(dir1, dir2);

    while (wait(NULL) > 0)
        ;

    return EXIT_SUCCESS;
}