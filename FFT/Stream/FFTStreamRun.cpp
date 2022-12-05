#include <csignal>
#include "FFTStream.h"

bool running = true;

void signalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            running = false;
            break;
        default:
            break;
    }
}

int main(int argc, char **argv){
    FFTStream fftstream;
    fftstream.run();

    return 0;
}
