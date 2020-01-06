#pragma once
#include "NativeHeader.h"

#define MAX_SHADERFILE_LINES 10240

nativenamespace{

	class Shader
	{

	private:

	public:
		enum AttributeType { Float, Int };
		Shader()
		{
			initialized = false;
			name = "...Unnamed Shader...";
			drawBuffer = 0;
			debug = false;
		}

		int id;
		int v_id;
		int g_id;
		int f_id;
		int tc_id;
		int te_id;
		std::map<std::string, int> uniforms;
		std::string name;
		bool initialized;
		bool debug;
		int drawBuffer;

		std::string vertexString;
		std::string geometryString;
		std::string fragmentString;
		std::string tessellationControlString;
		std::string tessellationEvaluationString;

		int GetUniformID(const std::string& variableName)
		{
			const char* u_name = variableName.c_str();
			int i = glGetUniformLocation(id,u_name);
			return i;
		}

		void CreateVertexNormalAttribute()
		{
			SetShaderAttribute(6, 3, AttributeType::Float, false, 0, 6);
		}

		void SetShaderAttribute(int location,int size,AttributeType type,bool normalise,GLuint relativeOffset, int VAObufferBindPoint)
		{
			glEnableVertexAttribArray(location);
			if (type == AttributeType::Int)
				glVertexAttribIFormat(location, size, GL_INT, relativeOffset);
			else
				glVertexAttribFormat(location, size, GL_FLOAT, normalise, relativeOffset);

			glVertexAttribBinding(location, VAObufferBindPoint); // bind to second vertex buffer
		}

		/*
		void Compile(std::string Name);
		int GetUniformLocation(char* variableName);
		void Debug();
		void Select();
		*/

		void CleanGPU()
		{
			if (!initialized)
				return;
			glUseProgram(id);

			glDetachShader(id, v_id);
			glDetachShader(id, f_id);
			if (geometryString.length() > 0)
				glDetachShader(id, g_id);
			if (tessellationControlString.length() > 0)
				glDetachShader(id, tc_id);
			if (tessellationEvaluationString.length() > 0)
				glDetachShader(id, te_id);

			glDeleteShader(v_id);
			glDeleteShader(f_id);
			if (geometryString.length() > 0)
				glDeleteShader(g_id);
			if (tessellationControlString.length() > 0)
				glDeleteShader(tc_id);
			if (tessellationEvaluationString.length() > 0)
				glDeleteShader(te_id);

			glDeleteProgram(id);
			initialized = false;
		}

		~Shader()
		{
			CleanGPU();
		}

		//static void DebugShaders(GLuint programID);

		//static bool CompileSuccessful(int obj);

		//static bool LinkSuccessful(int obj);

		//static GLuint Create(GLenum type, const char* src);

		//todo: need to implement log for shader linking

		void Recompile()
		{
			CleanGPU();
			Compile();
		}
		void Compile(std::string Name)
		{
			name = Name;
			Compile();
		}

		void Compile(std::string Name, const char* VertexShaderFilePath, const char* FragmentShaderFilePath)
		{
			name = Name;
			vertexString   = LoadFile(VertexShaderFilePath);
			fragmentString = LoadFile(FragmentShaderFilePath);
			Compile();
		}

		void Compile(std::string Name, const char* VertexShaderFilePath, const char* GeometryShaderFilePath, const char* FragmentShaderFilePath)
		{
			name = Name;
			vertexString   = LoadFile(VertexShaderFilePath);
			geometryString = LoadFile(GeometryShaderFilePath);
			fragmentString = LoadFile(FragmentShaderFilePath);
			Compile();
		}

		void Compile(std::string Name, const char* VertexShaderFilePath, const char* GeometryShaderFilePath, const char* FragmentShaderFilePath,bool initializeDefaultAttribPointers)
		{
			name = Name;
			vertexString   = LoadFile(VertexShaderFilePath);
			geometryString = LoadFile(GeometryShaderFilePath);
			fragmentString = LoadFile(FragmentShaderFilePath);
			Compile();
		}

		void BlitDrawBuffer(int width,int height)
		{

			glBlitNamedFramebuffer(drawBuffer, 0,0,0, width, height, 0, 0, width, height,
				GL_COLOR_ATTACHMENT0, GL_NEAREST);
		}

		void SetTextureUniform(const char* name, int unit, int texture)
		{
			checkGLError("Texture uniform set check");
			//glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, name), unit);
			checkGLError("Texture uniform active check");
			glActiveTexture(GL_TEXTURE0 + unit);
			checkGLError("Texture uniform bind check");
			glBindTexture(GL_TEXTURE_RECTANGLE, texture);
			checkGLError("post Texture uniform set check");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE, 0);
		}

		void Compile()
		{
			id = glCreateProgram();

			cout << "\nShader Log: " << name << " \n-> Program ID: " << id << endl;
	
			bool ready = true;
			GLuint vid = CreateFromString(GL_VERTEX_SHADER, vertexString.c_str());
			if (vid > 0)
				v_id = vid;
			else
				ready = false;

			//checkGLError("Shader pre geometry shader gl check:");
			if (geometryString.length() > 0)
			{
				if (debug)
				cout << name << " Geometry shader..." << endl;
				GLuint gid = CreateFromString(GL_GEOMETRY_SHADER, geometryString.c_str());
				if (gid > 0)
					g_id = gid;
				else
					ready = false;
			}

			if (tessellationControlString.length() > 0)
			{
				if (debug)
					cout << name << " Tess Control shader..." << endl;
				GLuint tcid = CreateFromString(GL_TESS_CONTROL_SHADER, tessellationControlString.c_str());
				if (tcid > 0)
					tc_id = tcid;
				else
					ready = false;
			}

			if (tessellationEvaluationString.length() > 0)
			{
				if (debug)
					cout << name << " Tess Evaluation shader..." << endl;
				GLuint teid = CreateFromString(GL_TESS_EVALUATION_SHADER, tessellationEvaluationString.c_str());
				if (teid > 0)
					te_id = teid;
				else
					ready = false;
			}

			GLuint fid = CreateFromString(GL_FRAGMENT_SHADER, fragmentString.c_str());
			if (fid > 0)
				f_id = fid;
			else
				ready = false;

			if (ready)
			{
				checkGLError(name + "Shader pre link gl check:");
				glAttachShader(id, v_id);
				if (geometryString.length() > 0)
					glAttachShader(id, g_id);
				if (tessellationControlString.length() > 0)
					glAttachShader(id, te_id);
				if (tessellationEvaluationString.length() > 0)
					glAttachShader(id, tc_id);
				glAttachShader(id, f_id);
			
				glLinkProgram(id);
				if (!CheckLinking())
				{
					glDetachShader(id, v_id);
					glDeleteShader(v_id);
					if (geometryString.size() > 0)
					{
						glDetachShader(id, g_id);
						glDeleteShader(g_id);
					}
					glDetachShader(id, f_id);
					glDeleteShader(f_id);
				
					return;
				}
				else {
					initialized = true;
				}
			}
			if (!initialized)
				return;

			checkGLError(name + " post compile gl check:");

			bool check_max_uniforms = false;
			if (check_max_uniforms)
			{
				int max_v_uniforms;
				int max_f_uniforms;
				glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_v_uniforms);
				glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_f_uniforms);
				cout << ":: max vertex uniforms: " << max_v_uniforms << endl;
				cout << ":: max fragment uniforms: " << max_v_uniforms << endl;
				cout << endl;
			}
			checkGLError(string(name + " Post compile check"));
			cout << endl;
			glUseProgram(0);
		}

		bool CheckLinking()
		{
			int rvalue;
			glGetProgramiv(id, GL_LINK_STATUS, &rvalue);
			if (!rvalue) {
				cout<<"\n-> Link Unsuccessfull"<<endl;
				fprintf(stderr, "Error in linking shader program ", name);
				GLchar log[10240];
				GLsizei length;
				glGetProgramInfoLog(id, 10239, &length, log);
				fprintf(stderr, "Linker log:\n%s\n", log);
				return false;
				//exit(41);
			}
			cout<<"-> Link Successfull"<<endl;
			return true;
		}

		int GetUniformLocation(char* variableName)
		{
			return glGetUniformLocation(id, variableName);
		}

		int GetUniformLocation(const char* variableName)
		{
			return glGetUniformLocation(id, variableName);
		}

		void Select()
		{
			glUseProgram(id);
		}

		static std::string LoadFile(const std::string& path)
		{
			return LoadFile(path.c_str());
		}

		static std::string LoadFile(const char* path)
		{
			std::string str;
			//cout << "\nRenderer: file name: " << path << "\n";
			std::ifstream stream;
			stream.open(path);

			if (!stream.good())
			{
				cout << "-> failed to load file: " << path << endl;
				return "";
			}
			else
			{
				//cout << "-> file is good" << endl;
			}

			char tmp[MAX_SHADERFILE_LINES];
			while (!stream.eof())
			{
				stream.getline(tmp, MAX_SHADERFILE_LINES);
				str += tmp;
				str += "\n";
			}
			//shaderString = str;
			return str;
		}

		unsigned int CreateShaderFromFile(unsigned int shaderprogram, GLenum type, const char* filePath)
		{
			//DebugString(" \nglx::Shader ->> Creating Shader: program ");
			//cout << shaderprogram << " :type: " << type << endl;

			//creat vertex shader
			std::string shaderfile = LoadFile(filePath);

			const char* shaderString = shaderfile.c_str();
			int ShaderID = Create(type, shaderString);

			glAttachShader(shaderprogram, ShaderID);

			glLinkProgram(shaderprogram);

			//cout << "\n glx::Shader: Program ID: " << shaderprogram << endl;
			//DebugShaders(shaderprogram);

			return ShaderID;
		}

		static const char* GetShaderType(GLenum Type)
		{
			switch (Type)
			{
			case GL_VERTEX_SHADER:		return "Vertex Shader";
			case GL_GEOMETRY_SHADER:	return "Geometry Shader";
			case GL_FRAGMENT_SHADER:	return "Fragment Shader";
			case GL_COMPUTE_SHADER:		return "Compute Shader";
			case GL_TESS_CONTROL_SHADER: return "Tessellation Control Shader";
			case GL_TESS_EVALUATION_SHADER: return "Tessellation Evaulation Shader";
			}
			return "Shader Type not recognised";
		}

		GLuint CreateFromString(GLenum type, const char* fileString)
		{
			//creat vertex shader
			std::string shaderfile;
			const char* shaderString = fileString; 
			unsigned int ShaderID = Create(type, shaderString);
			checkGLError("...create from string gl check:");
			if(debug)
			cout <<"Compiling Shader: "<< GetShaderType(type) << endl;

			if (!CheckShader(ShaderID,type))
				return -1;

			CompileSuccessful(ShaderID);
			return ShaderID;
		}
		
		bool CompileSuccessful(int obj)
		{
			int status;
			glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
			checkGLError("...create from string gl check:");
			if (status != GL_TRUE)
			{
				cout<<"\n<-Compile Unsuccessfull"<<endl;
			}
			else {
				if (debug)
				cout<<"\n<-Compile Successfull"<<endl;
			}
			return status;
		}

		GLuint Create(GLenum type, const char* src)
		{
			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &src, NULL);
			glCompileShader(shader);
			return shader;
		}

		bool CheckShader(GLuint shaderID,GLenum type)
		{
			GLint isCompiled = 0;
			//glUseProgram(id);
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_TRUE)
			{
				//cout << "...Shader ID: " << shaderID << " is Compiled\n" << endl;
				return true;
			}
			else {
				const char* type_name = GetShaderType(type);
				cout << "SHADER ERROR!:"<<name<<" "<<type_name<<" failed to compile" << endl;
				cout << "::Name: " << name << endl;
				cout << "::Type: " << type_name << endl;
				const GLubyte* version = glGetString(GL_VERSION);
				cout << "->> glx ShaderLog: OpenGL Version " << version << endl;
				const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
				cout << "->> GLSL Version " << shaderVersion << endl;

				GLint logSize = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
				//cout << "->>>log Size: " << logSize << endl;
				if (logSize > 0)
				{
					std::vector<GLchar> errorLog(logSize);
					glGetShaderInfoLog(shaderID, logSize, &logSize, &errorLog[0]);
					cout << "->>> problem:" << endl;

					for (int i = 0; i < logSize; i++)
						cout << errorLog[i];
				}
				else {
					cout << "->>>> DOUBLE WAMMY!  Log size is 0!" << endl;
				}

				cout << "->>>> Shader Compilation FAILED!" << endl;
				//glDeleteShader(shaderID); // Don't leak the shader.
				return false;
			}
		}

		void SetProjection(const mat4& mat)
		{
			SetUniform4x4("projection", mat);
		}
		void SetView(const mat4& mat)
		{
			SetUniform4x4("view", mat);
		}
		void SetModel(const mat4& mat)
		{
			SetUniform4x4("model", mat);
		}
		void SetColour(const vec4& col)
		{
			SetUniform4f("colour", col);
		}

		void SetUniform4f(int uniformID, float x, float y, float z, float w)
		{
			glProgramUniform4f(id, uniformID, x, y, z, w);
		}
		void SetUniform2f(int uniformID, float x, float y)
		{
			glProgramUniform2f(id, uniformID, x, y);
		}

		void SetUniform3f(int uniformID, float x, float y, float z)
		{
			glProgramUniform3f(id, uniformID, x, y, z);
		}

		void SetUniform3x3(int uniformID, const glm::mat3& mat)
		{
			glProgramUniformMatrix3fv(id, uniformID, 1, GL_FALSE, &mat[0][0]);
		}

		void SetUniform3x3(const std::string& uniformID, const glm::mat3& mat)
		{
			int uniform = GetUniformID(uniformID);
			glProgramUniformMatrix3fv(id, uniform, 1, GL_FALSE, &mat[0][0]);
		}


		void SetUniform4x4(int uniformID, const glm::mat4& mat)
		{
			glProgramUniformMatrix4fv(id, uniformID, 1, GL_FALSE, &mat[0][0]);
		}

		void SetUniform1i(int uniformID, int i)
		{
			glProgramUniform1i(id, uniformID, i);
		}

		void SetUniform1i(const std::string& uniformID, int i)
		{
			glProgramUniform1i(id, GetUniformID(uniformID), i);
		}

		void SetUniform4x4(const std::string& uniformID, const glm::mat4& mat)
		{
			int uniform = GetUniformID(uniformID);
			glProgramUniformMatrix4fv(id, uniform, 1, GL_FALSE, &mat[0][0]);
		}

		void SetUniform4x4Array(const std::string& uniformID, int count, bool transpose, const std::vector<glm::mat4> mat)
		{
			glUniformMatrix4fv(GetUniformID(uniformID), count, transpose, glm::value_ptr(mat.at(0)));
			//glProgramUniformMatrix4fv(id, GetUniformID(uniformID), 1, GL_FALSE, &mat[0][0]);
		}

		void SetUniform4x4Array(const std::string& uniformID, int count, bool transpose, const mat4* mat)
		{
			glUniformMatrix4fv(GetUniformID(uniformID), count, transpose,&mat[0][0][0]);
			//glProgramUniformMatrix4fv(id, GetUniformID(uniformID), 1, GL_FALSE, &mat[0][0]);
		}

		void SetUniform4f(const std::string& uniformID, const glm::vec4& vec)
		{
			SetUniform4f(uniformID, vec.x, vec.y, vec.z, vec.w);
		}
		void SetUniform3f(const std::string& uniformID, const glm::vec3& vec)
		{
			SetUniform3f(uniformID, vec.x, vec.y, vec.z);
		}
		int SetUniform2i(const std::string& uniformID, const glm::ivec2& vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform2i(id, uid, vec.x, vec.y);
			return uid;
		}
		int SetUniform3i(const std::string& uniformID, const glm::ivec3& vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform3i(id, uid, vec.x, vec.y, vec.z);
			return uid;
		}
		int SetUniform4i(const std::string& uniformID, const glm::ivec4& vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform4i(id, uid, vec.x, vec.y, vec.z, vec.w);
			return uid;
		}

		int SetUniform4vArray(const std::string& uniformID, int count,const std::vector<glm::vec4>& vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform4fv(id, uid,count, glm::value_ptr(vec.at(0)));
			return uid;
		}

		int SetUniform4vArray(const std::string& uniformID, int count, const vec4* vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform4fv(id, uid, count, &vec[0][0]);
			return uid;
		}

		int SetUniform3vArray(const std::string& uniformID, int count, const std::vector<glm::vec3>& vec)
		{
			int uid = GetUniformID(uniformID);
			glProgramUniform3fv(id, uid, count, glm::value_ptr(vec.at(0)));
			return uid;
		}

		void SetUniform2f(const std::string& uniformID, const glm::vec2& vec)
		{
			SetUniform2f(uniformID, vec.x, vec.y);
		}

		void SetUniform4f(const std::string& uniformID, float x, float y, float z, float w)
		{
			int u_id = GetUniformID(uniformID);
			glProgramUniform4f(id, u_id, x, y, z, w);
		}

		void SetUniform3f(const std::string& uniformID, float x, float y, float z)
		{
			glProgramUniform3f(id, GetUniformID(uniformID), x, y, z);
		}

		void SetUniform2f(const std::string& uniformID, float x, float y)
		{
			glProgramUniform2f(id, GetUniformID(uniformID), x, y);
		}

		int SetUniform1f(const std::string& uniformID, float f)
		{
			int ui = GetUniformID(uniformID);
			glProgramUniform1f(id, ui, f);
			return ui;
		}

		void SetUniform1f(int uniformID, float f)
		{
			glProgramUniform1f(id, uniformID, f);
		}

		void SetUniformIntArray(int uniformID, int sizeoflist, int* list)
		{
			glProgramUniform1iv(id, uniformID, sizeoflist, list);
		}

		void SetUniformPointer(const std::string& locationName,GLuint64EXT address)
		{
			unsigned int uniformLocation = glGetUniformLocation(id, locationName.c_str());
			glProgramUniformui64NV(id, uniformLocation, address);
		}

		void SetUniformPointer(unsigned int uniformLocation,GLuint64EXT address)
		{
			glProgramUniformui64NV(id, uniformLocation, address);
		}

		static int GetProgram()
		{
			int id;
			glGetIntegerv(GL_CURRENT_PROGRAM, &id);
			return id;
		}

		int GetNamedUniformID(const char* name)
		{
			if (id < 0)
				return -1;
			int uid = glGetUniformLocation(id, name);
			return uid;
		}
		
		void Reset()
		{
			glUseProgram(0);
		}

		static void DrawQuad()
		{
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		static void DrawQuadInstanced(int numInstances)
		{
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numInstances);
		}
	};
}

