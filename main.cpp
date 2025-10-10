#include <raylib.h>
#include <bits/stdc++.h>
#include <ctime>
#include <cstdlib>
#include <chrono>


using namespace std;

int randint(int min, int max){
    return min + (rand() % (max - min + 1));
}

float get_distance(Vector2 v1, Vector2 v2){
    return sqrt((v1.x-v2.x) * (v1.x-v2.x) + (v1.y-v2.y) * (v1.y-v2.y));
}

Vector2 get_midpoint(Vector2 v1, Vector2 v2){
    return {(v1.x+v2.x)/2 , (v1.y+v2.y)/2};
}

Vector2 subtract_vector(Vector2 v1, Vector2 v2){
    return {v1.x-v2.x, v1.y-v2.y};
}

void add_to_vector(Vector2 *v1, Vector2 v2, float prop){
    v1->x += v2.x * prop;
    v1->y += v2.y * prop;
}

Vector2 normalize(Vector2 v){
    float magnitude = get_distance(v, {0,0});
    return {v.x/magnitude, v.y/magnitude};
}

int main(){
    srand(time(0));

    InitWindow(800, 800, "asd");
    SetTargetFPS(60);

    Vector2 ball1 {400,400};
    Vector2 ball2 {300,300};
    Vector2 midPoint;
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    float cont;
    int points {0};
    auto start = std::chrono::high_resolution_clock::now();

    Vector2 apple {(float)randint(200, 600), (float)randint(200, 600)};
    float speed = 3;
    while (!WindowShouldClose()){
        if(IsKeyDown(KEY_W))ball1.y -= speed;
        if(IsKeyDown(KEY_S))ball1.y += speed;
        if(IsKeyDown(KEY_A))ball1.x -= speed;
        if(IsKeyDown(KEY_D))ball1.x += speed;

        if(IsKeyDown(KEY_UP))ball2.y -= speed;
        if(IsKeyDown(KEY_DOWN))ball2.y += speed;
        if(IsKeyDown(KEY_LEFT))ball2.x -= speed;
        if(IsKeyDown(KEY_RIGHT))ball2.x += speed;

        midPoint = get_midpoint(ball1,ball2);

        auto time = (std::chrono::high_resolution_clock::now() - start);
        double seconds = chrono::duration_cast<chrono::duration<double>>(time).count();
        char time_str[10];
        sprintf(time_str, "%.2f", seconds);

        if (get_distance(midPoint, apple) < 10){
            points++;
            apple = {(float)randint(200, 600), (float)randint(200, 600)};
        }
        if(apple.x > 800 || apple.x < 0 || apple.y > 800 || apple.y < 0){
            points--;
            apple = {(float)randint(200, 600), (float)randint(200, 600)};
        }

        speed = 3 + points*0.1;

        add_to_vector(&apple, normalize(subtract_vector(apple, ball1)), 1 + (100+points*5)/get_distance(apple, ball1));
        add_to_vector(&apple, normalize(subtract_vector(apple, ball2)), 1 + (100+points*5)/get_distance(apple, ball2));
        add_to_vector(&apple, normalize(subtract_vector(apple, midPoint)), (10+points)/get_distance(apple, midPoint));

        BeginDrawing();
        ClearBackground(BLACK);

        DrawLineEx(ball1, ball2, 5.0f, RED);

        DrawCircle(ball1.x, ball1.y, 20, WHITE);
        DrawCircle(ball2.x, ball2.y, 20, WHITE);


        DrawCircle(midPoint.x, midPoint.y, 10, GRAY);

        DrawCircle(apple.x, apple.y, 10, RED);

        DrawText(to_string(GetFPS()).c_str(), 30,30, 20, WHITE);
        DrawText(time_str, 700,30, 20, WHITE);
        DrawText(to_string(points).c_str(), 400, 30, 50, WHITE);
        EndDrawing();
        cont+=0.05;
    }

    CloseWindow();
    return 0;
}
