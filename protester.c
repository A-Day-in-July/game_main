
#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raymath.h>    

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define ENTITY_SPEED 100.0f
#define PROTESTER_HEALTH 5
#define ANIMATION_DURATION 0.2f
#define COVER_WIDTH 10.0f
#define COVER_HEIGHT 80.0f
#define BULLET_SPEED 1400.0f
#define BULLET_RANGE 700.0f
#define PROTESTER_SHOOT_COUNTDOWN 0.4f
#define MAX_PROJECTILES 100

typedef struct {
    Vector2 position;
    Vector2 velocity;
    int bullet_health;
    bool active;
    float animation_timer;
    float morale_boost;
    float cooldown;
} Entity;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float distance_traveled;
} Projectile;

Entity protester = {0};
Projectile projectiles[MAX_PROJECTILES] = {0};

float distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void init_protester() {
    srand((unsigned int)time(NULL));
    Vector2 pos = {680 + (rand() % 500), 50 + (rand() % 620)};
    protester.position = pos;
    protester.velocity = (Vector2){0, 0};
    protester.bullet_health = PROTESTER_HEALTH;
    protester.active = true;
    protester.animation_timer = 0;
    protester.morale_boost = 1.0f;
    protester.cooldown = 0;
}

void fire_projectile(Vector2 pos, Vector2 dir, Entity *entity) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].position = pos;
            projectiles[i].velocity = Vector2Scale(Vector2Normalize(dir), BULLET_SPEED);
            projectiles[i].active = true;
            projectiles[i].distance_traveled = 0.0f;
            entity->animation_timer = ANIMATION_DURATION;
            break;
        }
    }
}

void update_protester() {
    if (!protester.active) return;
    if (protester.animation_timer > 0) protester.animation_timer -= GetFrameTime();
    if (protester.cooldown > 0) protester.cooldown -= GetFrameTime();
    
    static Vector2 wander_target = {0, 0};
    static float wander_timer = 0.0f;
    if (wander_timer <= 0 || distance(protester.position, wander_target) < 20.0f) {
        wander_target.x = 600 + (rand() % (SCREEN_WIDTH - 600));
        wander_target.y = 50 + (rand() % (SCREEN_HEIGHT - 100));
        wander_timer = 3.0f + ((float)rand() / RAND_MAX) * 4.0f;
    }
    wander_timer -= GetFrameTime();
    
    Vector2 dir = Vector2Subtract(wander_target, protester.position);
    if (Vector2Length(dir) > 0) {
        dir = Vector2Normalize(dir);
        protester.velocity = Vector2Scale(dir, ENTITY_SPEED * protester.morale_boost);
    } else {
        protester.velocity = (Vector2){0, 0};
    }
    
    Vector2 new_pos = Vector2Add(protester.position, Vector2Scale(protester.velocity, GetFrameTime()));
    
    if (new_pos.x < COVER_WIDTH) new_pos.x = COVER_WIDTH;
    if (new_pos.x > SCREEN_WIDTH - COVER_WIDTH) new_pos.x = SCREEN_WIDTH - COVER_WIDTH;
    if (new_pos.y < COVER_HEIGHT / 2) new_pos.y = COVER_HEIGHT / 2;
    if (new_pos.y > SCREEN_HEIGHT - COVER_HEIGHT / 2) new_pos.y = SCREEN_HEIGHT - COVER_HEIGHT / 2;
    
    protester.position = new_pos;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && protester.cooldown <= 0) {
        Vector2 mouse_pos = GetMousePosition();
        Vector2 shoot_dir = Vector2Subtract(mouse_pos, protester.position);
        if (Vector2Length(shoot_dir) > 0) {
            fire_projectile(protester.position, shoot_dir, &protester);
            protester.cooldown = PROTESTER_SHOOT_COUNTDOWN;
        }
    }
}

void update_projectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].position.x += projectiles[i].velocity.x * GetFrameTime();
            projectiles[i].position.y += projectiles[i].velocity.y * GetFrameTime();
            projectiles[i].distance_traveled += Vector2Length(projectiles[i].velocity) * GetFrameTime();
            if (projectiles[i].distance_traveled > BULLET_RANGE) {
                projectiles[i].active = false;
            }
        }
    }
}

void draw_health_bar(Vector2 pos, int health, int max_health, Color c) {
    float width = 20.0f;
    float height = 4.0f;
    float health_ratio = (float)health / max_health;
    DrawRectangle(pos.x - width / 2, pos.y - 15, width, height, BLACK);
    DrawRectangle(pos.x - width / 2, pos.y - 15, width * health_ratio, height, c);
}

void draw_protester() {
    if (protester.active) {
        float scale = 1.0f + 0.2f * (protester.animation_timer / ANIMATION_DURATION);
        DrawCircleV(protester.position, 10.0f * scale, RED);
        draw_health_bar(protester.position, protester.bullet_health, PROTESTER_HEALTH, GREEN);
    }
}

void draw_projectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            DrawCircleV(projectiles[i].position, 3.0f, WHITE);
        }
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Protester Shooting Display");
    SetTargetFPS(60);
    init_protester();
    while (!WindowShouldClose()) {
        update_protester();
        update_projectiles();
        BeginDrawing();
        ClearBackground(GRAY);
        draw_protester();
        draw_projectiles();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}