#pragma once

class Scene
{
public:
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void InitScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void Update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void Render() = 0;

    /**
      Called when screen is resized
      */
    virtual void Resize(int, int) = 0;
};
