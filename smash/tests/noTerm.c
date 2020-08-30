#include<stdio.h>
#include<signal.h>
void main(){
signal(SIGTERM,SIG_IGN);
sleep(1000);
}
