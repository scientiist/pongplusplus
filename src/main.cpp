#define OLC_PGE_APPLICATION

#include "../../tetris/src/olcPixelGameEngine.hpp"

#include <iostream>
#define Vector2 olc::vf2d
#pragma region Physics Functions

bool GetOverlapsAABB(const Vector2& aPos, const Vector2& aSize, const Vector2& bPos, const olc::vf2d& bSize)
{
    auto absDistX = std::abs(aPos.x - bPos.x);
    auto absDistY = std::abs(aPos.y - bPos.y);

    auto sumW = aSize.x + bSize.x;
    auto sumH = aSize.y + bSize.y;

    if (absDistY >= sumH || absDistX >= sumW)
        return false;
    return true;
}
olc::vf2d GetSeparationAABB(const olc::vf2d& aPos, const olc::vf2d& aSize, const olc::vf2d& bPos, const olc::vf2d& bSize) {
    auto distX = aPos.x - bPos.x;
    auto distY = aPos.y - bPos.y;

    auto absDistX = std::abs(distX);
    auto absDistY = std::abs(distY);

    auto sumW = aSize.x + bSize.x;
    auto sumH = aSize.y + bSize.y;


    auto sx = sumW - absDistX;
    auto sy = sumH - absDistY;

    if (sx > sy) {
        if (sy > 0)
            sx = 0;
    } else {
        if (sx > 0)
            sy = 0;
    }

    if (distX < 0)
        sx = -sx;

    if (distY < 0)
        sy = -sy;

    return {sx, sy};
}
olc::vf2d GetNormalAABB(const olc::vf2d& separation, const olc::vf2d& velocity)
{
    auto d = std::sqrt(separation.x * separation.x + separation.y * separation.y);

    auto nx = separation.x / d;
    auto ny = separation.y / d;

    auto ps = velocity.x * nx + velocity.y * ny;

    if (ps <= 0)
    {
        return {nx, ny};
    }
    return {0,0};
}

struct LineLineIntersectResult
{
public:
    bool intersect;
    olc::vf2d position;
    explicit LineLineIntersectResult(bool res)
    {
        intersect = res;
    }
    explicit LineLineIntersectResult(olc::vf2d& pos)
    {
        position = pos;
        intersect = true;
    }
};
LineLineIntersectResult GetLineIntersects(const olc::vf2d& a1, const olc::vf2d& a2, const olc::vf2d& b1, const olc::vf2d& b2)
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
public:
    bool intersect;
    olc::vf2d position;
    Face face;
    LineRectIntersectResult(bool intersect)
    {
        intersect = false;
        position = {0,0};
        face = F_TOP; // ?
    }
    LineRectIntersectResult(olc::vf2d pos, Face _face)
    {
        position = pos;
        face = _face;
        intersect = true;
    }
};

