#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void main(int argc, char *argv[])
{
    int p_to_c[2]; //父进程到子进程
    int c_to_p[2]; //子进程到父进程

    if (pipe(p_to_c) < 0 || pipe(c_to_p) < 0) { //创建管道是否成功
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    int pid = fork(); //创建子进程
    if (pid == 0) {
        // 子进程
        close(p_to_c[1]); // 关闭从父进程到子进程的管道的写端
        close(c_to_p[0]); // 关闭子进程到父进程的管道的读端

        // 子进程从父进程读，打印
        char buf; 
        if (read(p_to_c[0], &buf, 1) != 1) {
            fprintf(2, "read from parent failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // 子进程将读到的内容写回子到父的管道
        if (write(c_to_p[1], &buf, 1) != 1) {
            fprintf(2, "write to parent failed\n");
            exit(1);
        }

        close(p_to_c[0]);
        close(c_to_p[1]);

        exit(0);
    } else {
        // 父进程
        close(p_to_c[0]); // 关闭从父进程到子进程的管道的读端
        close(c_to_p[1]); // 关闭从子进程到父进程的管道的写段

        char buf = 'a';
        // 父进程写到子进程
        if (write(p_to_c[1], &buf, 1) != 1) {
            fprintf(2, "write to child failed\n");
            exit(1);
        }

        // 父进程接受子进程的内容，打印
        if (read(c_to_p[0], &buf, 1) != 1) {
            fprintf(2, "read from child failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p_to_c[1]);
        close(c_to_p[0]);

        wait(0); // 等待子进程退出，防止其成为僵尸进程
        exit(0);
    }
}