
// Modified from https://gist.github.com/gamepopper/1931ca297f3decdee90e785f12762192

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

/// GLEW is needed to provide OpenGL extensions.
#include <GL/glew.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

/// GLM is needed to provide 3D math properties, particularly matrices for 3D transformations.
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp" 
#include "glm/gtx/transform.hpp"

#include <iostream>

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

///Basic vertex shader that transforms the vertex position based on a projection view matrix and passes the texture coordinate to the fragment shader.
const std::string defaultVertexShader =
"#version 330\n"\
"layout(location = 0) in vec3 position;"\

"void main() {"\
"	gl_Position = vec4(position, 1.0);"\
"}";

///Basic fragment shader that returns the colour of a pixel based on the input texture and its coordinate.
const std::string defaultFragShader =
"#version 330\n" \

"void main() {" \
"	gl_FragColor = vec4(1, 0, 0, 1);" \
"}";

///Shader Types
enum class ShaderType { Vertex, Fragment, Geometry, Count };
///Standard Uniforms in the shader.
enum class UniformType { TransformPVM, Count };
///Vertex attributes for shaders and the input vertex array.
enum class VertexAttribute { Position, TexCoord, COUNT };

///Shader Program
GLuint program = 0;

///List of shaders set up for a 3D scene.
GLuint shader[static_cast<unsigned int>(ShaderType::Count)];
///List of uniforms that can be defined values for the shader.
GLint uniform[static_cast<unsigned int>(UniformType::Count)];

///Vertex Array Object ID.
GLuint vao = 0;
///Vertex Buffer Object for Vertices.
GLuint vertexVBO = 0;
///Vertex Buffer Object for Indices.
GLuint indexVBO = 0;
///Depending on input, the amount of vertices or indices that are needed to be drawn for this object.
unsigned int drawCount;

///Checks for any errors specific to the shaders. It will output any errors within the shader if it's not valid.
void checkError(GLuint l_shader, GLuint l_flag, bool l_program, const std::string& l_errorMsg)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (l_program) { glGetProgramiv(l_shader, l_flag, &success); }
	else { glGetShaderiv(l_shader, l_flag, &success); }

	if (success) { return; }
	if (l_program) {
		glGetProgramInfoLog(l_shader, sizeof(error), nullptr, error);
	}
	else {
		glGetShaderInfoLog(l_shader, sizeof(error), nullptr, error);
	}

	std::cout << l_errorMsg << "\n";
}

///Creates and compiles a shader.
GLuint buildShader(const std::string& l_src, unsigned int l_type)
{
	GLuint shaderID = glCreateShader(l_type);
	if (!shaderID) {
		std::cout << "Bad shader type!";
		return 0;
	}
	const GLchar* sources[1];
	GLint lengths[1];
	sources[0] = l_src.c_str();
	lengths[0] = l_src.length();
	glShaderSource(shaderID, 1, sources, lengths);
	glCompileShader(shaderID);
	checkError(shaderID, GL_COMPILE_STATUS, false, "Shader compile error: ");
	return shaderID;
}

