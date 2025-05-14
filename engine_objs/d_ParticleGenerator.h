#ifndef DT25_D_PARTICLEGENERATOR_H
#define DT25_D_PARTICLEGENERATOR_H

#include <random>

enum d_PartGenType {
    D_PARTGEN_NORMAL,
};
class d_ParticleGenerator {
private:
    int d_type;
    glm::vec3 d_x;
    glm::vec3 d_y;
    glm::vec3 d_z;

    glm::vec3 d_start;
    glm::vec3 d_end;

    float d_radius;

    std::mt19937 m_mt;

public:
    d_ParticleGenerator(int d_type, glm::vec3 _center, float _radius, int _seed = 42);
    d_ParticleGenerator(int d_type, glm::vec3 xs, glm::vec3 ys, glm::vec3 zs, int _seed = 42);
    d_ParticleGenerator(int _type, glm::vec3 _start, glm::vec3 _end);
    d_ParticleGenerator() = default;
    ~d_ParticleGenerator() = default;


    /*
    * modes:
    *   - 0: uniform
    *   - 1: normal
    */
    std::array<float, 3> d_GenerateParticle(int mode = 0);
    std::vector<float> d_GenerateData(unsigned int n_instances);
    d_Texture d_GenerateOffsetTexture(std::string& name, unsigned int n_instances);

};


#endif 