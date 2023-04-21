#include "AriaPCH.h"

/**
 * Hello colorful triangle
 *
 * An OpenGL "Hello Triangle" using
 * - glad instead of glew (because reason), and glfw3,
   - pieces from learnopengl.com
 * - the gl_utils lib of Anton Gerdelan tutos
 * - DSA, looking at fendevel/Guide-to-Modern-OpenGL-Functions @github
 * - heavy comments! This is the way.
 *
 * @author ludo456 / the opensourcedev @github
 */
#include "Core/Application.h"

int main()
{	
    Application app;

    if (app.startup())
    {
        app.run();
    }

    return 0;
}