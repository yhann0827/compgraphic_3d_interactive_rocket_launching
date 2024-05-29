// You will need to define your own tests. Refer to CW1 or Exercise G.3 for
// examples.

#include <catch2/catch_amalgamated.hpp>
#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 matrix by matrix multiplication", "[mat44][mat44]")
{
    // TODO: implement your own tests here
    static constexpr float kEps_ = 1e-6f; // Increase tolerance
    using namespace Catch::Matchers;

    // Simple check: rotating zero degrees should yield an identity matrix
    SECTION("Identity multiply by Identity")
    {
        Mat44f identity;
        identity(0, 0) = 1.0f;
        identity(0, 1) = 0.0f;
        identity(0, 2) = 0.0f;
        identity(0, 3) = 0.0f;
        identity(1, 0) = 0.0f;
        identity(1, 1) = 1.0f;
        identity(1, 2) = 0.0f;
        identity(1, 3) = 0.0f;
        identity(2, 0) = 0.0f;
        identity(2, 1) = 0.0f;
        identity(2, 2) = 1.0f;
        identity(2, 3) = 0.0f;
        identity(3, 0) = 0.0f;
        identity(3, 1) = 0.0f;
        identity(3, 2) = 0.0f;
        identity(3, 3) = 1.0f;


        auto const identityAnswer = identity * identity;

        REQUIRE_THAT(identityAnswer(0, 0), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(identityAnswer(1, 1), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(identityAnswer(2, 2), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(identityAnswer(3, 3), WithinAbs(1.0f, kEps_));

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i != j) {
                    REQUIRE_THAT(identityAnswer(i, j), WithinAbs(0.0f, kEps_));
                }
            }
        }
    }
}

TEST_CASE("4x4 matrix by vector multiplication", "[mat44][vec4]")
{
    // TODO: implement your own tests here
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    // Simple check: multiplying an identity matrix by a vector should result in the same vector
    SECTION("Identity multiply by vector")
    {
        Mat44f identity2;
        identity2(0, 0) = 1.0f;
        identity2(0, 1) = 0.0f;
        identity2(0, 2) = 0.0f;
        identity2(0, 3) = 0.0f;
        identity2(1, 0) = 0.0f;
        identity2(1, 1) = 1.0f;
        identity2(1, 2) = 0.0f;
        identity2(1, 3) = 0.0f;
        identity2(2, 0) = 0.0f;
        identity2(2, 1) = 0.0f;
        identity2(2, 2) = 1.0f;
        identity2(2, 3) = 0.0f;
        identity2(3, 0) = 0.0f;
        identity2(3, 1) = 0.0f;
        identity2(3, 2) = 0.0f;
        identity2(3, 3) = 1.0f;

        Vec4f vector;
        vector.x = 1.0f;
        vector.y = 1.0f;
        vector.z = 1.0f;
        vector.w = 1.0f;

        auto const identityAnswer2 = identity2 * vector;

        REQUIRE_THAT(identityAnswer2.x, WithinAbs(vector.x, kEps_));
        REQUIRE_THAT(identityAnswer2.y, WithinAbs(vector.y, kEps_));
        REQUIRE_THAT(identityAnswer2.z, WithinAbs(vector.z, kEps_));
        REQUIRE_THAT(identityAnswer2.w, WithinAbs(vector.w, kEps_));
    }
}

