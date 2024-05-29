#include "simple_mesh.hpp"

SimpleMeshDataWithoutTexture concatenate(SimpleMeshDataWithoutTexture aM, SimpleMeshDataWithoutTexture const& aN)
{
    aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
    aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
    aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
    return aM;
}


GLuint create_vao(SimpleMeshData const& aMeshData)
{
    // Generate and bind VBO for positions
    GLuint vboPositions = 0;
    glGenBuffers(1, &vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

    GLuint vboColors = 0;
    glGenBuffers(1, &vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

    GLuint vboNormals = 0;
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

    GLuint vboTextures = 0;
    glGenBuffers(1, &vboTextures);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.textcoords.size() * sizeof(Vec2f), aMeshData.textcoords.data(), GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboNormals);
    glDeleteBuffers(1, &vboTextures);

    return vao;


}

GLuint create_vao_without_texture(SimpleMeshDataWithoutTexture const& aMeshData)
{
    // Generate and bind VBO for positions
    GLuint vboPositions = 0;
    glGenBuffers(1, &vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

    GLuint vboColors = 0;
    glGenBuffers(1, &vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

    GLuint vboNormals = 0;
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);


    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboNormals);


    return vao;
}
