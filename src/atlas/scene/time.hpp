#ifndef ATLAS_SCENE_TIME_HPP
#define ATLAS_SCENE_TIME_HPP

#include <chrono>

namespace atlas::scene
{
    class Time
    {
    public:
        Time();

        // elapsed seconds since the session start.
        std::chrono::milliseconds elapsed;

        // elapsed milliseconds since last frame
        std::chrono::milliseconds deltaTime;

        // the current frame index
        size_t frame;
    };
}

#endif // ATLAS_SCENE_TIME_HPP