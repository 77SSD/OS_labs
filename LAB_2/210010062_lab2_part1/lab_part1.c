#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
void print(char arr[]){
    int n = 11;

    for(int i = 0; i<n; i++){
        int value = fork();
        if(value == 0){
            printf("%c and (Pid : %d)\n", arr[i], (int) getpid());
        }
        sleep(rand()%4 + 1);
        if(value != 0){
            exit(0);
        }  
    }
}



int main(){
    char arr[12] = "Hello World"; 
    print(arr);

    return 0;
}
