#include "cube.hpp"
#include <typeinfo>
#include <stdexcept>
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.cpp"

SimpleMeshDataWithoutTexture make_cube(Vec3f aColor, Mat44f aPreTransform)
{
    std::vector<Vec3f> pos = {
        
        //one face
        {-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, 
        {1.0f, 1.0f,-1.0f}, {-1.0f,-1.0f,-1.0f}, {-1.0f, 1.0f,-1.0f},

        {1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f,-1.0f}, {1.0f,-1.0f,-1.0f},
        {1.0f, 1.0f,-1.0f}, {1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,-1.0f},
        
        {-1.0f,-1.0f,-1.0f}, {-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f,-1.0f},
        {1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f,-1.0f},

        {-1.0f, 1.0f, 1.0f}, {-1.0f,-1.0f, 1.0f}, {1.0f,-1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}, {1.0f,-1.0f,-1.0f}, {1.0f, 1.0f,-1.0f},
        
        {1.0f,-1.0f,-1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f,-1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f,-1.0f}, {-1.0f, 1.0f,-1.0f},

        {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f,-1.0f}, {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f,-1.0f, 1.0f}
    };

    std::vector<Vec3f> normals;
    for(size_t i = 0; i < pos.size(); i += 3)
    {
        Vec3f v1 = pos[i + 1] - pos[i];
        Vec3f v2 = pos[i + 2] - pos[i];
        Vec3f normal = normalize(cross(v1, v2));

        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
    }
 
    for (auto& p : pos) {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
    }
    std::vector col(pos.size(), aColor);

    return SimpleMeshDataWithoutTexture{ std::move(pos), std::move(col), std::move(normals)};
}