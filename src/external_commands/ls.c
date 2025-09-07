#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

void print_permissions(__mode_t mode) {
    // File type
    if (S_ISDIR(mode)) printf("d");
    else if (S_ISLNK(mode)) printf("l");
    else if (S_ISCHR(mode)) printf("c");
    else if (S_ISBLK(mode)) printf("b");
    else if (S_ISFIFO(mode)) printf("p");
    else if (S_ISSOCK(mode)) printf("s");
    else printf("-");

    // Owner permissions
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    // Group permissions
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    // Other permissions
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void print_long_format(const char *filename, struct stat *st) {
    // permissions
    print_permissions(st->st_mode);
    
    // number of links
    printf(" %2ld", st->st_nlink);
    
    // owner
    struct passwd *pw = getpwuid(st->st_uid);
    if (pw != NULL) {
        printf(" %-8s", pw->pw_name);
    } else {
        printf(" %-8d", st->st_uid);
    }
    
    // group
    struct group *gr = getgrgid(st->st_gid);
    if (gr != NULL) {
        printf(" %-8s", gr->gr_name);
    } else {
        printf(" %-8d", st->st_gid);
    }
    
    // file size
    printf(" %8ld", st->st_size);
    
    // modification time
    struct tm *timeinfo = localtime(&st->st_mtime);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
    printf(" %s", time_str);
    
    // filename
    printf(" %s\n", filename);
}

void list_directory(int long_listing, int all) {
    DIR *dir = opendir(".");
    
    if (dir == NULL) {
        perror("Error opening current directory");
        return;
    }

    struct dirent *entry;
    struct stat entry_stat;
    while ((entry = readdir(dir)) != NULL) {
        if (!all && entry->d_name[0] == '.') {
            continue;
        }

        if (long_listing) {
            stat(entry->d_name, &entry_stat);
            print_long_format(entry->d_name, &entry_stat);
        } else {
            printf("%s\n", entry->d_name);
        }
    }
    printf("\n");

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int long_listing = 0; // Flag for -l
    int all = 0;          // Flag for -a

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            long_listing = 1;
        } else if (strcmp(argv[i], "-a") == 0) {
            all = 1;
        } else if (strcmp(argv[i], "-la") == 0 || strcmp(argv[i], "-al") == 0) {
            long_listing = 1;
            all = 1;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    list_directory(long_listing, all);

    return 0;
}