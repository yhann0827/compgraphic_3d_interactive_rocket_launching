#include "cone.hpp"
#include <typeinfo>
#include <stdexcept>
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.cpp"

SimpleMeshDataWithoutTexture make_cone( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	std::vector<Vec3f> pos;
    std::vector<Vec3f> normals;
    float prevY = std::cos(0.f);
    float prevZ = std::sin(0.f);

    Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

    for (std::size_t i = 0; i < aSubdivs; ++i) {
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
        float y = std::cos(angle);
        float z = std::sin(angle);
        
        // // Use 0 for x-coordinate to create a cone
        // pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
        // pos.emplace_back(Vec3f{ 0.f, y, z });
        // pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });  // Tip of the cone

        Vec3f p1{0.f, prevY, prevZ};
        Vec3f p2{0.f, y, z};
        Vec3f p3{1.f, 0.f, 0.f};

        Vec3f normal1 = normalize(p1 - Vec3f{0.f, 0.f, 0.f});
        Vec3f normal2 = normalize(p2 - Vec3f{0.f, 0.f, 0.f});
        Vec3f normal3 = normalize(p3 - Vec3f{0.f, 0.f, 0.f});

        pos.emplace_back(p1);
        normals.emplace_back(normal1);

        pos.emplace_back(p2);
        normals.emplace_back(normal2);

        pos.emplace_back(p3);
        normals.emplace_back(normal3);

        
        prevY = y;
        prevZ = z;
    }
    
    for (auto& p : pos) {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
    }

    std::vector col(pos.size(), aColor);
    return SimpleMeshDataWithoutTexture{ std::move(pos), std::move(col), std::move(normals) };
}

