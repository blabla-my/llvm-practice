# include <stdio.h>

void hello () {
    printf("Hello,world.\n");
}

void hello2() {
    int x,y=0;
    int c = x+y;
    printf("%d\n",c);
}

int main() {
    hello();
    hello2();
    hello();
    hello2();
}
