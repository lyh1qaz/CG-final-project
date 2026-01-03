#include "ModelLoader.h"
#include "GeometryUtils.h"
#include "SceneContext.h"

// Simple test program to directly test each geometry export
int main() {
    // Test cylinder export
    Mesh* cylinderMesh = GeometryUtils::CreateCylinder(0.5f, 1.0f, 20);
    SceneObject* cylinderObj = new SceneObject("Cylinder", cylinderMesh);
    
    // Create a minimal SceneContext containing only our object
    SceneContext scene;
    scene.AddObject(cylinderObj);
    
    // Export cylinder to a separate file
    ModelLoader::ExportScene(&scene, "test_cylinder.obj");
    
    // No need to manually delete objects - SceneContext destructor will handle it
    // The SceneContext destructor automatically deletes all objects and their meshes
    
    return 0;
}