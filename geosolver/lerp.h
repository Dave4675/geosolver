#ifndef LERP_H
#define LERP_H

#include "geosolver_global.h"
#include <vector>

namespace geo
{

class Lerp
{
public:
    virtual r64 operator() (r64 t) = 0;
};

class UniformLerp: public Lerp
{
public:
    UniformLerp(std::vector<r64> &&points, r64 t0, r64 dt);
    virtual r64 operator() (r64 t);

private:
    std::vector<r64> points;
    r64 t_start;
    r64 dt;
    r64 t_end;
};

class RandomLerp: public Lerp
{
public:
    RandomLerp(std::vector<r64> &&value, std::vector<r64> &&time);
    virtual r64 operator() (r64 t);

private:
    std::vector<r64> value;
    std::vector<r64> time;
    s32 last_index;
};

r64 lerp(r64 p1, r64 p2, r64 t);

}

#endif // LERP_H
