#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.hpp"

#include <iostream>

#pragma region Physics Functions

bool GetOverlapsAABB(olc::vf2d& aPos, olc::vf2d& aSize, olc::vf2d& bPos, olc::vf2d& bSize)
{

}
const olc::vf2d GetSeparationAABB(olc::vf2d& aPos, olc::vf2d aSize, olc::vf2d bPos, olc::vf2d bSize) {

}
const olc::vf2d GetNormalAABB(olc::vf2d& aPos, olc::vf2d& aSize, olc::vf2d& bPos, olc::vf2d& bSize)
{

}

struct LineLineIntersectResult
{
public:
    bool intersect;
    olc::vf2d position;
    LineLineIntersectResult(bool res)
    {
        intersect = res;
    }
    LineLineIntersectResult(olc::vf2d& pos)
    {
        position = pos;
        intersect = true;
    }
};
const LineLineIntersectResult GetLineIntersects(olc::vf2d& a1, olc::vf2d& a2, olc::vf2d& b1, olc::vf2d& b2)
{
    olc::vf2d intersection = {0,0};
    auto b = a2 - a1;
    auto d = b2 - b1;
    auto bDotDPerp = b.x * d.y - b.y * d.x;
    if (bDotDPerp == 0)
    {
        return LineLineIntersectResult(false);
    }

    auto c = b1 - a1;
    auto t = (c.x * d.y - c.y * d.x) / bDotDPerp;

    if (t < 0 || t > 1)
    {
        return LineLineIntersectResult(false);
    }

    auto u = (c.x * b.y - c.y * b.x) / bDotDPerp;

    if (u < 0 || u > 1)
    {
        return LineLineIntersectResult(false);
    }

    intersection = (a1 + t * b);
    return LineLineIntersectResult(intersection);
}
enum Face {F_LEFT, F_TOP, F_RIGHT, F_BOTTOM};
struct LineRectIntersectResult
{
    bool intersect;
    olc::vf2d position;
    Face face;
};

const LineRectIntersectResult GetLineRectIntersect(olc::vf2d& segA, olc::vf2d& segB, olc::vf2d& rectPos, olc::vf2d& rectSize)
{

}

#pragma endregion


class Entity
{
    olc::PixelGameEngine *engine;
public:
    Entity(){}
    Entity(olc::PixelGameEngine *engineRef)
    {
        engine = engineRef;
    }
    virtual void Update(float delta)
    {

    }
    virtual void Draw()
    {

    }
};


class Ball : public Entity
{
    olc::PixelGameEngine *engine;
public:
    olc::vf2d position = {128,128};
    olc::vf2d velocity = {-30,0};
    int radius = 4;
    Ball()
    {

    }
    Ball(olc::PixelGameEngine *engineRef)
    {
        engine = engineRef;
    }
    void Reset()
    {
        position = {128, 128};
    }
    void Update(float delta) override
    {
        //this->velocity *= delta;
        this->position += (this->velocity*delta);
    }
    void Draw() override
    {
        engine->FillCircle(position, radius, olc::RED);
    }
};

enum PaddleSide
{
    LEFT, RIGHT
};

class Paddle : public Entity
{
    olc::PixelGameEngine *engine;
public:
    PaddleSide side;
    int score = 0;

    int yposition = 128;
    olc::vi2d size = olc::vi2d(5, 15);

    Paddle()
    {

    }
    Paddle(olc::PixelGameEngine *engineRef, PaddleSide side)
    {
        engine = engineRef;
        this->side = side;
    }

    void Reset()
    {
        //score = 0;

    }

    void MoveUp(float d)
    {

    }
    void MoveDown(float d)
    {

    }

    void Update(float delta) override
    {

    }
    void Draw() override
    {
        auto bounds = engine->GetScreenSize();

        int xposition = 10;
        // Draw Paddle
        if (side == RIGHT)
        {
            xposition = bounds.x - 10;
        }
        olc::vi2d position = olc::vi2d(xposition, yposition);
        engine->FillRect(position-size, size*2, olc::WHITE);

        olc::vi2d scoreCoords = {0,0};
        std::string scoreStr = std::to_string(score);
        // Draw Score
        if (side == RIGHT)
        {
            auto textDims = engine->GetTextSize(scoreStr);
            scoreCoords = {bounds.x-(textDims.x*2), 0};
        }


        engine->DrawString(scoreCoords, scoreStr, olc::WHITE, 2);
    }
};






class Pong : public olc::PixelGameEngine
{
public:
    Ball ball;
    Paddle lPaddle;
    Paddle rPaddle;


    int time;

    Pong()
    {
        rPaddle = Paddle(this,  RIGHT);
        lPaddle = Paddle(this, LEFT);
        ball = Ball(this);


        time = 0;
        sAppName = "Example";

    }
public:
    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float delta) override
    {

        // TODO: Decouple logic and graphics
        // Logic
        ball.Update(delta);
        lPaddle.Update(delta);
        rPaddle.Update(delta);


        // Graphics
        Clear(olc::BLACK);
        ball.Draw();
        lPaddle.Draw();
        rPaddle.Draw();

        return true;
    }
};


int main() {
    std::cout << "Starting Pong" << std::endl;
    Pong gameInst;
    if (gameInst.Construct(256, 240, 2, 2))
        gameInst.Start();
    return 0;
}
