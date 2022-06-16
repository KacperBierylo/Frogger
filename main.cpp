#include<stdio.h>
#include<iostream>
#include<time.h>
#include"../frogger/SDL2-2.0.10/include/SDL.h"
#include"../frogger/SDL2-2.0.10/include/SDL_main.h"

#define SCREEN_WIDTH 671
#define SCREEN_HEIGHT 700
#define LOGS_IN_LINE 3
#define DOUBLE_TURTLES_IN_LINE 3
#define TRIPLE_TURTLES_IN_LINE 2
#define CAR_NUMBER 10 
#define LIVES 5
#define BAR_WIDTH 100
#define BAR_HEIGHT 36
#define TIME 50
#define CAR_SPEED 5
#define LOG_SPEED 1
#define TRIPLE_TURTLES_SPEED 2
#define DOUBLE_TURTLES_SPEED 1
#define LEFT_GAP 16
#define BAY_WIDTH 59
#define GAP_WIDTH 85
#define FROG_WIDTH 50
#define CAR_WIDTH 50
#define LOG_WIDTH 216
#define DOUBLE_TURTLES_WIDTH 100
#define TRIPLE_TURTLES_WIDTH 150

struct Character {
    int x;
    int y;
    SDL_Surface* graphic;
};

void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void SetCars(Character cars[], char side);
void SetLogs(Character logs[]);
void SetTurtles(Character turtles[], int number);                //liczba zolwi w grupie, mozliwe 2 lub 3
void SetAll(Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* leftCars, Character* rightCars, Character* frog);
void frogDeath(int* lives, int* quit, Character* frog, int* timer, int* barColour, int* barColourStart, int* score, int* maxY);
void lastJump(int* lives, int* quit, Character* frog, int* timer, int* barColour, int* barColourStart, int* score, int* maxY, int* win, Character* frogs, int* frogsOnLifes);
void objectsMovementAndCollisons(Character* frog, Character* leftCars, Character* rightCars, Character* logs, Character* tripleTurtles, Character* doubleTurtles, int* quit, int time);
void showMenu(SDL_Surface* menu, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit);
void gameOver(SDL_Surface* over, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, int* frogsOnLifes, int* score);
void leave(SDL_Surface* quitImg, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, int* win, int* frogsOnLifes, int* score);
void frogMovement(Character* frog, int* score, int* pause, int* quit, int timer, int* maxY, int* bonus, int* frogsOnLifes);
void showPause(SDL_Surface* pauseImg, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* pause);
void drawAll(Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* leftCars, Character* rightCars, Character* frog, SDL_Surface* screen);
int loadTextures(SDL_Surface** charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer,
    SDL_Surface** plansza, SDL_Surface** over, SDL_Surface** pauseImg, SDL_Surface** menu, Character* frog, SDL_Surface** quitImg,
    Character* leftCars, Character* rightCars, Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* frogs);

