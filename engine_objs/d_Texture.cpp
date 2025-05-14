#include "d_Texture.h"

d_Texture::d_Texture(const std::filesystem::path& _filepath, d_TextureType _texType): d_Type(_texType), d_filepath(_filepath) {
    if(_filepath.string() == "")
        return;

    stbi_set_flip_vertically_on_load(true);
    const unsigned char* d_Data = stbi_load(_filepath.string().c_str(), &d_Width, &d_Height, &d_nCh, 0);

    if(!d_Data) {
        d_Logger::warn("[STBIO:] Texture returned empty data.");       
    } else {
        d_Logger::info("Processing texture: '" + _filepath.string() + "'. ");

        glGenTextures(1, &d_texID);
        glBindTexture(GL_TEXTURE_2D, d_texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        d_data_format = (d_nCh == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, d_data_format, d_Width, d_Height, 0, d_data_format, GL_UNSIGNED_BYTE, d_Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free((void*)d_Data);
}

d_Texture::~d_Texture() {
    //glDeleteTextures(1, &d_texID);
    return;
}