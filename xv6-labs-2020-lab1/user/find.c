#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) 
//格式化文件名(提取出文件或文件夹名)
//借鉴ls.c
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(char *path, char *target_name) {
    char buf[512];
    int fd;
    //使用kernel/fs.h中的头文件
    struct dirent de;  //存储目录项的信息,包括文件的 inode 号、文件名数组
    struct stat st;  //存储文件的状态信息,包括文件类型、所在设备号、inode 号、硬链接数和文件大小等信息。

    // 打开指定路径下的文件或目录
    if ((fd = open(path, 0)) < 0) { 
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    // 获取文件或目录的状态信息
    if (fstat(fd, &st) < 0) { 
        //检验是否成功获取文件或目录
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE: //文件
            if (strcmp(fmtname(path), target_name) == 0) {
                //strcmp 相同时返回0
                printf("%s\n", path);
            }
            break;

        case T_DIR: //目录
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                //目录路径不能太长（防止缓冲区溢出）
                printf("find: path too long\n");
                break;
            }
            // 遍历目录中的所有目录项
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                // 跳过.和..目录
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;

                // 拼接目录路径和目录项的文件名，得到新的路径
                memmove(buf, path, strlen(path));
                buf[strlen(path)] = '/';
                memmove(buf + strlen(path) + 1, de.name, DIRSIZ);
                buf[strlen(path) + 1 + DIRSIZ] = '\0';

                find(buf, target_name); //递归查找
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <pathname> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
