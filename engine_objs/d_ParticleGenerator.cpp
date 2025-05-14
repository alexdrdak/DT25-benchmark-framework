#include <sstream>
#include <random>
#include <array>
#include "../d_glob.h"
#include "d_Texture.h"
#include "d_ParticleGenerator.h"
#include "tiffio.h"

#include "stb_image.h"
#include "stb_image_write.h"

d_ParticleGenerator::d_ParticleGenerator(int d_type, glm::vec3 _center, float _radius, int _seed):
    d_type(d_type), d_x(_center), d_radius(_radius) { }

d_ParticleGenerator::d_ParticleGenerator(int d_type, glm::vec3 xs, glm::vec3 ys, glm::vec3 zs, int _seed):
    d_type(d_type), d_x(xs), d_y(ys), d_z(zs), m_mt(_seed) { }

d_ParticleGenerator::d_ParticleGenerator(int _type, glm::vec3 _start, glm::vec3 _end) : d_type(_type), d_start(_start), d_end(_end) { }

std::array<float, 3> d_ParticleGenerator::d_GenerateParticle(int mode) {
    switch (mode)
    {
        case 0:
        {
            std::uniform_real_distribution<float> d_uniformdis_x(d_x[0], d_x[1]);
            std::uniform_real_distribution<float> d_uniformdis_y(d_y[0], d_y[1]);
            std::uniform_real_distribution<float> d_uniformdis_z(d_z[0], d_z[1]);
            return { d_uniformdis_x(m_mt),
                    d_uniformdis_y(m_mt),
                    d_uniformdis_z(m_mt) };
        }

        case 1:
        {
            std::normal_distribution<float> d_normaldist_x(d_x[0], d_radius);
            std::normal_distribution<float> d_normaldist_y(d_x[1], d_radius);
            std::normal_distribution<float> d_normaldist_z(d_x[2], d_radius);
            return { d_normaldist_x(m_mt),
                     d_normaldist_y(m_mt),
                     d_normaldist_z(m_mt) };
        }
    }
}

std::vector<float> d_ParticleGenerator::d_GenerateData(unsigned int _nInstances) {
    std::vector<float> result;
    result.resize(_nInstances *3);
    if (result.size() != static_cast<size_t>(_nInstances) * 3)
        throw std::runtime_error("Failed to allocate memory of " + std::to_string(_nInstances * _nInstances * 3) + " for particles.");

    switch(d_type) {
        // uniform
        case 1: {
            for (unsigned int i = 0; i < _nInstances; i++) {
                auto temp = d_GenerateParticle();
                result[i * 3] = temp[0];
                result[i * 3 + 1] = temp[1];
                result[i * 3 + 2] = temp[2];
            }
            break;
        }

        // linear
        case 2: {
            glm::vec3 dir = d_end - d_start;
            if (_nInstances == 1)
            {
                glm::vec3 coords = d_start + (dir * 0.5f);
                result[0] = coords.x;
                result[1] = coords.y;
                result[2] = coords.z;
            } 
            else
            {
                for (unsigned int i = 0; i < _nInstances; i++) {
                    glm::vec3 coords = d_start + (dir / static_cast<float>(_nInstances - 1)) * static_cast<float>(i);
                    result[i * 3] = coords.x;
                    result[i * 3 + 1] = coords.y;
                    result[i * 3 + 2] = coords.z;
                }
            }

            break;
        }

        // uniform grid
        case 3:
        {
            result.resize(_nInstances * _nInstances * 3);
            if (result.size() != static_cast<size_t>(_nInstances * _nInstances * 3))
                throw std::runtime_error("Failed to allocate memory of " + std::to_string(_nInstances*_nInstances*3) + " for particles.");

            float d_step = 1.0f / _nInstances;
            float d_currStep1 = 0.0f;
            float d_currStep2 = 0.0f;

            for (size_t i = 0; i < _nInstances; i++)
            {
                for (size_t j = 0; j < _nInstances; j++)
                {
                    glm::vec3 newPoint = d_x + d_y * d_currStep1 + d_z * d_currStep2;

                    size_t index = (i * _nInstances + j) * 3; 

                    result[index] = newPoint.x;
                    result[index + 1] = newPoint.y;
                    result[index + 2] = newPoint.z;

                    d_currStep1 += d_step;
                }

                d_currStep1 = 0.0f;
                d_currStep2 += d_step;
            }

            break;
        }

        // gaussian
        case 4:
        {
            for (int i = 0; i < _nInstances; ++i) {

                auto temp = d_GenerateParticle(1);

                result[i * 3] = temp[0];
                result[i * 3 + 1] = temp[1];
                result[i * 3 + 2] = temp[2];
            }

            break;
        }

        // https://math.stackexchange.com/questions/1585975/how-to-generate-random-points-on-a-sphere
        case 5:
        {
            for (int i = 0; i < _nInstances; ++i) {

                auto temp = d_GenerateParticle(1);
                glm::vec3 point = glm::vec3(temp[0], temp[1], temp[2]);

                point = glm::normalize(point);

                result[i * 3] = d_x.x + point.x * d_radius;
                result[i * 3 + 1] = d_x.y + point.y * d_radius;
                result[i * 3 + 2] = d_x.z + point.z * d_radius;
            }

            break;
        }
    }

    return result;
}
