#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Error: no ip provided\n");
        exit(EXIT_FAILURE);
    }

    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *w = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool running = true;
    SDL_Rect player = { 100, 100, 20, 20 };

    SDL_Event evt;

    while (running)
    {
        char buf[100] = { 0 };
        read(sock, buf, 100 * sizeof(char));

        SDL_Point joystick;
        sscanf(buf, "%d %d", &joystick.x, &joystick.y);
        SDL_Point move = { joystick.x - 128, joystick.y - 128 };

        if (fabsf((float)move.x) < 10) move.x = 0;
        if (fabsf((float)move.y) < 10) move.y = 0;

        move.x = 3.f * (float)move.x / 100.f;
        move.y = 3.f * (float)move.y / 100.f;

        printf("%d %d\n", move.x, move.y);

        player.x += move.x;
        player.y += move.y;

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            }
        }

        SDL_RenderClear(r);

        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &player);

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderPresent(r);
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 0;
}

