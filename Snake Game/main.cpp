#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;


int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

Vector2 GenerateRandomCell()
{
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, GREEN);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{

public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class BonusFood
{
public:
    Vector2 position;
    Texture2D texture;
    bool active;
    double activationTime;
    double duration;

    BonusFood(deque<Vector2> snakeBody)
    {
        Image image = GenImageColor(16, 16, WHITE);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);

    active = false;
    activationTime = 0.0;
    duration = 5.0; 
    GenerateNewPosition(snakeBody);
    }

    ~BonusFood()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        if (active)
        {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }
    }

    void Activate(deque<Vector2> snakeBody)
    {
        active = true;
        activationTime = GetTime();
        GenerateNewPosition(snakeBody);
    }

    void Deactivate()
    {
        active = false;
    }

    void Update()
    {
        if (active && (GetTime() - activationTime >= duration))
        {
            active = false;
        }
    }

    void GenerateNewPosition(deque<Vector2> snakeBody)
    {
        Vector2 newPosition = GenerateRandomCell();
        while (ElementInDeque(newPosition, snakeBody))
        {
            newPosition = GenerateRandomCell();
        }
        position = newPosition;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    BonusFood bonusFood = BonusFood(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        bonusFood.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithBonusFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        bonusFood.Update();
        if (!bonusFood.active && EventTriggered(10.0)) 
        {
            bonusFood.Activate(snake.body);
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithBonusFood()
    {
        if (bonusFood.active && Vector2Equals(snake.body[0], bonusFood.position))
        {
            bonusFood.Deactivate();

        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    cout << "Starting Cans Snake Game " << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Cans Snake Game");
    SetTargetFPS(120);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (EventTriggered(0.1))
        {
            allowMove = true;
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
        {
            game.snake.direction = {0, -1};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
        {
            game.snake.direction = {0, 1};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
        {
            game.snake.direction = {1, 0};
            game.running = true;
            allowMove = false;
        }

        
        ClearBackground(BLACK);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, RED);
        DrawText("Cans Snake Game", offset - 5, 20, 40, RED);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, RED);
        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}