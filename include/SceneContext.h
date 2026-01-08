#ifndef SCENE_CONTEXT_H
#define SCENE_CONTEXT_H

#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Component.h"

// Forward declaration
class Camera;

struct SceneObject
{
    std::string name;
    Mesh *mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color; // Albedo

    // PBR Material properties
    float roughness = 0.5f;
    float metallic = 0.0f;

    // [新增] 纹理路径，用于 UI 显示和 Part C 加载
    std::string texturePath;
    // [新增] 纹理ID (如果加载成功)
    unsigned int textureId = 0;

    // [新增] 模型路径，用于保存/加载
    std::string meshPath;

    // Components
    std::vector<Component *> components;

    // Context reference
    class SceneContext *sceneContext = nullptr;

    SceneObject(std::string n, Mesh *m)
        : name(n), mesh(m), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f), texturePath(""), meshPath("") {}

    SceneObject *Clone()
    {
        SceneObject *newObj = new SceneObject(name, mesh);
        newObj->position = position;
        newObj->rotation = rotation;
        newObj->scale = scale;
        newObj->color = color;
        newObj->roughness = roughness;
        newObj->metallic = metallic;
        newObj->texturePath = texturePath;
        newObj->textureId = textureId;
        newObj->meshPath = meshPath;

        for (auto c : components)
        {
            Component *newC = c->Clone();
            newObj->AddComponent(newC);
        }
        return newObj;
    }

    ~SceneObject()
    {
        for (auto c : components)
            delete c;
        components.clear();
    }

    void Update(float deltaTime)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->Update(deltaTime);
        }
    }

    // [New] Render gizmos
    void DrawGizmos(Shader &shader)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->OnDrawGizmos(shader);
        }
    }

    void OnCollision(SceneObject *other)
    {
        for (auto c : components)
        {
            if (c->enabled)
                c->OnCollision(other);
        }
    }

    void AddComponent(Component *c)
    {
        c->owner = this;
        components.push_back(c);
        c->Start();
    }

    template <typename T>
    T *AddComponent()
    {
        T *c = new T();
        c->owner = this;
        components.push_back(c);
        c->Start();
        return c;
    }

    template <typename T>
    T *GetComponent()
    {
        for (auto c : components)
        {
            if (dynamic_cast<T *>(c))
                return static_cast<T *>(c);
        }
        return nullptr;
    }

    void RemoveComponent(Component *c)
    {
        auto it = std::find(components.begin(), components.end(), c);
        if (it != components.end())
        {
            delete *it;
            components.erase(it);
        }
    }
};

class SceneContext
{
public:
    std::vector<SceneObject *> objects;
    SceneObject *selectedObject = nullptr;
    Camera *mainCamera = nullptr;

    SceneContext();
    ~SceneContext();

    SceneContext *Clone()
    {
        SceneContext *newScene = new SceneContext();
        for (auto obj : objects)
        {
            SceneObject *newObj = obj->Clone();
            newScene->AddObject(newObj);
        }
        return newScene;
    }

    void AddObject(SceneObject *obj);
    void DrawAll(Shader &shader);
    void DrawGizmos(Shader &shader);

    void SaveScene(const std::string &filename);
    void LoadScene(const std::string &filename);
};

#endif