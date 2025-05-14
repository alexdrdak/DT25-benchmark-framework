#ifndef DT25_D_IMGEXPORT_H
#define DT25_D_IMGEXPORT_H


#include <string>

class d_ImgExport {
public:
    d_ImgExport();
    static void d_Capture(const std::string& _path);
};


#endif 
