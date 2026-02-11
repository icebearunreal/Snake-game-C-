#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 15
#define MAX_SNAKE 100

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

typedef struct {
    int x[MAX_SNAKE];
    int y[MAX_SNAKE];
    int length;
    int dir;
} Snake;

int foodX, foodY;
int gameOver = 0;

void spawn_food(Snake *s) {
    int valid = 0;
    while (!valid) {
        valid = 1;
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
        for (int i = 0; i < s->length; i++) {
            if (s->x[i] == foodX && s->y[i] == foodY) {
                valid = 0;
            }
        }
    }
}

void draw(Snake *s) {
    printf("\033[2J\033[H");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            if (x == foodX && y == foodY) {
                printf("O");
                printed = 1;
            }

            for (int i = 0; i < s->length; i++) {
                if (s->x[i] == x && s->y[i] == y) {
                    printf(i == 0 ? "@" : "o");
                    printed = 1;
                }
            }

            if (!printed) printf(".");
        }
        printf("\n");
    }

    printf("Score: %d\n", s->length - 1);
}

void update(Snake *s) {
    for (int i = s->length; i > 0; i--) {
        s->x[i] = s->x[i-1];
        s->y[i] = s->y[i-1];
    }

    if (s->dir == 0) s->y[0]--;
    if (s->dir == 1) s->x[0]++;
    if (s->dir == 2) s->y[0]++;
    if (s->dir == 3) s->x[0]--;

    if (s->x[0] < 0 || s->x[0] >= WIDTH || s->y[0] < 0 || s->y[0] >= HEIGHT)
        gameOver = 1;

    for (int i = 1; i < s->length; i++) {
        if (s->x[0] == s->x[i] && s->y[0] == s->y[i])
            gameOver = 1;
    }

    if (s->x[0] == foodX && s->y[0] == foodY) {
        if (s->length < MAX_SNAKE - 1)
            s->length++;
        spawn_food(s);
    }
}

void input(Snake *s) {
    char c;
    if (read(STDIN_FILENO, &c, 1) > 0) {
        if (c == 'w' && s->dir != 2) s->dir = 0;
        if (c == 'd' && s->dir != 3) s->dir = 1;
        if (c == 's' && s->dir != 0) s->dir = 2;
        if (c == 'a' && s->dir != 1) s->dir = 3;
        if (c == 'q') gameOver = 1;
    }
}

int main() {
    srand(time(NULL));
    enable_raw_mode();

    Snake snake;
    snake.length = 3;
    snake.dir = 1;

    for (int i = 0; i < snake.length; i++) {
        snake.x[i] = WIDTH / 2 - i;
        snake.y[i] = HEIGHT / 2;
    }

    spawn_food(&snake);

    while (!gameOver) {
        draw(&snake);
        input(&snake);
        update(&snake);
        usleep(100000);
    }

    printf("Game Over. Final Score: %d\n", snake.length - 1);
    return 0;
}