int main(int argc, char** argv) {
    srand(time(NULL));
    int state = 3, rc, time, time2 = 0, lives = LIVES, pause = 0, timeBar = SCREEN_WIDTH / 2, score = 0, maxY = SCREEN_HEIGHT - 25, secounds = 50, bonus[5] = {
    }, win = 0, frogsOnLifes = 0;               //state = 0 - gra trwa, = 1 - wybor po przegranej, albo œmieræ, jeœli ¿ycia > 1, = 2 - koniec programu, = 3 menu, = 4 pytanie o wyjscie
    char livesText[128];
    char scoreText[128];
    char bonusText[128][5];
    char timerText[32];
    SDL_Surface* screen;
    SDL_Surface* board;
    SDL_Surface* over;
    SDL_Surface* charset;
    SDL_Surface* pauseImg;
    SDL_Surface* menu;
    SDL_Surface* quitImg;
    SDL_Texture* scrtex;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    Character frog;
    Character leftCars[3 * CAR_NUMBER / 5];
    Character rightCars[2 * CAR_NUMBER / 5];
    Character logs[3 * LOGS_IN_LINE];  // 3 rzedy z balami
    Character doubleTurtles[DOUBLE_TURTLES_IN_LINE];
    Character tripleTurtles[TRIPLE_TURTLES_IN_LINE];
    Character frogs[5];
    SetAll(logs, doubleTurtles, tripleTurtles, leftCars, rightCars, &frog);                            //ustalenie pozycji obiektow
    rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    if (rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetWindowTitle(window, "Frogger");
    SDL_ShowCursor(SDL_DISABLE);

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
    int barColourStart = zielony;
    int barColourEnd = czerwony;
    int barColour = barColourStart;
    int timer = 0;

    loadTextures(&charset, screen, scrtex, window, renderer,
        &board, &over, &pauseImg, &menu, &frog, &quitImg,
         leftCars,  rightCars, logs, doubleTurtles, tripleTurtles, frogs);

    while (state != 2) {
        time = SDL_GetTicks();

        while (state == 3) {
            showMenu(menu, screen, renderer, scrtex, &state);
        }
        if (state == 2) break;

        SDL_FillRect(screen, NULL, czarny);
        DrawSurface(screen, board, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

        DrawRectangle(screen, 0, 0, BAR_WIDTH, BAR_HEIGHT, czerwony, czarny);
        sprintf(livesText, "Lives: %d ", lives);

        DrawRectangle(screen, 0, SCREEN_HEIGHT - BAR_HEIGHT, BAR_WIDTH, BAR_HEIGHT, czerwony, czarny);
        sprintf(scoreText, "Score: %d ", score);

        DrawRectangle(screen, SCREEN_WIDTH - timeBar, SCREEN_HEIGHT - 20, timeBar, 20, czarny, barColour);
        if (time - time2 >= 1000) {
            sprintf(timerText, "Time left: %d ", TIME - timer);
            timeBar = (TIME - timer) * SCREEN_WIDTH / (2 * TIME);                                                       //pasek czasu, œmieræ ¿aby po czasie
            time2 = time;
            timer++;
            if (timer > 40) barColour = barColourEnd;
            if (timer > TIME) {
                frogDeath(&lives, &state, &frog, &timer, &barColour, &barColourStart, &score, &maxY);
            }
        }

        DrawString(screen, 4, 10, livesText, charset);
        DrawString(screen, 4, SCREEN_HEIGHT - 18, scoreText, charset);
        DrawString(screen, SCREEN_WIDTH - 108, SCREEN_HEIGHT - 28, timerText, charset);                                 //rysowanie napisow
        for (int i = 0; i < frogsOnLifes; i++) {
            if (frogs[i].y <= SCREEN_HEIGHT - 645) {
                sprintf(bonusText[i], "Bonus: %d ", bonus[i]);
                DrawString(screen, frogs[i].x - 45, frogs[i].y - 30, bonusText[i], charset);
            }
        }
        drawAll(logs, doubleTurtles, tripleTurtles, leftCars, rightCars, &frog, screen);                             //rysowanie obiektow

        for (int i = 0; i < frogsOnLifes; i++) {
            DrawSurface(screen, frogs[i].graphic, frogs[i].x, frogs[i].y);                                  //rysowanie zab na lisciach
        }

        SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
        SDL_RenderCopy(renderer, scrtex, NULL, NULL);
        SDL_RenderPresent(renderer);
        frogMovement(&frog, &score, &pause, &state, timer, &maxY, bonus, &frogsOnLifes);                                   //sterowanie froggerem

        objectsMovementAndCollisons(&frog, leftCars, rightCars, logs, tripleTurtles, doubleTurtles, &state, time);          //kolizje froggera i poruszanie sie obiektow

        lastJump(&lives, &state, &frog, &timer, &barColour, &barColourStart, &score, &maxY, &win, frogs, &frogsOnLifes);         //wskakiwanie na pole koncowe

        if ((lives > 1) && (state == 1)) {
            frogDeath(&lives, &state, &frog, &timer, &barColour, &barColourStart, &score, &maxY);                               //smierc lub utrata zyc
        }

        if ((lives <= 1) && (state == 1)) {
            state = 1;
        }

        while (state == 1) {
            gameOver(over, screen, renderer, scrtex, &state, &frogsOnLifes, &score);                     //menu po przegranej

            if (state == 0) {
                lives = LIVES;
                SetAll(logs, doubleTurtles, tripleTurtles, leftCars, rightCars, &frog);

            }
        }

        while (pause == 1) {
            showPause(pauseImg, screen, renderer, scrtex, &pause);
        }

        while (state == 4) {
            leave(quitImg, screen, renderer, scrtex, &state, &win, &frogsOnLifes, &score);                                                               //zapytanie czy wyjsc po przycisnieciu q
            if (state == 3) {
                lives = LIVES;
                timer = 0;
                SetAll(logs, doubleTurtles, tripleTurtles, leftCars, rightCars, &frog);
            }
        }
    }

    SDL_FreeSurface(screen);
    SDL_DestroyTexture(scrtex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
    SDL_Surface* charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    }
}

void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
}

void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
}