TEST_CASE("Rotation around x-axis", "[mat44]")
{
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    // Test for rotating by 45 degrees around the x-axis
    // Rotating 45 degrees = pi/4 radians.
    SECTION("Rotate 45 degrees in respect to x-axis")
    {
        auto const rotation = make_rotation_x(3.1415926f / 4.0f);

        REQUIRE_THAT(rotation(0, 0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(1, 1), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(1, 2), WithinAbs(-std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(2, 1), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(2, 2), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
    }
}

TEST_CASE("rotate in respect with y-axis", "[mat44]")
{
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    // Test for rotating by 45 degrees around the x-axis
    // Rotating 45 degrees = pi/4 radians.
    SECTION("Rotate 45 degrees in respect to y-axis")
    {
        auto const rotation = make_rotation_y(3.1415926f / 4.0f);

        REQUIRE_THAT(rotation(0, 0), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(0, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(0, 2), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(1, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(1, 1), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(2, 0), WithinAbs(-std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(2, 2), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
    }

}

TEST_CASE("rotate in respect with z-axis", "[mat44][vec4]")
{
    // TODO: implement your own tests here
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    // Test for rotating by 45 degrees around the x-axis
    // Rotating 45 degrees = pi/4 radians.
    SECTION("Rotate 45 degrees in respect to z-axis")
    {
        auto const rotation = make_rotation_z(3.1415926f / 4.0f);

        REQUIRE_THAT(rotation(0, 0), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(0, 1), WithinAbs(-std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(0, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(0, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(1, 0), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(1, 1), WithinAbs(std::sqrt(2.0f) / 2.0f, kEps_));
        REQUIRE_THAT(rotation(1, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(1, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(2, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(2, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(2, 2), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(rotation(2, 3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rotation(3, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rotation(3, 3), WithinAbs(1.f, kEps_));
    }
}

TEST_CASE("matrix translation", "[mat44][vec4]")
{
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    // Test for a translation matrix
    SECTION("Translation matrix for (1, 2, 3)")
    {
        Vec3f translationVector{ 1.0f, 2.0f, 3.0f };
        auto const translationMatrix = make_translation(translationVector);

        REQUIRE_THAT(translationMatrix(0, 0), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(translationMatrix(0, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(0, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(0, 3), WithinAbs(1.0f, kEps_));

        REQUIRE_THAT(translationMatrix(1, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(1, 1), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(translationMatrix(1, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(1, 3), WithinAbs(2.0f, kEps_));

        REQUIRE_THAT(translationMatrix(2, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(2, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(2, 2), WithinAbs(1.0f, kEps_));
        REQUIRE_THAT(translationMatrix(2, 3), WithinAbs(3.0f, kEps_));

        REQUIRE_THAT(translationMatrix(3, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(3, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(3, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translationMatrix(3, 3), WithinAbs(1.0f, kEps_));
    }
}

TEST_CASE("make_perspective projection", "[mat44][vec4]")
{
    // TODO: implement your own tests here
    static constexpr float kEps_ = 1e-6f;
    using namespace Catch::Matchers;

    SECTION("Custom Parameters")
    {
        // Perspective projection with specified parameters
        SECTION("Custom Parameters")
        {
            auto const proj = make_perspective_projection(
                45.f * 3.1415926f / 180.f,
                1280 / float(720),
                1.0f, 100.0f
            );

            REQUIRE_THAT(proj(0, 0), WithinAbs(1.0f / (1280 / float(720) * std::tan(45.f * 0.5f * 3.1415926f / 180.f)), kEps_));
            REQUIRE_THAT(proj(0, 1), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(0, 2), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(0, 3), WithinAbs(0.f, kEps_));

            REQUIRE_THAT(proj(1, 1), WithinAbs(1.0f / std::tan(45.f * 0.5f * 3.1415926f / 180.f), kEps_));
            REQUIRE_THAT(proj(1, 0), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(1, 2), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(1, 3), WithinAbs(0.f, kEps_));

            REQUIRE_THAT(proj(2, 0), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(2, 1), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(2, 2), WithinAbs(-(100.0f + 1.0f) / (100.0f - 1.0f), kEps_));
            REQUIRE_THAT(proj(2, 3), WithinAbs(-2.0f * 100.0f * 1.0f / (100.0f - 1.0f), kEps_));

            REQUIRE_THAT(proj(3, 1), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(3, 1), WithinAbs(0.f, kEps_));
            REQUIRE_THAT(proj(3, 2), WithinAbs(-1.0f, kEps_));
            REQUIRE_THAT(proj(3, 3), WithinAbs(0.0f, kEps_));
        }
    }

    //     The custom parameters for the perspective projection test case were chosen based on common values used in computer graphics for setting up a camera projection matrix. Here's how each parameter was chosen:

    //     Field of View (FOV): 45 degrees
    //         A field of view of 45 degrees is a common choice for a balanced perspective.

    //     Aspect Ratio: 16:9
    //         The aspect ratio of 16:9 is a standard widescreen aspect ratio used in many displays.

    //     Near Plane: 1.0f
    //         The near plane represents the closest distance at which objects are visible. Choosing a value of 1.0f is a common practice.

    //     Far Plane: 100.0f
    //         The far plane represents the furthest distance at which objects are visible. Choosing a value of 100.0f allows for a significant depth range.

    // These values are not strict rules, and they can vary based on the specific requirements of a given application or scene. The chosen parameters aim to cover a typical scenario for a perspective projection matrix.
}