#include <iostream>
#include "rendering.hpp"
#include "raymath.h"
#include "planets.hpp"
#include "general_relativity.hpp"
#include "rlgl.h"

int WIDTH = 0, HEIGHT = 0;

int main() {
    Particles planets = initSunEarth();
    //updateParticlesEinstein(planets, 0.01_day);
    
    InitWindow(0, 0, "2D softbodies");
    ToggleFullscreen();
    WIDTH = GetScreenWidth(), HEIGHT = GetScreenHeight();

    Texture2D background = LoadTexture("resources/milky_way.png");
    Texture2D sunTexture = LoadTexture("resources/sun_texture.png");
    Texture2D earthTexture = LoadTexture("resources/earth_texture.png");

    Model skysphere = LoadModelFromMesh(GenMeshSphere(100.0f, 32, 32));
    Model sunModel = LoadModelFromMesh(GenMeshSphere(25.0f, 30, 30));
    Model earthModel = LoadModelFromMesh(GenMeshSphere(0.25f, 10, 10));

    skysphere.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = background;
    sunModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = sunTexture;
    earthModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = earthTexture;

    sunModel.transform = MatrixRotateX(PI/2.0f);
    earthModel.transform = MatrixRotateY(PI / 2.0f);

    SetTargetFPS(60);
    DisableCursor();

    Camera camera = { 0 };
    camera.position = Vector3{ 0.0f, 200.0f, -150.0f };
    camera.target = Vector3{ 0.0f, 0.0f, -25.0f }; 
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float days = 0;

    constexpr double dDay = 0.5;

    while (!WindowShouldClose()) {
        updateParticlesEinstein(planets, dDay * 1.0_day);
        days += dDay;
        sunModel.transform *= MatrixRotateY(0.22 * dDay);
        earthModel.transform *= MatrixRotateY(2*PI*dDay);
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
        ClearBackground(WHITE);
        //DrawTexturePro(background, { 0,0, 2048, 1024 }, {0, 0, (float)WIDTH, (float)HEIGHT}, { 0,0 }, 0.0f, WHITE);
            BeginMode3D(camera);

                rlDisableDepthMask();
                rlDisableBackfaceCulling();

                DrawModel(skysphere, camera.position, 1.0f, WHITE);

                rlEnableDepthMask();
                rlEnableBackfaceCulling();

                drawSpatialGrid(300, 75, planets);
                DrawModel(sunModel, toRenderVector(planets.positions[0]), 1.0f, WHITE);
                DrawModel(earthModel, toRenderVector(planets.positions[1]), 1.0f, WHITE);
            EndMode3D();
            DrawText(TextFormat("position : (%f, %f, %f)", camera.position.x, camera.position.y, camera.position.z), WIDTH-1200, 0, 50, WHITE);
            DrawText(TextFormat("time elapsed: %f days", days), WIDTH - 1200, 50, 50, WHITE);
            DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadModel(skysphere);
    UnloadModel(sunModel);
    UnloadModel(earthModel);
    UnloadTexture(background);
    UnloadTexture(sunTexture);
    UnloadTexture(earthTexture);
    CloseWindow();
}
