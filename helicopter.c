#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raymath.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define HELICOPTER_HEALTH 20
#define HELICOPTER_SPEED 120.0f
#define ANIMATION_DURATION 0.2f
#define DYING_DURATION 2.0f

typedef enum { IDLE, MOVING, ATTACKING, RETREATING, TAKING_COVER, DYING } AIState;
typedef enum { SHOOTER, MELEE, HELICOPTER } PoliceType;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    PoliceType police_type;
    int bullet_health;
    bool active;
    AIState ai_state;
    float animation_timer;
    Vector2 wander_target;
    float wander_timer;
} Entity;

Entity helicopter = {0};

float distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void init_helicopter() {
    srand((unsigned int)time(NULL));
    Vector2 heli_pos = {900, 360};
    helicopter.position = heli_pos;
    helicopter.velocity = (Vector2){0, 0};
    helicopter.police_type = HELICOPTER;
    helicopter.bullet_health = HELICOPTER_HEALTH;
    helicopter.active = true;
    helicopter.ai_state = MOVING;
    helicopter.animation_timer = 0;
    helicopter.wander_target = (Vector2){0, 0};
    helicopter.wander_timer = 0.0f;
}

void update_helicopter() {
    if (!helicopter.active) return;
    if (helicopter.animation_timer > 0) helicopter.animation_timer -= GetFrameTime();
    if (helicopter.ai_state == DYING) {
        helicopter.position.y += 200.0f * GetFrameTime();
        if (helicopter.animation_timer <= 0) {
            helicopter.active = false;
        }
        return;
    }
    if (helicopter.wander_timer <= 0 || distance(helicopter.position, helicopter.wander_target) < 20.0f) {
        helicopter.wander_target.x = 600 + (rand() % (SCREEN_WIDTH - 600));
        helicopter.wander_target.y = 50 + (rand() % (SCREEN_HEIGHT - 100));
        helicopter.wander_timer = 3.0f + ((float)rand() / RAND_MAX) * 4.0f;
    }
    helicopter.wander_timer -= GetFrameTime();
    Vector2 dir = Vector2Subtract(helicopter.wander_target, helicopter.position);
    if (Vector2Length(dir) > 0) {
        dir = Vector2Normalize(dir);
        helicopter.velocity = Vector2Scale(dir, HELICOPTER_SPEED);
    } else {
        helicopter.velocity = (Vector2){0, 0};
    }
    Vector2 new_pos = Vector2Add(helicopter.position, Vector2Scale(helicopter.velocity, GetFrameTime()));
    helicopter.position = new_pos;
    if (helicopter.position.x < 0) helicopter.position.x = 0;
    if (helicopter.position.x > SCREEN_WIDTH) helicopter.position.x = SCREEN_WIDTH;
    if (helicopter.position.y < 0) helicopter.position.y = 0;
    if (helicopter.position.y > SCREEN_HEIGHT) helicopter.position.y = SCREEN_HEIGHT;
}

void draw_health_bar(Vector2 pos, int health, int max_health, Color c) {
    float width = 20.0f;
    float height = 4.0f;
    float health_ratio = (float)health / max_health;
    DrawRectangle(pos.x - width / 2, pos.y - 15, width, height, BLACK);
    DrawRectangle(pos.x - width / 2, pos.y - 15, width * health_ratio, height, c);
}

void draw_helicopter() {
    if (helicopter.active) {
        float scale = 1.0f + 0.2f * (helicopter.animation_timer / ANIMATION_DURATION);
        Vector2 pos = helicopter.position;
        if (helicopter.ai_state == DYING) {
            for (int k = 0; k < 5; k++) {
                float offset_x = sinf(GetTime() * 10 + k) * 10.0f;
                float offset_y = cosf(GetTime() * 10 + k) * 10.0f;
                Vector2 exp_pos = {pos.x + offset_x, pos.y + offset_y};
                float exp_size = 15.0f * (1.0f - helicopter.animation_timer / DYING_DURATION);
                DrawCircleV(exp_pos, exp_size, ORANGE);
            }
        }
        DrawRectangleRounded((Rectangle){pos.x - 30, pos.y - 10, 60, 20}, 0.5, 10, BLUE);
        DrawRectangle(pos.x + 30, pos.y - 3, 25, 5, BLUE);
        float tail_angle = GetTime() * 720;
        Vector2 tail_center = {pos.x + 50, pos.y};
        for (int k = 0; k < 2; k++) {
            float a = tail_angle + k * 180;
            Vector2 end = {tail_center.x + cosf(a * DEG2RAD) * 7, tail_center.y + sinf(a * DEG2RAD) * 7};
            DrawLineV(tail_center, end, BLACK);
        }
        Vector2 rotor_center = {pos.x, pos.y};
        float rotor_angle = GetTime() * 360;
        DrawCircle(rotor_center.x, rotor_center.y, 4, GRAY);
        for (int k = 0; k < 4; k++) {
            float a = rotor_angle + k * 90;
            Vector2 end = {rotor_center.x + cosf(a * DEG2RAD) * 35, rotor_center.y + sinf(a * DEG2RAD) * 35};
            DrawLineEx(rotor_center, end, 3, BLACK);
        }
        DrawRectangle(pos.x - 28, pos.y - 8, 8, 8, YELLOW);
        DrawRectangle(pos.x - 28, pos.y + 3, 8, 8, YELLOW);
        if (helicopter.ai_state != DYING) {
            draw_health_bar((Vector2){pos.x, pos.y + 20}, helicopter.bullet_health, HELICOPTER_HEALTH, GREEN);
        }
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Helicopter Display");
    SetTargetFPS(60);
    init_helicopter();
    while (!WindowShouldClose()) {
        update_helicopter();
        BeginDrawing();
        ClearBackground(GRAY);
        draw_helicopter();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}