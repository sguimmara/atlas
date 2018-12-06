#ifndef ATLAS_SCENE_VIEWPORT_HPP
#define ATLAS_SCENE_VIEWPORT_HPP

namespace atlas::scene
{
    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;

        inline float aspect() const noexcept { return width / height; }
    };
}

#endif