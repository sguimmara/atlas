#ifndef ATLAS_RENDERER_TEXTURE_HPP
#define ATLAS_RENDERER_TEXTURE_HPP

#include "common.hpp"

namespace atlas::renderer
{
    class Texture
    {
    public:
        Texture();
        Texture(const std::string& filename);
        Texture(uint32_t width, uint32_t height, void* rgba);
        Texture(Texture&& rhs);
        ~Texture();

        inline vk::ImageView view() const noexcept { return _view; }
        inline vk::Sampler sampler() const noexcept { return _sampler; }

    private:
        vk::ImageView _view;
        vk::Sampler _sampler;
        vk::Image _image;
        
        void create(int width, int height, const vk::Format&, void*);
    };
}

#endif // ATLAS_RENDERER_TEXTURE_HPP