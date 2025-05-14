#include "d_ImgExport.h"
#include "../d_glob.h"

#include "stb_image.h"
#include "stb_image_write.h"

void d_ImgExport::d_Capture(const std::string &_path) {
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint fbWidth = dims[2];
    GLint fbHeight = dims[3];

    stbi_flip_vertically_on_write(1);

    unsigned char* buffer = new unsigned char[ fbWidth * fbHeight * 3 ];
    glReadPixels( 0, 0, fbWidth, fbHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer );
    stbi_write_png(_path.c_str(), fbWidth, fbHeight, 3, buffer, fbWidth*3);
    if (stbi_failure_reason()) {
        d_Logger::err("Error saving a screenshot: " + std::string(stbi_failure_reason()));
    }

    delete[] buffer;
}
