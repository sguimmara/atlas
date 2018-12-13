#ifndef ATLAS_RENDERER_VIEWPORT_HPP
#define ATLAS_RENDERER_VIEWPORT_HPP

namespace atlas::renderer
{
    // Viewport expressed in relative, normalized coordinates
    // (i.e a width of 1 represents 100% of the window's width)
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