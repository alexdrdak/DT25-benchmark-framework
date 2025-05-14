#include "d_NURBS.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

d_NURBS::d_NURBS(std::vector<glm::vec3> _points, std::vector<float> _w, std::vector<float> _k)
    : d_points(std::move(_points)), d_weights(std::move(_w)), d_knots(std::move(_k)) {

    if (d_knots.size() != d_points.size() + d_degree + 1) {
        throw std::invalid_argument("[d_NURBS]: Invalid configuration (knot vector size mismatch).");
    }

    if (d_weights.size() != d_points.size()) {
        throw std::invalid_argument("[d_NURBS]: Invalid configuration (weights size mismatch).");
    }

    d_degree = d_knots.size() - d_points.size() - 1;
    d_Logger::info("[d_NURBS]: Initialized with degree " + std::to_string(d_degree));
}

d_NURBS::d_NURBS(std::vector<glm::vec3> _points, int _d): d_points(std::move(_points)), d_degree(_d) {
    d_Logger::info("[d_NURBS]: no knot vector provided. assigning a default, nonperiodic uniform knot vector.");

    d_weights.resize(d_points.size());
    std::fill(d_weights.begin(), d_weights.end(), 1.0f);
    d_knots.resize(d_points.size() + d_degree + 1);

    std::string debugString = "[";
    for (int i =0; i < d_degree; i++)
        debugString += "0.000000, ";
    float t_val = 0.0f;
    for(int i = d_degree; i < d_points.size(); i++) {
        debugString += std::to_string(t_val) + ", ";
        d_knots[i] = t_val++;
    }
    for(int i = d_points.size(); i < d_knots.size(); i++) {
        debugString += std::to_string(t_val) + ", ";
        d_knots[i] = (float)d_degree;
    }
    debugString.pop_back();
    debugString.pop_back();
    debugString += "]";

    d_Logger::info("[d_NURBS]: generated knot vector: '"+debugString+"'");
}


float d_NURBS::d_GetBasis(int _index, int _degree, float _value) {
    if (_degree == 0) {
        // Handle periodic or last knot interval explicitly
        if (_index + 1 == d_knots.size()) {
            return (_value >= d_knots[_index] && _value <= d_knots[_index + 1]) ? 1.0f : 0.0f;
        }
        return (_value >= d_knots[_index] && _value < d_knots[_index + 1]) ? 1.0f : 0.0f;
    }

    float denom1 = d_knots[_index + _degree] - d_knots[_index];
    float denom2 = d_knots[_index + _degree + 1] - d_knots[_index + 1];

    float term1 = denom1 > 0 ? (_value - d_knots[_index]) / denom1 * d_GetBasis(_index, _degree - 1, _value) : 0.0f;
    float term2 = denom2 > 0 ? (d_knots[_index + _degree + 1] - _value) / denom2 * d_GetBasis(_index + 1, _degree - 1, _value) : 0.0f;

    return term1 + term2;
}

glm::vec3 d_NURBS::d_GetPointByParameter(float _parameter) {
    glm::vec3 numerator(0.0f);
    float denominator = 0.0f;

    for (size_t i = 0; i < d_points.size(); i++) {
        float basisValue = d_GetBasis(i, d_degree, _parameter) * d_weights[i];
        numerator += basisValue * d_points[i];
        denominator += basisValue;
    }

    if (denominator < 1e-6) {  // Use a small epsilon for stability
        d_Logger::err("[d_NURBS]: Warning - Denominator near zero, clamping to avoid division by zero.");
        denominator = 1e-6;  // Prevent division by zero
    }
    //d_Logger::info("[d_NURBS]: queried for "+std::to_string(_parameter)+" t parameter. loc: "+glm::to_string(numerator/denominator));

    return numerator / denominator;
}

void d_NURBS::d_Init(std::vector<glm::vec3> _points, std::vector<float> _w, std::vector<float> _k) {
    d_points = _points;
    d_weights = _w;
    d_knots = _k;
    d_degree = d_knots.size() - d_points.size() - 1;
}

glm::vec3 d_NURBS::d_GetPointByPerc(float _perc) {
    if (_perc >= 1.0f)
        return d_points.back();
    return d_GetPointByParameter(d_knots.back()*_perc);
}
