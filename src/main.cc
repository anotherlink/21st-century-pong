// Copyright [2015] <Chafic Najjar>

#include "src/pong.h"

int main(int argc, char* argv[]) {
    if (argc<5){
        printf("\n [!] USAGE : %s <X> <Y> <W> <H>\n\n" // " [joystick]\n\n"
         , argv[0]);
        exit(1);
    }

    Pong pong(argc, argv);
    printf("[ready]\n");
    pong.execute();
}
