#ifndef DT25_TEXTURE_H
#define DT25_TEXTURE_H

#include <string>
#include <filesystem>
#include "../d_glob.h"
#include <vector>
#include "stb_image.h"

enum d_TextureType {
    D_TEXTURE_DIFFUSE,
    D_TEXTURE_SPECULAR
};

class d_Texture {
private:    
    std::filesystem::path d_filepath;

    int d_Width, d_Height, d_nCh;    
    short int d_data_format;

    d_TextureType d_Type;
public:
    d_Texture(const std::filesystem::path& _filepath, d_TextureType _texType);
    d_Texture() = default;
    ~d_Texture();

    GLuint d_texID = 0;

    inline d_TextureType d_getType() { return d_Type; }
    inline std::filesystem::path getPath() { return d_filepath;  }

    inline short int d_GetDataFormat() { return d_data_format; };
};


#endif
