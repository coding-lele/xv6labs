#include "kernel/types.h"
#include "user/user.h"

void sieve(int p_to_c[]){
    int prime;
    int num;
    int c_to_gc[2]; // 子进程到孙子进程

    pipe(c_to_gc);

     // 读取第一个素数
    if (read(p_to_c[0], &prime, sizeof(prime)) <= 0) {
        //当read()==0时表示已读取到文件的末尾；<0为错误情况
        close(p_to_c[0]); 
        close(c_to_gc[0]);
        close(c_to_gc[1]);
        exit(0);
    }
    printf("prime %d\n", prime);

     // 创建孙子进程
    int pid;
    pid = fork();
   
    if (pid == 0) {
        // 孙子进程
        close(c_to_gc[1]); // 关闭孙子进程到父进程的管道的写端
        close(p_to_c[0]); // 孙子进程不需要父进程的管道读端
        sieve(c_to_gc);
    }
    else {
        // 对当前这个子进程
        close(c_to_gc[0]); // 关闭子进程到孙子进程的管道的读端
        while (1) {
            // 从父进程读取数据
            if (read(p_to_c[0], &num, sizeof(num)) <= 0) {
                break;
            } // 如果末尾时跳出循环

            if (num % prime != 0) {
                // 将非素数发送给孙子进程
               write(c_to_gc[1], &num, sizeof(num));
            }
        }

        // 关闭相关通信管道
        close(p_to_c[0]);
        close(c_to_gc[1]);

        // 等待孙子进程结束
        wait(0);
    }
    
}

int main(int argc, char *argv[]){
    int p_to_c[2]; //父进程到子进程
    pipe(p_to_c); //创建管道
       
    int pid;
    pid=fork();
    if(pid==0){ 
        //子进程
        close(p_to_c[1]); // 关闭父子进程的管道的写端
        sieve(p_to_c);
    }
    else{
        //父进程
         close(p_to_c[0]); //关闭父进程到子进程的管道的读端
         for(int i=2;i<=35;i++){ //将数据写入管道，发送给子进程
             write(p_to_c[1], &i, sizeof(i));
         }
       
        close(p_to_c[1]); // 关闭管道，否则子进程会阻塞

        wait(0); // 等待子进程
    }

    exit(0); 
}