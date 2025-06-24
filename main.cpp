#include <raylib.h>
#include <string>
#include <vector>
#include <raymath.h>

#include <sstream>
#include <iostream>
#include <cmath>
// Assuming you have this struct:
float speed = 400;

Color rgbatocolor(const std::string &rgba, int al) {
    std::vector<int> values;

    // Extract the inside of rgba( ... )
    size_t start = rgba.find('(');
    size_t end = rgba.find(')');
    Color c = {0, 0, 0, 255};
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return c; // Fallback for malformed input

    const std::string numbers = rgba.substr(start + 1, end - start - 1);
    std::stringstream ss(numbers);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            float value = std::stof(token);
            // If it's the alpha (usually the 4th value), scale to 255
            if (values.size() == 3) {
                values.push_back(static_cast<int>(value * 255));
            } else {
                values.push_back(static_cast<int>(value));
            }
        } catch (...) {
            return {0, 0, 0, 255}; // fallback
        }
    }

    while (values.size() < 4) values.push_back(al); // default alpha

    c.r = values[0];
    c.g = values[1];
    c.b = values[2];
    c.a = values[3];
    return c;
}

class Chain {
public:
    std::pmr::vector<Vector2> positions;
    std::pmr::vector<int> radii{30, 50, 40, 30, 20, 10, 5};

    Vector2 position = {100, 100};
    Vector2 position2 = {50, 100};
    Vector2 velocity{0, 0};


    Chain() {
        positions.push_back({100, 100});
        for (int i = 1; i < radii.size(); i++) {
            positions.push_back({positions[i - 1].x - 50, positions[i - 1].y});
        }
    }

    void update(float delta_time) {
        // if (IsKeyDown(KEY_RIGHT)) {
        //     positions[0].x += speed * delta_time;
        // }if (IsKeyDown(KEY_LEFT)) {
        //     positions[0].x -= speed * delta_time;
        // }
        // if (IsKeyDown(KEY_DOWN)) {
        //     positions[0].y += speed * delta_time;
        // }
        // if (IsKeyDown(KEY_UP)) {
        //     positions[0].y -= speed * delta_time;
        // }
        Vector2 mousePosition = GetMousePosition();
        Vector2 direction = Vector2Subtract(mousePosition, positions[0]);

        Vector2 normalizedDir = Vector2Normalize(direction);
        //Vector2 circleNormal = Vector2Normalize(Vector2Subtract(positions[0], positions[1]));
        Vector2 v1 = Vector2Subtract(positions[0], positions[1]);
        Vector2 v2 = Vector2Subtract(positions[1], positions[2]);
        float angle = Vector2Angle(v1, v2);
        Vector2 vel = normalizedDir; // your current velocity

// Rotate v2 90 degrees to get a perpendicular direction
        Vector2 v2_perp = Vector2Normalize((Vector2){ -v2.y, v2.x });

        // Project velocity onto v2_perp (dot product gives magnitude in perp dir)
        float perp_component = Vector2DotProduct(vel, v2_perp);

        // Final velocity is only in perpendicular direction
        Vector2 perp_velocity = Vector2Scale(v2_perp, perp_component);

        if (angle > PI / 2) {
            velocity = perp_velocity;

        };
        if (angle < -PI / 2) {
            velocity = perp_velocity;

        };
        float distnaceFromMouse = Vector2Distance(mousePosition, positions[0]);
        if (distnaceFromMouse > 200) {
            velocity = Vector2Scale(normalizedDir, speed);
            //positions[0] += velocity * delta_time*.3;
        } else {
            if (Vector2Length(velocity) > 1.0f) {
                Vector2 decelDir = Vector2Normalize(velocity);
                velocity = Vector2Subtract(velocity, Vector2Scale(decelDir, 300 * delta_time));
            } else {
                velocity = (Vector2){0, 0};
            }
        }

        positions[0] += velocity * delta_time;


        for (int i = 1; i < positions.size(); i++) {
            Vector2 directionChild = Vector2Subtract(positions[i - 1], positions[i]);
            Vector2 normalizedDirChild = Vector2Normalize(directionChild);
            Vector2 velocityChild = Vector2Scale(normalizedDirChild, speed);
            if (Vector2Distance(positions[i - 1], positions[i]) > 40) {
                positions[i] += velocityChild * delta_time;
            }
        }
    }


    void draw() const {
        for (int i = 0; i < positions.size(); i++) {
            DrawCircle(positions[i].x, positions[i].y, radii[i], rgbatocolor("rgb(255, 227, 187))", 255));
            if (i < positions.size() - 1) {
                Vector2 normal = Vector2Subtract(positions[i], positions[i + 1]);
                float angle = Vector2Angle(normal, {1, 0});
                float x = radii[i] * cosf(-angle - PI / 2);
                float y = radii[i] * sin(-angle - PI / 2);
                DrawCircle(x + positions[i].x, y + positions[i].y, 3, RED);
                x = radii[i] * cosf(-angle + PI / 2);
                y = radii[i] * sin(-angle + PI / 2);
                DrawCircle(x + positions[i].x, y + positions[i].y, 3, RED);
            } else {
                Vector2 normal = Vector2Subtract(positions[i], positions[i - 1]);
                float angle = Vector2Angle(normal, {1, 0});
                float x = radii[i] * cosf(-angle - PI / 2);
                float y = radii[i] * sin(-angle - PI / 2);
                DrawCircle(x + positions[i].x, y + positions[i].y, 3, RED);
                x = radii[i] * cosf(-angle + PI / 2);
                y = radii[i] * sin(-angle + PI / 2);
                DrawCircle(x + positions[i].x, y + positions[i].y, 3, RED);
            }

            // BeginBlendMode(BLEND_MULTIPLIED);
            // DrawCircle(positions[i].x, positions[i].y+13,radii[i],DARKBLUE);
            // EndBlendMode();
        }
        Vector2 normal = Vector2Subtract(positions[0], positions[1]);
        float angle = Vector2Angle(normal, {1, 0});
        float x = radii[0] * cosf(-angle - PI / 2);
        float y = radii[0] * sin(-angle - PI / 2);
        DrawCircle(x + positions[0].x, y + positions[0].y, 3, RED);
        x = radii[0] * cosf(-angle + PI / 2);
        y = radii[0] * sin(-angle + PI / 2);
        DrawCircle(x + positions[0].x, y + positions[0].y, 3, RED);
    }
};


int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1700, 900, "Procedural Animation");

    Chain c;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        c.update(dt);

        BeginDrawing();
        DrawFPS(10, 01);

        c.draw();


        ClearBackground(rgbatocolor("rgb(3, 166, 161))", 250));

        EndDrawing();
    }
}
