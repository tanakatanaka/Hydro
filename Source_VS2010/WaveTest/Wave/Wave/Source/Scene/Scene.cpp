#include "scene.h"

class SceneBasic : public Scene
{
private:
    int width, height;
    
public:
    SceneBasic();

    void InitScene();
    void Update( float t );
    void Render();
    void Resize(int, int);
};
