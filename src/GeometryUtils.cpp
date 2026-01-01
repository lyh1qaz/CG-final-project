#include "GeometryUtils.h"
#include <vector>
#include <cmath>

const float PI = 3.1415926535f;

// Helper function to calculate normal
inline glm::vec3 CalculateNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    return glm::normalize(glm::cross(b - a, c - a));
}

Mesh* GeometryUtils::CreateCube()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Cube vertex positions
    float verticesPos[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    // Cube face indices
    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 1, 5, 5, 4, 0, // Bottom face
        2, 3, 7, 7, 6, 2, // Top face
        0, 3, 7, 7, 4, 0, // Left face
        1, 2, 6, 6, 5, 1  // Right face
    };

    // Create vertices
    for (int i = 0; i < 8; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(verticesPos[i * 3], verticesPos[i * 3 + 1], verticesPos[i * 3 + 2]);
        // Simple normal calculation: each vertex's normal points to its face's normal direction
        vertex.Normal = vertex.Position; // Cube's normal is the same as vertex position (center at origin)
        vertices.push_back(vertex);
    }

    // Add indices
    for (int i = 0; i < 36; i++) {
        indices.push_back(cubeIndices[i]);
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateSphere(int latitudeSegments, int longitudeSegments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Generate vertices
    for (int lat = 0; lat <= latitudeSegments; lat++) {
        float theta = lat * PI / latitudeSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeSegments; lon++) {
            float phi = lon * 2 * PI / longitudeSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Calculate vertex position
            glm::vec3 position;
            position.x = cosPhi * sinTheta;
            position.y = cosTheta;
            position.z = sinPhi * sinTheta;

            // Normal is the same as vertex position (since center is at origin)
            glm::vec3 normal = position;

            // Texture coordinates
            glm::vec2 texCoord;
            texCoord.x = (float)lon / (float)longitudeSegments;
            texCoord.y = (float)lat / (float)latitudeSegments;

            vertices.push_back({position, normal, texCoord});
        }
    }

    // Generate indices
    for (int lat = 0; lat < latitudeSegments; lat++) {
        for (int lon = 0; lon < longitudeSegments; lon++) {
            unsigned int first = lat * (longitudeSegments + 1) + lon;
            unsigned int second = first + longitudeSegments + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateCylinder(float radius, float height, int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    float halfHeight = height / 2.0f;

    // Generate side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        // Bottom vertex
        glm::vec3 bottomPos(radius * cosAngle, -halfHeight, radius * sinAngle);
        glm::vec3 bottomNormal(cosAngle, 0.0f, sinAngle);
        vertices.push_back({bottomPos, bottomNormal, glm::vec2((float)i / segments, 0.0f)});

        // Top vertex
        glm::vec3 topPos(radius * cosAngle, halfHeight, radius * sinAngle);
        glm::vec3 topNormal(cosAngle, 0.0f, sinAngle);
        vertices.push_back({topPos, topNormal, glm::vec2((float)i / segments, 1.0f)});
    }

    // Generate top and bottom centers
    // Top center
    vertices.push_back({glm::vec3(0.0f, halfHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});
    // Bottom center
    vertices.push_back({glm::vec3(0.0f, -halfHeight, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});

    int topCenterIdx = vertices.size() - 2;
    int bottomCenterIdx = vertices.size() - 1;

    // Generate side indices
    for (int i = 0; i < segments; i++) {
        int current = i * 2;
        int next = (i + 1) * 2;

        // First triangle
        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);

        // Second triangle
        indices.push_back(next);
        indices.push_back(next + 1);
        indices.push_back(current + 1);
    }

    // Generate top face indices
    for (int i = 0; i < segments; i++) {
        indices.push_back(topCenterIdx);
        indices.push_back(i * 2 + 1);
        indices.push_back(((i + 1) % segments) * 2 + 1);
    }

    // Generate bottom face indices
    for (int i = 0; i < segments; i++) {
        indices.push_back(bottomCenterIdx);
        indices.push_back(((i + 1) % segments) * 2);
        indices.push_back(i * 2);
    }

    return new Mesh(vertices, indices, textures);
}

Mesh* GeometryUtils::CreateCone(float radius, float height, int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Generate vertices
    // Bottom center
    vertices.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f)});
    
    // Bottom vertices
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back({glm::vec3(x, 0.0f, z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle))});
    }
    
    // Apex
    vertices.push_back({glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f)});
    
    int apexIdx = vertices.size() - 1;
    
    // Generate bottom face indices
    for (int i = 1; i < segments + 1; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
    
    // Generate side face indices
    for (int i = 1; i < segments + 1; i++) {
        indices.push_back(apexIdx);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Calculate side face normals
    for (int i = 1; i < segments + 2; i++) {
        vertices[i].Normal = CalculateNormal(vertices[apexIdx].Position, vertices[i].Position, vertices[(i % segments) + 1].Position);
    }

    return new Mesh(vertices, indices, textures);
}