const LineRectIntersectResult GetLineRectIntersect(const olc::vf2d& segA, const olc::vf2d& segB, const olc::vf2d& rectPos, const olc::vf2d& rectSize)
{
    auto intersection = olc::vf2d(0,0);
    auto collidingFace = F_TOP;


    auto rect_topleft = rectPos - rectSize;
    auto rect_bottomright = rectPos + rectSize;
    auto rect_bottomleft = rectPos + olc::vf2d(-rectSize.x, rectSize.y);
    auto rect_topright = rectPos + olc::vf2d(rectSize.x, -rectSize.y);


    auto top_hits = GetLineIntersects(segA, segB, rect_topleft, rect_topright);
    auto bottom_hits = GetLineIntersects(segA, segB, rect_bottomleft, rect_bottomright);
    auto left_hits = GetLineIntersects(segA, segB, rect_topleft, rect_bottomleft);
    auto right_hits = GetLineIntersects(segA, segB, rect_topright, rect_bottomright);

    if (top_hits.intersect || bottom_hits.intersect || left_hits.intersect || right_hits.intersect)
    {
        intersection = segB;

        if (top_hits.intersect && (segA-top_hits.position).mag() < (segA-intersection).mag())
        {
            intersection = top_hits.position;
            collidingFace = F_TOP;
        }

        if (bottom_hits.intersect && (segA-bottom_hits.position).mag() < (segA-intersection).mag())
        {
            intersection = bottom_hits.position;
            collidingFace = F_BOTTOM;
        }

        if (left_hits.intersect && (segA-left_hits.position).mag() < (segA-intersection).mag())
        {
            intersection = left_hits.position;
            collidingFace = F_LEFT;
        }
        if (right_hits.intersect && (segA-right_hits.position).mag() < (segA-intersection).mag())
        {
            intersection = right_hits.position;
            collidingFace = F_RIGHT;
        }
        return LineRectIntersectResult(intersection, collidingFace);
    }
    return LineRectIntersectResult(false);
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
    int speedModifier = 1;
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
        velocity = {-30, 0};
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

enum PaddleSide {P_LEFT, P_RIGHT};

class Paddle : public Entity
{
    olc::PixelGameEngine *engine;
public:
    PaddleSide side;
    int score = 0;

    olc::vf2d position = {10, 128};
    olc::vf2d velocity = {0,0};

    olc::vi2d size = olc::vi2d(5, 15);

    Paddle()
    {

    }
    Paddle(olc::PixelGameEngine *engineRef, PaddleSide side)
    {
        engine = engineRef;
        this->side = side;
        auto bounds = engine->GetScreenSize();
        if (side == P_RIGHT)
        {
            position.x = bounds.x - 10;
        }
    }

    void Reset()
    {
        //score = 0;

    }

    void Update(float delta) override
    {

        this->position += this->velocity*delta;
        this->velocity *= (1-(delta*8));
    }
    void Draw() override
    {
        auto bounds = engine->GetScreenSize();
        // Draw Paddle

        engine->FillRect(position-size, size*2, olc::WHITE);

        olc::vi2d scoreCoords = {0,0};
        std::string scoreStr = std::to_string(score);
        // Draw Score
        if (side == P_RIGHT)
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
        rPaddle = Paddle(this,  P_RIGHT);
        lPaddle = Paddle(this, P_LEFT);
        ball = Ball(this);

        time = 0;
        sAppName = "Pong in C++ by Josh";


    }
public:
    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float delta) override
    {

        auto bounds = GetScreenSize();

        // TODO: Decouple logic and graphics
        // Logic
        ball.Update(delta);
        lPaddle.Update(delta);
        rPaddle.Update(delta);


        // Left paddle - Ball Collision
        auto ballSize = olc::vf2d(ball.radius, ball.radius);
        bool lpOverlaps = GetOverlapsAABB(lPaddle.position, lPaddle.size, ball.position, ballSize);

        if (lpOverlaps)
        {
            auto sep = GetSeparationAABB(lPaddle.position, lPaddle.size, ball.position, ballSize);
            auto norm = GetNormalAABB(sep, ball.velocity);
            ball.position -= sep;

            ball.velocity.x = -(ball.velocity.x*1.5);
            ball.velocity.y = -ball.velocity.y;
            ball.velocity.y += lPaddle.velocity.y;

        }

        // Right paddle - Ball Collision
        auto rpOverlaps = GetOverlapsAABB(rPaddle.position, rPaddle.size, ball.position, ballSize);
        if (rpOverlaps)
        {
            auto sep = GetSeparationAABB(rPaddle.position, rPaddle.size, ball.position, ballSize);
            auto norm = GetNormalAABB(sep, ball.velocity);

            ball.position -= sep;
            ball.velocity.x = -(ball.velocity.x*1.5);
            ball.velocity.y = -ball.velocity.y;
            ball.velocity.y += rPaddle.velocity.y;
        }

        // Boundary collides

        if (ball.position.y < 0 || ball.position.y > bounds.y)
        {
            ball.velocity.y = -ball.velocity.y;
        }

        if (ball.position.x < 0)
        {
            lPaddle.score +=1;
            ball.Reset();
            lPaddle.position.y = 128;
            rPaddle.position.y = 128;
        }

        if (ball.position.x > bounds.x) {
            rPaddle.score +=1;
            ball.Reset();
            lPaddle.position.y = 128;
            rPaddle.position.y = 128;
        }


        // Paddle Inputs
        if (this->GetKey(olc::Key::W).bHeld)
        {
            lPaddle.velocity.y += delta*300;
        }
        if (this->GetKey(olc::Key::S).bHeld)
        {
            lPaddle.velocity.y -= delta*300;
        }
        if (this->GetKey(olc::Key::UP).bHeld)
        {
            rPaddle.velocity.y -= delta*300;
        }
        if (this->GetKey(olc::Key::DOWN).bHeld)
        {
            rPaddle.velocity.y += delta*300;
        }

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
