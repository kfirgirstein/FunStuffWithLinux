#include<stdio.h>

void logout() {
    printf("You are successfully logged out\n");
    int c = getchar();
    printf("thank you for pressing %d \n",c);

}

int main(){
logout();
return 0;
}
