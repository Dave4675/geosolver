#include "stdafx.h"
#include "lerp.h"
#include <assert.h>

namespace geo
{

UniformLerp::UniformLerp(std::vector<r64> &&points, r64 t0, r64 dt):
    points(points), t_start(t0), dt(dt)
{
    assert(dt > 0);
    t_end = t0 + (points.size()-1)*dt;
}

r64 UniformLerp::operator ()(r64 t)
{
    assert(t <= t_end);
    assert(t >= t_start);
    if(t == t_end)
    {
        return points.back();
    }

    s32 first_index = (s32)((t-t_start)/dt);
    r64 t_first = first_index*dt;
    t = (t - t_first - t_start)/dt;
    return points[first_index]*(1-t) + points[first_index+1]*t;
}

RandomLerp::RandomLerp(std::vector<r64> &&value, std::vector<r64> &&time):
    value(value), time(time), last_index(0)
{

}

r64 RandomLerp::operator ()(r64 t)
{
    assert(t >= time.front());
    assert(t <= time.back());

    while(t < time[last_index]) last_index--;
    while(t > time[last_index+1]) last_index++;

    r64 dt = time[last_index + 1] - time[last_index];
    r64 t_first = time[last_index];
    t = (t - t_first)/dt;
    return value[last_index]*(1-t) + value[last_index+1]*t;
}

r64 lerp(r64 p1, r64 p2, r64 t)
{
    return (1-t)*p1 + t*p2;
}

}

