#include "raylib.h"

#define SNAKE_LENGTH 256

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int cellSize = 20;

    InitWindow(screenWidth, screenHeight, "Snake Game");

    Vector2 snake[SNAKE_LENGTH] = {0};
    Vector2 food = {0};
    int snakeLength = 1;
    int score = 0;
    int snakeSpeed = 6;
    int counterTail = 0;
    int framesCounter = 0;
    int currentDirection = KEY_RIGHT;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (!IsPaused()) {
            framesCounter++;

            if (framesCounter >= (60 / snakeSpeed)) {
                framesCounter = 0;

                for (int i = snakeLength; i > 0; i--) {
                    snake[i].x = snake[i - 1].x;
                    snake[i].y = snake[i - 1].y;
                }

                if (currentDirection == KEY_RIGHT) snake[0].x += cellSize;
                if (currentDirection == KEY_LEFT) snake[0].x -= cellSize;
                if (currentDirection == KEY_UP) snake[0].y -= cellSize;
                if (currentDirection == KEY_DOWN) snake[0].y += cellSize;

                if ((snake[0].x) > (screenWidth - cellSize)) snake[0].x = 0;
                if ((snake[0].x) < 0) snake[0].x = screenWidth - cellSize;
                if ((snake[0].y) > (screenHeight - cellSize)) snake[0].y = 0;
                if ((snake[0].y) < 0) snake[0].y = screenHeight - cellSize;

                for (int i = 1; i < snakeLength; i++) {
                    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                        snakeLength = 1;
                        score = 0;
                    }
                }

                if ((snake[0].x) == food.x && (snake[0].y) == food.y) {
                    food.x = (GetRandomValue(0, (screenWidth / cellSize) - 1) * cellSize);
                    food.y = (GetRandomValue(0, (screenHeight / cellSize) - 1) * cellSize);
                    snakeLength++;
                    score++;
                    framesCounter = 0;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < (screenWidth / cellSize); i++) {
            for (int j = 0; j < (screenHeight / cellSize); j++) {
                if ((i + j) % 2 == 0) {
                    DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, DARKGRAY);
                } else {
                    DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, GRAY);
                }
            }
        }

        for (int i = 0; i < snakeLength; i++) {
            DrawRectangle(snake[i].x, snake[i].y, cellSize, cellSize, DARKGREEN);
        }

        DrawRectangle(food.x, food.y, cellSize, cellSize, RED);
        
        DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
