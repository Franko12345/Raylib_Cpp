#include <algorithm>
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

float lerp(float a, float b, float t){
    return a + (b-a) * t;
}

int global_cont {0};

struct Obj{
    Vector2 pos;
    Vector2 size;
    Color color;
    float speed;
    float points;

    Obj(Vector2 p, Vector2 s) : pos(p), size(s), color(WHITE), speed(0), points(1){}
    Obj(Vector2 p, Vector2 s, Color c) : pos(p), size(s), color(c), speed(0), points(1){}
    Obj(Vector2 p, Vector2 s, Color c, float sp) : pos(p), size(s), color(c), speed(sp), points(1){}
    Obj(Vector2 p, Vector2 s, Color c, float sp, float point) : pos(p), size(s), color(c), speed(sp), points(point){}
};

void reverse_arr(Vector2 arr[], int size, Vector2 *recipient){
    for (int i = size-1; i >= 0; i--){
        recipient[size-i-1] = arr[i];
    }
}

// struct Timer{
//     int startTime;
//     int duration;
//     void (*callback);

//     Timer() : startTime(-1), duration(-1), callback(){}
//     Timer(int d, void(*c)) : startTime(global_cont), duration(d), callback(c){}
// };

int main(){
    srand(time(0));

    InitWindow(800, 800, "Game");
    SetTargetFPS(60);

    Obj player = Obj({400,400}, {25,40}, WHITE, 4);

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    float cont {};
    int points {0};
    auto start = std::chrono::high_resolution_clock::now();

    vector<Obj> enemies = {};
    vector<Vector2> killPoint = {{(float)randint(200, 600), (float)randint(200, 600)}};

    int Kpoints {};

    float angle = 0;


    int dash_timer;
    int group_timer;

    bool kill_dash_active {false};
    bool grouping {false};
    const int DASH_TICKS {100};

    Vector2 grouping_points[110];
    int grouping_point_qty{0};

    while (!WindowShouldClose()){
        if(IsKeyDown(KEY_W))player.pos.y -= player.speed;
        if(IsKeyDown(KEY_S))player.pos.y += player.speed;
        if(IsKeyDown(KEY_A))player.pos.x -= player.speed;
        if(IsKeyDown(KEY_D))player.pos.x += player.speed;

        if(!kill_dash_active && !grouping && IsKeyPressed(KEY_LEFT_SHIFT) && Kpoints > 0){
            Kpoints--;
            kill_dash_active = true;
            player.speed += 3;
            dash_timer = global_cont;
            player.color = {255,255,255,125};
        }

        if(!grouping && !kill_dash_active && IsKeyPressed(KEY_SPACE) && Kpoints > 0){
            Kpoints--;
            player.speed += 3;
            grouping = true;
            group_timer = global_cont;
            player.color = {100,100,255,125};
            fill(begin(grouping_points), end(grouping_points), (Vector2){-1,-1});
            grouping_point_qty = 0;
        }

        if (kill_dash_active && global_cont-dash_timer > DASH_TICKS){
            player.speed -= 3;
            kill_dash_active = false;
            player.color = WHITE;
        }

        if (grouping && global_cont-group_timer > DASH_TICKS){
            player.speed -= 3;
            grouping = false;
            player.color = WHITE;
            float size {};
            float spd {};
            float points {};
            Vector2 posE;
            cout << "Enemies size before: " << enemies.size() << endl;
            for(int i = 0; i < enemies.size(); i++){
                auto enemy = enemies[i];
                Vector2 grouping_points_revrsed[110];
                reverse_arr(grouping_points, grouping_point_qty, grouping_points_revrsed);
                bool collision = CheckCollisionPointPoly(enemy.pos, grouping_points, grouping_point_qty) || CheckCollisionPointPoly(enemy.pos, grouping_points_revrsed, grouping_point_qty);
                if (collision){
                    cout << "Collided with enemy: " << i << endl;
                    enemies[i].color = YELLOW;
                    enemies[i].speed = 0;
                    size+=enemy.size.x;
                    spd+=enemy.speed/10;
                    posE=enemy.pos;
                    enemies.erase(enemies.begin()+i);
                    i--;
                }
            }
            size = 10 + 3 * sqrt(size);
            enemies.push_back(Obj(posE, {size, 0}, RED, spd));

            cout << "Enemies size before: " << enemies.size() << endl;
        }

        if(grouping){
            grouping_points[grouping_point_qty] = player.pos;
            grouping_point_qty++;
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S)){
            angle = lerp(angle, 15*cos((float)global_cont/5), 0.2);
        }else{
            angle = lerp(angle, 0, 0.1);
        }

        if (global_cont % 1000 == 0)
            killPoint.push_back({(float)randint(200, 600), (float)randint(200, 600)});

        if (cont > 5){
            cont = 0;
            Obj enemy = Obj({(float)randint(200, 600), (float)randint(200, 600)}, {10,0}, RED, 1);
            enemies.push_back(enemy);
        }

        for(int i = 0; i < killPoint.size(); i++){
            if (get_distance(killPoint[i], player.pos) < 25){
                killPoint.erase(killPoint.begin()+i);
                i++;
                Kpoints++;
            }
        }

        auto time = (std::chrono::high_resolution_clock::now() - start);
        double seconds = chrono::duration_cast<chrono::duration<double>>(time).count();
        char time_str[10];
        sprintf(time_str, "%.2f", seconds);

        for (int i = 0; i < enemies.size(); i++){
            auto enemy = enemies[i];
            add_to_vector(&enemies[i].pos, normalize(subtract_vector(player.pos, enemy.pos)), enemy.speed);
            if (kill_dash_active && get_distance(player.pos, enemy.pos) < min(player.size.x/2, player.size.y/2)+enemy.size.x){
                points+=pow(enemies[i].size.x/10, 2);
                enemies.erase(enemies.begin()+i);
            }
        }

        bool stable = false;
        while (!stable){
            stable = true;
            for (auto &enemy : enemies){
                for (auto &e : enemies){
                    if (&enemy == &e) continue;
                    float dst = get_distance(enemy.pos, e.pos);
                    float min_dst = enemy.size.x + e.size.x;
                    if (dst < min_dst - 0.01){
                        stable = false;
                        Vector2 offset = normalize(subtract_vector(enemy.pos, e.pos));
                        add_to_vector(&enemy.pos, offset, 0.5*(min_dst-dst));
                        add_to_vector(&e.pos, offset, -0.5*(min_dst-dst));
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (grouping_point_qty >= 4){
            DrawSplineLinear(grouping_points, grouping_point_qty, 5, {30,100,30,100});
            DrawTriangleFan(grouping_points, grouping_point_qty, {50,120,50,100});
            Vector2 grouping_points_revrsed[110];
            reverse_arr(grouping_points, grouping_point_qty, grouping_points_revrsed);
            // for (int i = grouping_point_qty-1; i >= 0; i--){
            //     grouping_points_revrsed[grouping_point_qty-i-1] = grouping_points[i];
            // }
            DrawTriangleFan(grouping_points_revrsed, grouping_point_qty, {50,120,50,100});
        }

        Vector2 grouping_points_revrsed[110];
        reverse_arr(grouping_points, grouping_point_qty, grouping_points_revrsed);



        //draw player
        Rectangle playerRectangle = {player.pos.x, player.pos.y, player.size.x,player.size.y};
        DrawRectanglePro(playerRectangle, {player.size.x/2, player.size.y/2}, angle, player.color);

        //draw killBalls
        for (auto &kp : killPoint)
            DrawCircleV(kp, 5, GREEN);

        //draw enemies
        for (auto &enemy : enemies)
            DrawCircle(enemy.pos.x, enemy.pos.y, enemy.size.x, enemy.color);

        //draw power bar
        for(float i = 0; i < Kpoints; i++)
            DrawRectangleV((Vector2){750-i*30, 750}, (Vector2){20,20} , GREEN);


        //DEBUG GROUPING AREA
        // for(int i = 1; i < grouping_point_qty-2; i+=4){
        //     DrawTriangle(grouping_points[0], grouping_points[i], grouping_points[i+2], RED);
        //     DrawTriangle(grouping_points_revrsed[0], grouping_points_revrsed[i], grouping_points_revrsed[i+2], BLUE);
        // }
        // for(int i = 0; i < grouping_point_qty; i++){
        //     DrawCircleV(grouping_points[i], 2, GREEN);
        // }

        DrawText(to_string(GetFPS()).c_str(), 30,30, 20, WHITE);
        DrawText(time_str, 700,30, 20, WHITE);
        DrawText(to_string(points).c_str(), 400, 30, 50, WHITE);
        EndDrawing();
        cont+=0.05;
        global_cont++;
    }

    CloseWindow();
    return 0;
}
