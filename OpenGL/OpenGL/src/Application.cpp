#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//�ֱ����vertexShader��FragmentShader
struct ShaderProgramSource
{
	std::string VertexShader;
	std::string FragmentShader;
};

//�Զ�������shader�ļ�
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

//����shader
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	//����һ��shader����  ������һ�����Ա����õ�id
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	//Replaces the source code in a shader object
	//��һ��shader object���滻Դ��
	glShaderSource(id, 1, &src, nullptr);
	//Compiles a shader object
	glCompileShader(id);

	//ERROR HANDLING
	int result;
	//Returns a parameter from a shader object
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		//Returns the information log for a shader object
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile" <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
//����shader  vertex shader and fragment shader 
static unsigned int CreateShader(const std::string& vertexShader, const std::string fragmentShader)
{
	//����һ��program����
	//GLuint  =  unsigned int
	unsigned int program = glCreateProgram();
	//����һ��shader�����֮��,�������shader
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	//Attaches a shader object to a program object
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	//Links a program object
	glLinkProgram(program);
	//Validates a program object
	glValidateProgram(program);

	//Deletes a shader object
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error" << std::endl;
	//��ӡopengl�İ汾��Ϣ  �Կ���Ϣ
	std::cout << glGetString(GL_VERSION) << std::endl;

	float position[6] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f,  0.5f
	};

	//����buffer�������ݷŵ�������
	//Vertex Buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

	//������ζ�ȡ������������
	//Layer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, 0);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	std::cout << source.VertexShader << std::endl;
	std::cout << source.FragmentShader << std::endl;

	unsigned int shader = CreateShader(source.VertexShader, source.FragmentShader);
	//Installs a program object as part of current rendering state
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/*glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glEnd();*/

		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	//Deletes a program object
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}