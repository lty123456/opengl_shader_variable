When i start my graphic coding from https://learnopengl.com/,i found glsl uniform setting code in cpp is unmanaged and may be chaos,so i write this little tool

Example:
//step1:declare struct to be mapping to shader

#include <glm/glm.hpp>
#include "shadervariable.h"

ShaderStruct lightBase1
("lightBase1", 
    { 
        {"ambient", glm::vec3(0.2f, 0.2f, 0.2f)},
        {"diffuse", glm::vec3{0.6f, 0.6f, 0.6f}},
        {"specular", glm::vec3{1.0f, 1.0f, 1.0f}},       
    }
);

ShaderStruct dirLight
(lightBase1, "dirLight",
    { 
        {"direction", glm::vec3(-0.2f, -1.0f, -0.3f)},
    }
);

ShaderStruct pointLightBase1
(lightBase1, "pointLightBase1",
    {
        {"diffuse", glm::vec3{0.8f, 0.8f, 0.8f}},
        {"specular", glm::vec3{1.0f, 1.0f, 1.0f}},

        {"constant", 1.0f},
        {"linear", 0.09f},
        {"quadratic", 0.00032f},
    }
);

ShaderStruct pointLight1
(pointLightBase1, "pointLight1",
    {
        {"position", glm::vec3{0.7f,  0.2f,  2.0f}},
    }
);

ShaderStruct pointLight2
(pointLightBase1, "pointLight2",
    {
        {"position", glm::vec3{2.3f, -3.3f, -4.0f}},
    }
);

ShaderStruct pointLight3
(pointLightBase1, "pointLight3",
    {
        {"position", glm::vec3{-4.0f,  2.0f, -12.0f}},
    }
);

ShaderStruct pointLight4
(pointLightBase1, "pointLight4",
    {
        {"position", glm::vec3{0.0f,  0.0f, -3.0f}},
    }
);


ShaderArray<ShaderStruct> pointLights("pointLights", { pointLight1, pointLight2, pointLight3, pointLight4 });

ShaderStruct spotLight
(pointLightBase1, "spotLight",
    {
        {"position", glm::vec3{0.0f,  0.0f, -3.0f}},
        {"direction", glm::vec3{0.0f,  0.0f, -3.0f}},
        {"cutOff", glm::cos(glm::radians(12.5f))},
        {"outerCutOff", glm::cos(glm::radians(15.0f))},
    }
);

//step2:write
pointLights.Write(id);  //paramter id mean shader program id that you want to write to
spotLight.Write(id);

this example is from my learnning opengl code,it is not complex ,but the tool also can nesting the struct and array for each other,i have do some test for it but delete these code when it unused.

Depend:
1.glad
2.glm(optical glm support can use macro switch to set off)