void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    }
}

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
}

void SetCars(Character cars[], char side) {
    int y = 0;
    int howMany = 0;
    if (side == 'l') {
        y = SCREEN_HEIGHT - 75;
        howMany = 3;
    }
    else if (side == 'r') {
        y = SCREEN_HEIGHT - 125;
        howMany = 2;
    }
    int a = CAR_NUMBER / 5;
    for (int i = 0; i < howMany; i++) {
        int b = -(rand() % (SCREEN_WIDTH / 2));                                                                                         //pozycje samochodow
        for (int j = 0; j < a; j++) {                                                            // 5-liczba torow
            cars[j + i * a].y = y;
            cars[j + i * a].x = b + (rand() % 300 + SCREEN_WIDTH / (CAR_NUMBER / 5));
            b = cars[j + i * a].x;
            if (side == 'l')
                cars[j + i * a].graphic = SDL_LoadBMP("../frogger/graphics/redcarleft.bmp");
            else if (side == 'r')
                cars[j + i * a].graphic = SDL_LoadBMP("../frogger/graphics/redcarright.bmp");
        }
        y -= 100;
    }
}

void SetLogs(Character logs[]) {
    int y[3] = { SCREEN_HEIGHT - 425, SCREEN_HEIGHT - 475, SCREEN_HEIGHT - 575
    };

    for (int i = 0; i < 3; i++) {
        int b = -LOG_WIDTH;
        for (int j = 0; j < LOGS_IN_LINE; j++) {
            logs[j + i * LOGS_IN_LINE].y = y[i];
            logs[j + i * LOGS_IN_LINE].x = b + (rand() % (SCREEN_WIDTH / LOGS_IN_LINE) + LOG_WIDTH);
            b = logs[j + i * LOGS_IN_LINE].x;
            logs[j + i * LOGS_IN_LINE].graphic = SDL_LoadBMP("../frogger/graphics/log.bmp");

        }
    }
}

void SetTurtles(Character turtles[], int number) {
    int b = -SCREEN_WIDTH - DOUBLE_TURTLES_WIDTH;
    if (number == 3) {
        for (int i = 0; i < TRIPLE_TURTLES_IN_LINE; i++) {
            turtles[i].y = SCREEN_HEIGHT - 375;
            turtles[i].x = b + (rand() % (2 * SCREEN_WIDTH / TRIPLE_TURTLES_IN_LINE) + TRIPLE_TURTLES_WIDTH);
            b = turtles[i].x;
            turtles[i].graphic = SDL_LoadBMP("../frogger/graphics/3turtles.bmp");
        }
    }
    else if (number == 2) {
        for (int i = 0; i < DOUBLE_TURTLES_IN_LINE; i++) {
            turtles[i].y = SCREEN_HEIGHT - 525;
            turtles[i].x = b + (rand() % (2 * SCREEN_WIDTH / DOUBLE_TURTLES_IN_LINE) + DOUBLE_TURTLES_WIDTH);
            b = turtles[i].x;
            turtles[i].graphic = SDL_LoadBMP("../frogger/graphics/2turtles.bmp");
        }
    }
}