///Function to load the shaders from string data.
void LoadFromMemory(const std::string& shaderData, ShaderType type)
{
	if (shaderData.empty())
		return;

	if (shader[static_cast<unsigned int>(type)])
	{
		glDetachShader(program, shader[static_cast<unsigned int>(type)]);
		glDeleteShader(shader[static_cast<unsigned int>(type)]);
	}

	switch (type)
	{
	case ShaderType::Vertex:
		shader[static_cast<unsigned int>(type)] = buildShader(shaderData, GL_VERTEX_SHADER);
		break;
	case ShaderType::Geometry:
		shader[static_cast<unsigned int>(type)] = buildShader(shaderData, GL_GEOMETRY_SHADER);
		break;
	case ShaderType::Fragment:
		shader[static_cast<unsigned int>(type)] = buildShader(shaderData, GL_FRAGMENT_SHADER);
		break;
	default:
		break;
	}

	if (program == 0)
	{
		program = glCreateProgram();
	}

	glAttachShader(program, shader[static_cast<unsigned int>(type)]);
	glBindAttribLocation(program, static_cast<GLuint>(VertexAttribute::Position), "position");

	glLinkProgram(program);
	checkError(program, GL_LINK_STATUS, true, "Shader link error:");
	glValidateProgram(program);
	checkError(program, GL_VALIDATE_STATUS, true, "Invalid shader:");
}

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
	bool exit = false;
	bool sRgb = false;

	while (!exit)
	{
		// Request a 24-bits depth buffer when creating the window
		sf::ContextSettings contextSettings;
		contextSettings.depthBits = 24;

		// Create the main window
		sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML graphics with OpenGL", sf::Style::Default);
		window.setVerticalSyncEnabled(true);

		// Initialise GLEW for the extended functions.
		glewExperimental = GL_FALSE;
		if (glewInit() != GLEW_OK)
			return EXIT_FAILURE;

		// Load a texture to apply to our 3D cube
		sf::Texture texture;
		if (!texture.loadFromFile("resources/texture.jpg"))
			return EXIT_FAILURE;

		// Attempt to generate a mipmap for our cube texture
		// We don't check the return value here since
		// mipmapping is purely optional in this example
		std::ignore = texture.generateMipmap();

		// Make the window the active window for OpenGL calls
		std::ignore = window.setActive(true);

		// Load the shaders we need.
		if (program == 0)
		{
			LoadFromMemory(defaultVertexShader, ShaderType::Vertex);
			LoadFromMemory(defaultFragShader, ShaderType::Fragment);
		}

		// Enable Z-buffer read and write and culling.
		glEnable(GL_DEPTH_TEST);

		// Setup a perspective projection
		GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
		glm::mat4 projection = glm::frustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.0f);

        GLfloat vertices[] = {
            -1.f, -1.f, 0.f,
            -1.f, 1.f, 0.f,
            1.f, 1.f, 0.f/*,
            1.f, -1.f, 0.f*/
        };
        GLuint indices[] = {
            0, 1, 2
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glBindVertexArray(0);
		
		//Make sure to bind the vertex array to null if you wish to define more objects.
		glBindVertexArray(0);

		// Make the window no longer the active window for OpenGL calls
		// auto a = window.setActive(false);
        if (glGetError() == GL_INVALID_OPERATION)
            std::cout << "Set window error\n";

		// Create a clock for measuring the time elapsed
		sf::Clock clock;

		// Flag to track whether mipmapping is currently enabled
		bool mipmapEnabled = true;

		// Start game loop
		while (window.isOpen())
		{
			// Process events
            while (const std::optional event = window.pollEvent())
			{
				// Close window: exit
				if (event->is<sf::Event::Closed>())
				{
					exit = true;
					window.close();
				}

                else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                {
                    // Escape key: exit
                    if (key_pressed->code == sf::Keyboard::Key::Escape)
                    {
                        exit = true;
                        window.close();
                    }

                    // Return key: toggle mipmapping
                    else if (key_pressed->code == sf::Keyboard::Key::Enter)
                    {
                        if (mipmapEnabled)
                        {
                            // We simply reload the texture to disable mipmapping
                            if (!texture.loadFromFile("resources/texture.jpg"))
                                return EXIT_FAILURE;

                            mipmapEnabled = false;
                        }
                        else
                        {
                            std::ignore = texture.generateMipmap();
                            mipmapEnabled = true;
                        }
                    }

                    // Space key: toggle sRGB conversion
                    else if (key_pressed->code == sf::Keyboard::Key::Space)
                    {
                        sRgb = !sRgb;
                        window.close();
                    }
                }

				// Adjust the viewport when the window is resized
				else if (const auto* resized = event->getIf<sf::Event::Resized>())
				{
					// Make the window the active window for OpenGL calls
					std::ignore = window.setActive(true);
					glViewport(0, 0, resized->size.x, resized->size.y);

					// Make the window no longer the active window for OpenGL calls
					std::ignore = window.setActive(false);
				}
			}

			// Clear the depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Configure the viewport (the same size as the window)
			glViewport(0, 0, window.getSize().x, window.getSize().y);

			// Make the window the active window for OpenGL calls
			std::ignore = window.setActive(true);

			// Bind the texture
			glBindVertexArray(vao);

			//Bind the shaders.
			glUseProgram(program);

			// Draw the cube (scissored)
			// glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);

			// Reset the vertex array bound, shader and texture for other assets to draw.
			glBindVertexArray(0);
			glUseProgram(0);

			// Make the window no longer the active window for OpenGL calls
			std::ignore = window.setActive(false);

			// Finally, display the rendered frame on screen
			window.display();
		}

		//Destroy all buffers, shaders and programs.
		glDeleteBuffers(1, &vertexVBO);
		glDeleteBuffers(1, &indexVBO);
		glDeleteVertexArrays(1, &vao);

		//Setting these values to zero will allow them to be initialised with new data on reset.
		vertexVBO = 0;
		indexVBO = 0;
		vao = 0;

		for (unsigned int i = 0; i < static_cast<unsigned int>(ShaderType::Count); i++)
		{
			glDetachShader(program, shader[i]);
			glDeleteShader(shader[i]);
			shader[i] = 0;
		}

		for (unsigned int i = 0; i < static_cast<unsigned int>(UniformType::Count); i++)
		{
			uniform[i] = -1;
		}

		glDeleteProgram(program);
		program = 0;
	}

	return EXIT_SUCCESS;
}