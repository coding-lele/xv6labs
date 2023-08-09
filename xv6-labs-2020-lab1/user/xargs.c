#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command>\n");
        exit(1);
    }

    char* argvs[MAXARG]; // 存放子进程 exec 的参数
    char buf[32], line_buf[32];
    int index = 0, length, n = 0;
    char *p;
    p = line_buf;

    for (int i = 1; i < argc; i++) {
        argvs[index++] = argv[i];
    }
    while ((length = read(0, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < length; i++) {
        if (buf[i] == '\n') {  //遇到换行符
            line_buf[n] = 0;   // 标志结束
            argvs[index++] = p;
            argvs[index] = 0;  // 当前行的参数结束，设为 NULL
            n = 0;
            p = line_buf;
            index = argc - 1;  // 重置 index
            if (fork() == 0) {
                exec(argv[1], argvs);
            }
            wait(0);  // 等待子进程
        } 
        else if (buf[i] == ' ') { //遇到空格，读取当前命令参数到argvs中
            line_buf[n++] = 0;
            argvs[index++] = p;
            p = &line_buf[n];
        } 
        else { //将buf中的内容存储到line_buf中
            line_buf[n++] = buf[i];
        }
    }
    }
    exit(0);
}