void SetAll(Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* leftCars, Character* rightCars, Character* frog) {
    frog->x = SCREEN_WIDTH / 2;
    frog->y = SCREEN_HEIGHT - 25;
    SetCars(rightCars, 'r');
    SetCars(leftCars, 'l');
    SetLogs(logs);
    SetTurtles(tripleTurtles, 3);
    SetTurtles(doubleTurtles, 2);
}
void frogDeath(int* lives, int* quit, Character* frog, int* timer, int* barColour, int* barColourStart, int* score, int* maxY) {
    if (*lives > 1)*quit = 0;
    else *quit = 1;
    frog->x = SCREEN_WIDTH / 2;
    frog->y = SCREEN_HEIGHT - 25;
    *timer = 0;
    *maxY = SCREEN_HEIGHT - 25;
    *barColour = *barColourStart;
    --* lives;
}

void lastJump(int* lives, int* quit, Character* frog, int* timer, int* barColour, int* barColourStart, int* score, int* maxY, int* win, Character* frogs, int* frogsOnLifes) {
    int check = 0;
    if (frog->y < SCREEN_HEIGHT - 575) {
        frog->y = SCREEN_HEIGHT - 645;                                                                          //wskakiwanie do pol koncowych

        if ((frog->x > 0) && (frog->x <= LEFT_GAP)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        if ((frog->x > LEFT_GAP) && (frog->x <= LEFT_GAP + BAY_WIDTH)) {
            frog->x = LEFT_GAP + BAY_WIDTH / 2;
            for (int i = 0; i < *frogsOnLifes; i++) {
                if (frog->x == frogs[i].x) check++;
            }
            if (check == 0) {
                frogs[*frogsOnLifes].x = frog->x;
                frogs[*frogsOnLifes].y = frog->y;
                ++* frogsOnLifes;
                frog->x = SCREEN_WIDTH / 2;
                frog->y = SCREEN_HEIGHT - 25;
                *timer = 0;
                *maxY = SCREEN_HEIGHT;
            }
            else frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);
        }

        else if ((frog->x > LEFT_GAP + BAY_WIDTH) && (frog->x <= LEFT_GAP + BAY_WIDTH + GAP_WIDTH)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        else if ((frog->x > LEFT_GAP + BAY_WIDTH + GAP_WIDTH) && (frog->x <= LEFT_GAP + 2 * BAY_WIDTH + GAP_WIDTH)) {
            frog->x = LEFT_GAP + GAP_WIDTH + BAY_WIDTH + BAY_WIDTH / 2;
            for (int i = 0; i < *frogsOnLifes; i++) {
                if (frog->x == frogs[i].x) check++;
            }
            if (check == 0) {
                frogs[*frogsOnLifes].x = frog->x;
                frogs[*frogsOnLifes].y = frog->y;
                ++* frogsOnLifes;
                frog->x = SCREEN_WIDTH / 2;
                frog->y = SCREEN_HEIGHT - 25;
                *timer = 0;
                *maxY = SCREEN_HEIGHT;
            }
            else frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        }

        else if ((frog->x > LEFT_GAP + 2 * BAY_WIDTH + GAP_WIDTH) && (frog->x <= LEFT_GAP + 2 * 60 + 2 * GAP_WIDTH)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        else if ((frog->x > LEFT_GAP + 2 * BAY_WIDTH + 2 * GAP_WIDTH) && (frog->x <= LEFT_GAP + 3 * BAY_WIDTH + 2 * GAP_WIDTH)) {
            frog->x = LEFT_GAP + 2 * GAP_WIDTH + 2 * BAY_WIDTH + BAY_WIDTH / 2;
            for (int i = 0; i < *frogsOnLifes; i++) {
                if (frog->x == frogs[i].x) check++;
            }
            if (check == 0) {
                frogs[*frogsOnLifes].x = frog->x;
                frogs[*frogsOnLifes].y = frog->y;
                ++* frogsOnLifes;
                frog->x = SCREEN_WIDTH / 2;
                frog->y = SCREEN_HEIGHT - 25;
                *timer = 0;
                *maxY = SCREEN_HEIGHT;
            }
            else frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);
        }

        else if ((frog->x > LEFT_GAP + 3 * BAY_WIDTH + 2 * GAP_WIDTH) && (frog->x <= LEFT_GAP + 3 * BAY_WIDTH + 3 * GAP_WIDTH)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        else if ((frog->x > LEFT_GAP + 3 * BAY_WIDTH + 3 * GAP_WIDTH) && (frog->x <= LEFT_GAP + 4 * BAY_WIDTH + 3 * GAP_WIDTH)) {
            frog->x = LEFT_GAP + 3 * GAP_WIDTH + 3 * BAY_WIDTH + BAY_WIDTH / 2;
            for (int i = 0; i < *frogsOnLifes; i++) {
                if (frog->x == frogs[i].x) check++;
            }
            if (check == 0) {
                frogs[*frogsOnLifes].x = frog->x;
                frogs[*frogsOnLifes].y = frog->y;
                ++* frogsOnLifes;
                frog->x = SCREEN_WIDTH / 2;
                frog->y = SCREEN_HEIGHT - 25;
                *timer = 0;
                *maxY = SCREEN_HEIGHT;
            }
            else frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);
        }

        else if ((frog->x > LEFT_GAP + 4 * BAY_WIDTH + 3 * GAP_WIDTH) && (frog->x <= LEFT_GAP + 4 * BAY_WIDTH + 4 * GAP_WIDTH)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);

        else if ((frog->x > LEFT_GAP + 4 * BAY_WIDTH + 4 * GAP_WIDTH) && (frog->x <= LEFT_GAP + 5 * BAY_WIDTH + 4 * GAP_WIDTH)) {
            frog->x = LEFT_GAP + 4 * GAP_WIDTH + 4 * BAY_WIDTH + BAY_WIDTH / 2;
            for (int i = 0; i < *frogsOnLifes; i++) {
                if (frog->x == frogs[i].x) check++;
            }
            if (check == 0) {
                frogs[*frogsOnLifes].x = frog->x;
                frogs[*frogsOnLifes].y = frog->y;
                ++* frogsOnLifes;
                frog->x = SCREEN_WIDTH / 2;
                frog->y = SCREEN_HEIGHT - 25;
                *timer = 0;
                *maxY = SCREEN_HEIGHT;
            }
            else frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);
        }

        else if ((frog->x > LEFT_GAP + 5 * BAY_WIDTH + 4 * GAP_WIDTH) && (frog->x <= SCREEN_WIDTH)) frogDeath(lives, quit, frog, timer, barColour, barColourStart, score, maxY);
    }
}

