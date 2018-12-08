#ifndef ATLAS_SCENE_FRAMEINFO_HPP
#define ATLAS_SCENE_FRAMEINFO_HPP

namespace atlas::scene
{
    struct FrameInfo
    {
        
        double elapsed;

        // elapsed seconds since last frame
        double deltaTime;

        // the current frame index
        size_t frame;
    };
}

#endif // ATLAS_SCENE_FRAMEINFO_HPP