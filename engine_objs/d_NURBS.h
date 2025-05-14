#ifndef DT25_D_NURBS_H
#define DT25_D_NURBS_H

#include <vector>
#include "../d_glob.h"

class d_NURBS {
private:
    std::vector<glm::vec3> d_points;
    std::vector<float> d_weights;
    std::vector<float> d_knots;

    int d_degree;

public:
    d_NURBS() = default;
    d_NURBS(std::vector<glm::vec3> _points, std::vector<float> _w, std::vector<float> _k);
    d_NURBS(std::vector<glm::vec3> _points, int _d);

    void d_Init(std::vector<glm::vec3> _points, std::vector<float> _w, std::vector<float> _k);
    glm::vec3 d_GetPointByParameter(float _parameter);
    glm::vec3 d_GetPointByPerc(float _perc);
    float d_GetBasis(int _index, int _degree, float _value);
};


#endif 
