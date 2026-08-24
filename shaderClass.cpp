#include "shaderClass.h"

std::string get_file_contents(const char* filename)

{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw (errno);
}
	Shader::Shader(const char* vertexFile, const char* fragmentFile)
	{
		std::string vertexCode = get_file_contents(vertexFile);
		std::string fragmentCode = get_file_contents(fragmentFile);

		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();


		// creamos el shader de vertex, los compilamos y los linkeamos en un programa de shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		glCompileShader(vertexShader);

		// creamos el shader de fragmento, lo compilamos y lo linkeamos en un programa de shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		glCompileShader(fragmentShader);

		// creamos el programa de shader y lo linkeamos con los shaders de vertex y fragmento
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);

		// borramos los shaders de vertex y fragmento
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
}
	void Shader::Activate()
	{
		glUseProgram(ID);
	}

	void Shader::Delete()
	{
		glUseProgram(ID);
	}