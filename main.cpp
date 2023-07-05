#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>
#include <vector>
#include <string>

bool rectangle_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

int main(int argc, char* argv[]) {
    const int windowWidth = 800;
    const int windowHeight = 600;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cout << "SDL TTF could not initialize! SDL_Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Learning C++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cout << "SDL window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "SDL renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Rect player = { 100, 100, 50, 50 };
    SDL_Rect enemy = { windowWidth - 100, 0, 50, 50 };
    std::vector<SDL_Rect> bullets;
    std::vector<SDL_Rect> enemies;
    const int maxEnemies = 3;

    std::random_device rd;
    std::mt19937 generator(rd());

    // Create a uniform distribution for the range
    std::uniform_int_distribution<int> distribution(100, windowHeight - 100);

    const char* fontPath = "open_sans.ttf";
    const int fontSize = 24;
    TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
    if (font == nullptr) {
        std::cout << "Font could not initialize! " << TTF_GetError() << std::endl;
        return 1;
    }

    int score = 0;
    std::string text = "Score: " + std::to_string(score);
    SDL_Color textColor = { 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == nullptr) {
        std::cout << "Text surface initialize! " << std::endl;
        return 1;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);  // Free the surface once the texture is created
    if (textTexture == nullptr) {
        std::cout << "Text texture initialize! " << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not be initialized! SDL_mixer Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Load background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("background_music.wav");
    if (backgroundMusic == nullptr) {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    // Load sound effect
    Mix_Chunk* soundEffect = Mix_LoadWAV("piupiu.wav");
    if (soundEffect == nullptr) {
        std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(backgroundMusic);
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    // Play background music infinitely
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        std::cout << "Failed to play background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                // Handle key press
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_UP:
                case SDLK_w:
                    player.y -= 10;
                    if (player.y == 0)
                        player.y = windowHeight;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    player.y += 10;
                    if (player.y == windowHeight)
                        player.y = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    player.x -= 10;
                    if (player.x == 0)
                        player.x = 10;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    player.x += 10;
                    if (player.x >= windowWidth / 2)
                        player.x = 10;
                    break;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    SDL_Rect bullet = { player.x + player.w + 2, player.y, 25, 10 };
                    bullets.push_back(bullet);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Change color for main player
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // Draw the player
        SDL_RenderFillRect(renderer, &player);

        for (int i = 0; i < bullets.size(); i++) {
            // Set color for Bullets
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &bullets[i]);
            bullets[i].x += 2; // Move the bullet to the right by 2 pixels

            // Check if the bullet reaches the screen width
            if (bullets[i].x > windowWidth) {
                // Remove the bullet from the vector
                bullets.erase(bullets.begin() + i);
                i--; // Decrement the loop variable to account for the erased element
            }
        }

        if (enemies.size() < maxEnemies) {
            for (int i = 0; i < maxEnemies; i++) {
                enemy.y = distribution(generator);
                enemies.push_back(enemy);
            }
        }

        for (int i = 0; i < enemies.size(); i++) {
            SDL_Rect enemy = enemies[i];
            // Set color for Enemies
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &enemy);
            for (SDL_Rect bullet : bullets) {
                bool collision = rectangle_collision(enemy.x, enemy.y, enemy.w, enemy.h, bullet.x, bullet.y, bullet.w, bullet.h);
                if (collision) {
                    score += 1;
                    text = "Score:" + std::to_string(score);
                    // Play the sound effect
                    if (Mix_PlayChannel(-1, soundEffect, 0) == -1) {
                        std::cout << "Failed to play sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
                    }
                    enemies.erase(enemies.begin() + i);
                    i--;
                }
            }
        }

        textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { 400, 500, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        // Update the window
        SDL_RenderPresent(renderer);
    }

    Mix_FreeChunk(soundEffect);
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