void objectsMovementAndCollisons(Character* frog, Character* leftCars, Character* rightCars, Character* logs, Character* tripleTurtles, Character* doubleTurtles, int* quit, int time) {

    if ((frog->x > SCREEN_WIDTH) || (frog->x < 0) || (frog->y < 0) || (frog->y > SCREEN_HEIGHT)) *quit = 1;         //gdy zaba wyjdzie poza okno

    if (time % 5 == 0) {
        for (int i = 0; i < 3 * CAR_NUMBER / 5; i++) {
            leftCars[i].x -= CAR_SPEED;
            if ((frog->y == leftCars[i].y) && (abs(frog->x - leftCars[i].x) < (FROG_WIDTH + CAR_WIDTH) / 2)) *quit = 1;
            if (leftCars[i].x < -SCREEN_WIDTH + CAR_WIDTH) leftCars[i].x = SCREEN_WIDTH + CAR_WIDTH;
        }

        for (int i = 0; i < 2 * CAR_NUMBER / 5; i++) {
            rightCars[i].x += CAR_SPEED;
            if ((frog->y == rightCars[i].y) && (abs(frog->x - rightCars[i].x) < (FROG_WIDTH + CAR_WIDTH) / 2)) *quit = 1;
            if (rightCars[i].x > 2 * SCREEN_WIDTH - CAR_WIDTH) rightCars[i].x = -CAR_WIDTH;
        }
    }

    if (time % 5 == 0) {
        for (int i = 0; i < 3 * LOGS_IN_LINE; i++) {
            if (frog->y == logs[i].y) *quit = 1;                                                                    //ustawienie porazki, gdy zaba znajdzie sie na wysokosci ktorejs z klod

        }
        for (int i = 0; i < 3 * LOGS_IN_LINE; i++) {
            logs[i].x += LOG_SPEED;
            if ((frog->y == logs[i].y) && (abs(frog->x - logs[i].x) < (LOG_WIDTH) / 2)) {
                *quit = 0;                                                                                                              //cofniecie porazki jesli zaba jest na ktorejs z klod
                frog->x += LOG_SPEED;
            }

            if (logs[i].x > 2 * SCREEN_WIDTH - LOG_WIDTH) logs[i].x = -LOG_WIDTH;
        }
    }

    if (time % 5 == 0) {
        if ((frog->y == tripleTurtles[0].y) || (frog->y == doubleTurtles[0].y)) *quit = 1;              //ustawienie porazki, gdy zaba znajduje sie na wysokosci ktorejsc z grup zolwi
        for (int i = 0; i < TRIPLE_TURTLES_IN_LINE; i++) {
            tripleTurtles[i].x -= TRIPLE_TURTLES_SPEED;
            if ((frog->y == tripleTurtles[i].y) && (abs(frog->x - tripleTurtles[i].x) < (TRIPLE_TURTLES_WIDTH) / 2)) {
                *quit = 0;                                                                                                                                              //cofniecie wyjscia jesli zaba jest na ktorejs z grup zolwi
                frog->x -= TRIPLE_TURTLES_SPEED;
            }
            if (tripleTurtles[i].x < -SCREEN_WIDTH + TRIPLE_TURTLES_WIDTH) tripleTurtles[i].x = SCREEN_WIDTH + TRIPLE_TURTLES_WIDTH;
        }

        for (int i = 0; i < DOUBLE_TURTLES_IN_LINE; i++) {
            doubleTurtles[i].x -= DOUBLE_TURTLES_SPEED;
            if ((frog->y == doubleTurtles[i].y) && (abs(frog->x - doubleTurtles[i].x) < (DOUBLE_TURTLES_WIDTH) / 2)) {
                *quit = 0;
                frog->x -= DOUBLE_TURTLES_SPEED;
            }

            if (doubleTurtles[i].x < -SCREEN_WIDTH + DOUBLE_TURTLES_WIDTH) doubleTurtles[i].x = SCREEN_WIDTH + DOUBLE_TURTLES_WIDTH;
        }
    }
}

