//#include "Image.hpp"
//
//#include <stb/stb_image.h>
//
//namespace atlas
//{
//    namespace graphics
//    {
//        Image Image::FromFile(std::string path)
//        {
//            int width, height, channels;
//            const auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
//            if (!pixels)
//            {
//                throw std::runtime_error("failed to load texture image: " + path);
//            }
//
//            vk::DeviceSize size = width * height * 4;
//
//            return FromRGBA(pixels, width, height);
//        }
//
//        Image atlas::graphics::Image::FromRGBA(unsigned char * rgba32, int width, int height)
//        {
//            vk::Buffer stagingBuffer;
//            vk::DeviceMemory stagingBufferMemory;
//            // TODO
//            return Image();
//        }
//    }
//}