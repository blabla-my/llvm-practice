# include "hello.h"

void hello() {
    printf("Hello.\n");
}

void bye() {
    int a =1 ;
    int b =2 ;
    int c = a+b ;
    printf("c=%d\n", c);
    hello();
}

// int main() {
//     bye();
//     hello();
// }
