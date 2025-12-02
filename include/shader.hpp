#pragma once
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
public:
  unsigned int ID;

  Shader(const char* vertexPath, const char* fragmentPath)
  {
      // Read shader files
      std::string vertexCode = readShaderFile(vertexPath);
      std::string fragmentCode = readShaderFile(fragmentPath);
      const char* vShaderCode = vertexCode.c_str();
      const char* fShaderCode = fragmentCode.c_str();

      // Compile shaders
      unsigned int vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
      unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

      // Link program
      ID = glCreateProgram();
      glAttachShader(ID, vertex);
      glAttachShader(ID, fragment);
      glLinkProgram(ID);

      // Check for errors
      int success;
      char infoLog[512];
      glGetProgramiv(ID, GL_LINK_STATUS, &success);
      if (!success)
      {
          glGetProgramInfoLog(ID, 512, NULL, infoLog);
          std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      }

      // Cleanup
      glDeleteShader(vertex);
      glDeleteShader(fragment);
  }

  void use()
  {
      glUseProgram(ID);
  }

  void setBool(const std::string &name, bool value) const
    {
      glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
  void setInt(const std::string &name, int value) const
    {
      glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
  void setFloat(const std::string &name, float value) const
    {
      glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
  void setVec2(const char* name, float x, float y)
  {
    glUniform2f(glGetUniformLocation(ID, name), x, y);
  }

private:
    std::string readShaderFile(const char* filepath)
    {
        std::ifstream file;
        std::stringstream buffer;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            file.open(filepath);
            buffer << file.rdbuf();
            file.close();
            return buffer.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath << std::endl;
            return "";
        }
    }

    unsigned int compileShader(unsigned int type, const char* source)
    {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        return shader;
    }
};