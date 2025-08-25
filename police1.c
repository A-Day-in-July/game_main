#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include<raymath.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define ENTITY_SPEED 150.0f
#define POLICE_HEALTH 5
#define ANIMATION_DURATION 0.2f
#define COVER_WIDTH 10.0f
#define COVER_HEIGHT 80.0f

typedef struct {
    Vector2 position;
    Vector2 velocity;
    int bullet_health;
    bool active;
    float animation_timer;
    float morale_boost;
} Entity;

Entity police = {0};

float distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void init_police() {
    srand((unsigned int)time(NULL));
    Vector2 pos = {680 + (rand() % 500), 50 + (rand() % 620)};
    police.position = pos;
    police.velocity = (Vector2){0, 0};
    police.bullet_health = POLICE_HEALTH;
    police.active = true;
    police.animation_timer = 0;
    police.morale_boost = 1.0f;
}

void update_police() {
    if (!police.active) return;
    if (police.animation_timer > 0) police.animation_timer -= GetFrameTime();
    
    static Vector2 wander_target = {0, 0};
    static float wander_timer = 0.0f;
    if (wander_timer <= 0 || distance(police.position, wander_target) < 20.0f) {
        wander_target.x = 600 + (rand() % (SCREEN_WIDTH - 600));
        wander_target.y = 50 + (rand() % (SCREEN_HEIGHT - 100));
        wander_timer = 3.0f + ((float)rand() / RAND_MAX) * 4.0f;
    }
    wander_timer -= GetFrameTime();
    
    Vector2 dir = Vector2Subtract(wander_target, police.position);
    if (Vector2Length(dir) > 0) {
        dir = Vector2Normalize(dir);
        police.velocity = Vector2Scale(dir, ENTITY_SPEED * police.morale_boost);
    } else {
        police.velocity = (Vector2){0, 0};
    }
    
    Vector2 new_pos = Vector2Add(police.position, Vector2Scale(police.velocity, GetFrameTime()));
    
    if (new_pos.x < COVER_WIDTH) new_pos.x = COVER_WIDTH;
    if (new_pos.x > SCREEN_WIDTH - COVER_WIDTH) new_pos.x = SCREEN_WIDTH - COVER_WIDTH;
    if (new_pos.y < COVER_HEIGHT / 2) new_pos.y = COVER_HEIGHT / 2;
    if (new_pos.y > SCREEN_HEIGHT - COVER_HEIGHT / 2) new_pos.y = SCREEN_HEIGHT - COVER_HEIGHT / 2;
    
    police.position = new_pos;
}

void draw_health_bar(Vector2 pos, int health, int max_health, Color c) {
    float width = 20.0f;
    float height = 4.0f;
    float health_ratio = (float)health / max_health;
    DrawRectangle(pos.x - width / 2, pos.y - 15, width, height, BLACK);
    DrawRectangle(pos.x - width / 2, pos.y - 15, width * health_ratio, height, c);
}

void draw_police() {
    if (police.active) {
        float scale = 1.0f + 0.2f * (police.animation_timer / ANIMATION_DURATION);
        DrawRectangleV(Vector2Subtract(police.position, (Vector2){10.0f * scale, 10.0f * scale}), 
                       (Vector2){20.0f * scale, 20.0f * scale}, BLUE);
        draw_health_bar(police.position, police.bullet_health, POLICE_HEALTH, GREEN);
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Square Police Display");
    SetTargetFPS(60);
    init_police();
    while (!WindowShouldClose()) {
        update_police();
        BeginDrawing();
        ClearBackground(GRAY);
        draw_police();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}