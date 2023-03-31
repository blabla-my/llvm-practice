# include "hello.h"

void foo(){
  printf("I am foo\n");
}


int main(){
    hello();
    bye();
    hook(&foo);
}