void showMenu(SDL_Surface* menu, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit) {
    SDL_Event event;
    DrawSurface(screen, menu, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);                                                 //menu
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_1) *quit = 0;
            else if (event.key.keysym.sym == SDLK_2) *quit = 2;
            break;
        }
    }
}

void gameOver(SDL_Surface* over, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, int* frogsOnLifes, int* score) {
    SDL_Event event;
    DrawSurface(screen, over, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);                                                 //menu po przegranej
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) *quit = 2;
            else if (event.key.keysym.sym == SDLK_1) {
                *quit = 0;
                *frogsOnLifes = 0;
                *score = 0;
            }
            break;
        }
    }
}
void leave(SDL_Surface* quitImg, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* quit, int* win, int* frogsOnLifes, int* score) {
    SDL_Event event;
    DrawSurface(screen, quitImg, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);                                                     //wyjscie zapytanie
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_y) {
                *quit = 3;
                *win = 0;
                *frogsOnLifes = 0;
                *score = 0;
            }
            else if (event.key.keysym.sym == SDLK_n) *quit = 0;
            break;
        }
    }
}

void frogMovement(Character* frog, int* score, int* pause, int* quit, int timer, int* maxY, int* bonus, int* frogsOnLifes) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) *quit = 2;                                     //opcje podczas gry
            else if (event.key.keysym.sym == SDLK_UP) {
                frog->y -= 50;
                if (frog->y < *maxY) {

                    if (frog->y == SCREEN_HEIGHT - 625) {                                            //ustalanie wyniku 
                        bonus[*frogsOnLifes] = (TIME - timer) * 10;
                        *score += 10 + 50 + *bonus;
                        *maxY = 0;
                    }
                    else if (frog->y >= SCREEN_HEIGHT - 625) *score += 10;

                    *maxY = frog->y;
                }
            }
            else if (event.key.keysym.sym == SDLK_DOWN) frog->y += 50;
            else if (event.key.keysym.sym == SDLK_RIGHT) frog->x += 50;
            else if (event.key.keysym.sym == SDLK_LEFT) frog->x -= 50;
            else if (event.key.keysym.sym == SDLK_p) *pause = 1;
            else if (event.key.keysym.sym == SDLK_q) *quit = 4;
            break;

        case SDL_QUIT:
            *quit = 2;
            break;
        }
    }
}

