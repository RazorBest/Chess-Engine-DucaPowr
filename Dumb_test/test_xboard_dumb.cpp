#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char mesaj[1024];
    
    while(scanf("%s", mesaj)) {
        ;
    }
    write(1, "BAAA\n", 6);
    fflush(stdout);
    // scanf("%s", mesaj); // Get "xboard"
    // scanf("%s", mesaj); // Get "protover N" with the version number N

    // printf("feature myname=\"DucaPowr\" san=0\n"); // Send "feature" command to xboard
    // scanf("%s", mesaj); // Get "accepted/rejected"

    // scanf("%s", mesaj); // Get "new"
    // scanf("%s", mesaj); // Get "random"

    // scanf("%s", mesaj); // get "move"
    // printf("move d7d5\n"); // Send "move"
    return 0;
}
