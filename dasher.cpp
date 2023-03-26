#include "raylib.h"

// Animation type
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // Update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{   
    // window size
    int windowDimension[2];    
    windowDimension[0] = 512;
    windowDimension[1] = 380;

    // window init
    InitWindow(windowDimension[0], windowDimension[1], "Dapper Dasher!");
    
    // movement
    int velocity{0};
    // jump velocity (pixel/second)
    int jumpVel{-600}; 
    // acceleration due to gravity (pixel/second)/second
    int gravity{1'000};

    // NEBULA
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{3};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {   
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDimension[0] + i * 300;
        nebulae[i].pos.y =  windowDimension[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1/16;
    }
    // nebula velocity (pixel/second);
    int nebVel{-200};

    // finish line
    float finishLine{ nebulae[sizeOfNebulae - 1].pos.x};

    // SCARFY
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimension[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimension[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;

    // rectangle is in the air - prevent air jumping
    bool isInAir{false};

    // background
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    // midground
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    
    // foreground
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    // scarfy and nebulae collision detection
    bool isCollided{};

    SetTargetFPS(60); // frame per second

    // loop execute each frame
    while (!WindowShouldClose())
    {
        // delta time (time since last frame)
        const float dT {GetFrameTime()};

        // Start drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // background scroll (px/s)
        bgX -= 20 * dT;
        mgX -= 40 * dT;
        fgX -= 80 * dT;

        if(bgX <= -background.width * 2)
        {
            bgX = 0.0;
        }

        if(mgX <= -midground.width * 2)
        {
            mgX = 0.0;
        }

        if(fgX <= -foreground.width * 2)
        {
            fgX = 0.0;
        }

        // Draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // Draw the midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // Draw the foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);        
        
        if(isOnGround(scarfyData, windowDimension[1]))
        {
            // rectangle is on the ground
            velocity = 0;
            isInAir = false;
        }
        else 
        {
            // rectangle is in the air (apply gravity)
            velocity += gravity * dT;
            isInAir = true;
        }


        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel; 
        }
        
        // update scarfy position (jump)
        scarfyData.pos.y += velocity * dT;

        for ( int i = 0; i < sizeOfNebulae; i++)
        {
            // update the position of each nebula
            nebulae[i].pos.x += nebVel * dT;
        }

        // update finishline
        finishLine += nebVel * dT;

        if(!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for ( int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] =  updateAnimData(nebulae[i], dT, 7);
        }

        for( AnimData nebula: nebulae)
        {
            float padding{50};
            Rectangle nebRec{
                nebula.pos.x + padding,
                nebula.pos.y + padding,
                nebula.rec.width - 2 * padding,
                nebula.rec.height - 2 * padding
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            if(CheckCollisionRecs(nebRec, scarfyRec)) 
            {
                isCollided = true;
            }
        }

        if(isCollided)
        {
            DrawText("GAME OVER!", windowDimension[0] / 4, windowDimension[1] / 2, 41, WHITE);
        }
        else if(scarfyData.pos.x >= finishLine)
        {     
            DrawText("YOU WIN!", windowDimension[0] / 4, windowDimension[1] / 2, 41, WHITE);
        }
        else
        {
            for ( int i = 0; i < sizeOfNebulae; i++)
            {
                // Draw nebulae
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }        
        
            // Draw character
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // Stop drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}