void showPause(SDL_Surface* pauseImg, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int* pause) {
    SDL_Event event;
    DrawSurface(screen, pauseImg, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);                                                    //pauza
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_p) *pause = 0;
            break;
        }
    }
}

void drawAll(Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* leftCars, Character* rightCars, Character* frog, SDL_Surface* screen) {


    for (int i = 0; i < 3 * LOGS_IN_LINE; i++) {
        DrawSurface(screen, logs[i].graphic, logs[i].x, logs[i].y);
    }

    for (int i = 0; i < TRIPLE_TURTLES_IN_LINE; i++) {
        DrawSurface(screen, tripleTurtles[i].graphic, tripleTurtles[i].x, tripleTurtles[i].y);
    }
    for (int i = 0; i < DOUBLE_TURTLES_IN_LINE; i++) {
        DrawSurface(screen, doubleTurtles[i].graphic, doubleTurtles[i].x, doubleTurtles[i].y);                          //rysowanie planszy i obiektow
    }

    DrawSurface(screen, frog->graphic, frog->x, frog->y);

    for (int i = 0; i < 3 * CAR_NUMBER / 5; i++) {
        DrawSurface(screen, leftCars[i].graphic, leftCars[i].x, leftCars[i].y);
    }
    for (int i = 0; i < 2 * CAR_NUMBER / 5; i++) {
        DrawSurface(screen, rightCars[i].graphic, rightCars[i].x, rightCars[i].y);
    }
}

int loadTextures(SDL_Surface** charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer,
    SDL_Surface** board, SDL_Surface** over, SDL_Surface** pauseImg, SDL_Surface** menu, Character* frog, SDL_Surface** quitImg,
    Character* leftCars, Character* rightCars, Character* logs, Character* doubleTurtles, Character* tripleTurtles, Character* frogs) {
    *charset = SDL_LoadBMP("../frogger/graphics/letters.bmp");                                                                                       //ladowanie tekstur
    if (*charset == NULL) {
        printf("SDL_LoadBMP(letters.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    *board = SDL_LoadBMP("../frogger/graphics/plansza.bmp");
    if (*board == NULL) {
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    *over = SDL_LoadBMP("../frogger/graphics/over.bmp");
    if (*over == NULL) {
        printf("SDL_LoadBMP(over.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    *pauseImg = SDL_LoadBMP("../frogger/graphics/pause.bmp");
    if (*pauseImg == NULL) {
        printf("SDL_LoadBMP(pause.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    *menu = SDL_LoadBMP("../frogger/graphics/menu.bmp");
    if (*menu == NULL) {
        printf("SDL_LoadBMP(menu.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    frog->graphic = SDL_LoadBMP("../frogger/graphics/frogger.bmp");
    if (frog->graphic == NULL) {
        printf("SDL_LoadBMP(frogger.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    *quitImg = SDL_LoadBMP("../frogger/graphics/quit.bmp");
    if (*quitImg == NULL) {
        printf("SDL_LoadBMP(quit.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    if (leftCars->graphic == NULL) {
        printf("SDL_LoadBMP(redcarleft.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    if (rightCars->graphic == NULL) {
        printf("SDL_LoadBMP(redcarright.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    if (logs->graphic == NULL) {
        printf("SDL_LoadBMP(log.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    if (doubleTurtles->graphic == NULL) {
        printf("SDL_LoadBMP(2turtles.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    if (tripleTurtles->graphic == NULL) {
        printf("SDL_LoadBMP(3turtles.bmp) error: %s\n", SDL_GetError());
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    for (int i = 0; i < 5; i++) {
        frogs[i].graphic = SDL_LoadBMP("../frogger/graphics/frogger.bmp");
    }
}