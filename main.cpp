#include <raylib.h>
#include <string>
#include <vector>
#include <raymath.h>

#include <sstream>
#include <iostream>
#include <cmath>
// Assuming you have this struct:
float speed = 400;
// void DrawCatmullRomCurve(Vector2 *points, int count, int segments) {
//     for (int i = 1; i < count - 2; i++) {
//         for (int j = 0; j < segments; j++) {
//             float t1 = (float)j / segments;
//             float t2 = (float)(j + 1) / segments;
//
//             Vector2 p1 = Vector2CatmullRom(points[i - 1], points[i], points[i + 1], points[i + 2], t1);
//             Vector2 p2 = Vector2CatmullRom(points[i - 1], points[i], points[i + 1], points[i + 2], t2);
//
//             DrawLineV(p1, p2, RED);
//         }
//     }
// }

void DrawRotatedEllipse(Vector2 center, float radiusH, float radiusV, float angleRad, Color color) {
    const int segments = 100;
    Vector2 points[segments + 1];

    for (int i = 0; i <= segments; i++) {
        float theta = 2 * PI * i / segments;

        // Ellipse point before rotation
        float x = radiusH * cosf(theta);
        float y = radiusV * sinf(theta);

        // Rotate the point
        float xr = x * cosf(angleRad) - y * sinf(angleRad);
        float yr = x * sinf(angleRad) + y * cosf(angleRad);

        // Translate to center
        points[i] = { center.x + xr, center.y + yr };
    }

    for (int i = 0; i < segments; i++) {
        DrawLineV(points[i], points[i + 1], color);
    }
}
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
    std::pmr::vector<int> radii{30, 50, 40, 30, 20, 15,10, 5};

    Vector2 position = {100, 100};
    Vector2 position2 = {50, 100};
    Vector2 velocity{0, 0};


    Chain() {
        positions.push_back({100, 100});
        for (int i = 1; i < radii.size(); i++) {
            if ( i>4) {
                positions.push_back({positions[i - 1].x - 20, positions[i - 1].y});
                continue;
            }
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
        Vector2 const mousePosition = GetMousePosition();
        Vector2 const direction = Vector2Subtract(mousePosition, positions[0]);

        Vector2 const normalizedDir = Vector2Normalize(direction);
        //Vector2 const circleNormal = Vector2Normalize(Vector2Subtract(positions[0], positions[1]));
        Vector2 const v1 = Vector2Subtract(positions[0], positions[1]);
        Vector2 const v2 = Vector2Subtract(positions[1], positions[2]);
        float const angle = Vector2Angle(v1, v2);
        Vector2 const vel = normalizedDir; // your current velocity

        // Rotate v2 90 degrees to get a perpendicular direction
        Vector2 const v2_perp = Vector2Normalize((Vector2){-v2.y, v2.x});

        // Project velocity onto v2_perp (dot product gives magnitude in perp dir)
        float const perp_component = Vector2DotProduct(vel, v2_perp);

        // Final velocity is only in perpendicular direction
        Vector2 const perp_velocity = Vector2Scale(v2_perp, perp_component);

        if (angle > PI / 2) {
            velocity = perp_velocity;
        };
        if (angle < -PI / 2) {
            velocity = perp_velocity;
        };
        if (float distanceFromMouse = Vector2Distance(mousePosition, positions[0]); distanceFromMouse > 200) {
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
        int num_points = radii.size() * 2 + 4;
        Vector2 points[num_points];
        int count = 0;
        int countLeft = radii.size() * 2+3;


        Vector2 normal = Vector2Subtract(positions[0], positions[0 + 1]);
        float angle = Vector2Angle(normal, {1, 0});
        float x = static_cast<float>(radii[0]) * cosf(-angle);
        float y = static_cast<float>(radii[0]) * sin(-angle);
        points[count++] = {positions[0].x +x, positions[0].y+y};
        points[countLeft--] = {positions[0].x +x, positions[0].y+y};

         x = static_cast<float>(radii[0]) * cosf(+
             -angle+PI/4);
         y = static_cast<float>(radii[0]) * sin(-angle+PI/4);
        points[count++] = {positions[0].x +x, positions[0].y+y};


        x = radii[0] * cosf(-angle - PI / 4);
        y = radii[0] * sin(-angle - PI / 4);
        points[countLeft--] = {positions[0].x +x, positions[0].y+y};
        std::cout << points[countLeft].y<<std::endl;


        //DrawCircle(static_cast<int>(positions[0].x), positions[0].y, static_cast<float>(radii[0]),
                       // rgbatocolor("rgb(255, 227, 187))", 255));
        for (int i = 0; i < positions.size(); i++) {
            // DrawCircle(static_cast<int>(positions[i].x), positions[i].y, static_cast<float>(radii[i]+5),
            //            rgbatocolor("rgb(255, 166, 115)", 255));
            if (i < positions.size() - 1) {
                normal = Vector2Subtract(positions[i], positions[i + 1]);
                angle = Vector2Angle(normal, {1, 0});
                x = static_cast<float>(radii[i]) * cosf(-angle - PI / 2);
                y = static_cast<float>(radii[i]) * sin(-angle - PI / 2);
                points[countLeft--] = {positions[i].x +x, positions[i].y+y};

                x = static_cast<float>(radii[i]) * cosf(-angle + PI / 2);
                y = static_cast<float>(radii[i]) * sin(-angle + PI / 2);
                points[count++] = {positions[i].x +x, positions[i].y+y};



            } else {
                normal = Vector2Subtract(positions[i-1], positions[i ]);
                angle = Vector2Angle(normal, {1, 0});
                x = cosf(-angle - PI / 2) * static_cast<float>(radii[i]);
                y = static_cast<float>(radii[i]) * sin(-angle - PI / 2);
               points[countLeft--] = {positions[i].x +x, positions[i].y+y};

                x = static_cast<float>(radii[i]) * cosf(-angle + PI / 2);
                y = static_cast<float>(radii[i]) * sin(-angle + PI / 2);
                points[count++] = {positions[i].x +x, positions[i].y+y};


            }



        }

        // for (Vector2 x: points) {
        //     DrawCircle(x.x,x.y,3,RED);
        // }
        DrawSplineCatmullRom(points,num_points,20,rgbatocolor("rgb(255, 227, 187)", 255));
        for (int i = 0; i < positions.size(); i++) {

            DrawCircle(static_cast<int>(positions[i].x), positions[i].y, static_cast<float>(radii[i]+5),
                       rgbatocolor("rgb(255, 166, 115)", 255));
        }
        DrawSplineCatmullRom(points,num_points,17,rgbatocolor("rgb(255, 166, 115)", 255));


        DrawSplineSegmentCatmullRom(points[num_points-2],points[num_points-1],points[0],points[1], 10,rgbatocolor("rgb(255, 79, 15)",255));
        //DrawEllipse(points[3].x ,points[3].y,10,20,RED);
        //DrawRotatedEllipse(points[3] ,points[3].y,10,20,RED);
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
