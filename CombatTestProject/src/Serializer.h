#pragma once
#include "NativeHeader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <experimental\filesystem>

nativenamespace{

	struct path_leaf_string
	{
		std::string operator()(const std::experimental::filesystem::directory_entry& entry) const
		{
			return entry.path().string();
		}
	};

	class Save
	{
	private:
		static std::ofstream _outstream;
		static std::ifstream _instream;

	public:
		
		Save() : outstream(&_outstream) , instream(&_instream)
		{
		}
		

		std::string file;
		std::ofstream* outstream;
		std::ifstream* instream;

		std::ofstream& Out()
		{
			return *outstream;
		}
		std::ifstream& In()
		{
			return *instream;
		}
		
		bool debug = false;

		void CreateTextFile(const string& filePath)
		{
			std::ofstream _file;
			_file.open(filePath);
			_file.close();
			file = filePath;
		}

		void CreateTextFile()
		{
			if (file == "")
			{
				cout << ">>>>>>ERROR!<<<<<< Serializtion: no file name provided" << endl;
				return;
			}
			std::ofstream _file;
			_file.open(file);
			_file.close();
		}

		bool Exists()
		{
			std::ifstream _file(file);
			return (bool)_file;
		}

		static bool FileExists(const string& filePath)
		{
			std::ifstream _file(filePath);
			return (bool)_file;
		}

		std::ofstream& EnableOutStream()
		{
			(*outstream).open(file, std::ios_base::app);
			return (*outstream);
		}
		void CloseOutStream()
		{
			(*outstream).close();
		}
		void AddLine(const string& field)
		{
			(*outstream) << field << endl;
		}

		std::ofstream EnableFileStream()
		{

			std::ofstream fileStream;
			fileStream.open(file, std::ios_base::app);
			return fileStream;
		}
		void CloseFileStream(std::ofstream& stream)
		{
			stream.close();
		}
		void AddLineToStream(std::ofstream& stream,const string& field)
		{
			stream << field << endl;
		}

		void OpenAndAddLine(const std::string& field)
		{
			std::ofstream _file;
			_file.open(file, std::ios_base::app);
			_file << field << "\n";
			_file.close();
			if(debug)
			cout << "SAVED:\n"<<field<<"\n...TO FILE..." << file << endl;
		}

		static float ExtractAsFloat(const std::string& line)
		{
			return std::stof(line);
		}

		static double ExtractAsDouble(const std::string& line)
		{
			return std::stod(line);
		}

		static int ExtractAsInt(const std::string& line)
		{
			return std::stoi(line);
		}

		static unsigned long ExtractAsUlong(const std::string& line)
		{
			return std::stoul(line);
		}

		static long ExtractAsLong(const std::string& line)
		{
			return std::stol(line);
		}

		static long long ExtractAsLongLong(const std::string& line)
		{
			return std::stoll(line);
		}

		static unsigned long long ExtractAsULongLong(const std::string& line)
		{
			return std::stoull(line);
		}

		static mat4 ExtractAsMat4(const string& delimit,const string& line)
		{
			std::vector<string> output;
			Save::DelimitSplit(delimit, line, output);
			mat4 mat;
			mat[0][0] = std::stof(output[0]);
			mat[0][1] = std::stof(output[1]);
			mat[0][2] = std::stof(output[2]);
			mat[0][3] = std::stof(output[3]);

			mat[1][0] = std::stof(output[4]);
			mat[1][1] = std::stof(output[5]);
			mat[1][2] = std::stof(output[6]);
			mat[1][3] = std::stof(output[7]);

			mat[2][0] = std::stof(output[8]);
			mat[2][1] = std::stof(output[9]);
			mat[2][2] = std::stof(output[10]);
			mat[2][3] = std::stof(output[11]);

			mat[3][0] = std::stof(output[12]);
			mat[3][1] = std::stof(output[13]);
			mat[3][2] = std::stof(output[14]);
			mat[3][3] = std::stof(output[15]);
			return mat;
		}

		static string Delimit(const string& delimiter, const string& line)
		{
			size_t pos = 0;
			string l = line;
			while ((pos = l.find(delimiter)) != std::string::npos) {
			//	token = l.substr(0, pos);
				l.erase(0, pos + delimiter.length());
			}
			//std::cout << l << std::endl;
			return l;
		}

		static void DelimitLine(const string& identifier,const string& delimiter, const string& line, std::vector<string>& output)
		{
			size_t pos = 0;
			if ((pos = line.find(identifier)) == std::string::npos) {
				//cout << "Serializer:  DelimitLine: Identifier not found: " << identifier << endl;
				return;
			}
			//pos = line.find(identifier);
			string sub_string = line;
			sub_string.erase(0, pos + identifier.length());
			DelimitSplit(delimiter, sub_string, output);
		}
		static string DelimitLine(const string& identifier, const string& line)
		{
			size_t pos = 0;
			if ((pos = line.find(identifier)) == std::string::npos) {
				//cout << "Serializer:  DelimitLine: Identifier not found: " << identifier << endl;
				return "";
			}
			//pos = line.find(identifier);
			string sub_string = line;
			sub_string.erase(0, pos + identifier.length());
			return sub_string;
		}

		static void DelimitSplit(const string& delimiter, const string& line,std::vector<string>& output)
		{
			size_t pos = 0;
			string token;
			string l = line;
			
			while ((pos = l.find(delimiter)) != std::string::npos) {
				token = l.substr(0, pos);
				//std::cout << token << endl;
				if(token != "")
					output.push_back(token);
				l.erase(0, pos + delimiter.length());
				//output.push_back(l);
				//std::cout << l << endl;
			}
			if(l != "")
				output.push_back(l);
			//std::cout << l << endl;
		}

		static string GetFileName(const string& filepath)
		{
			std::vector<string> output;
			DelimitSplit("\\", filepath, output);
			if (output.size() == 0)
				return "Unable to get file name";
			for (string& s : output)
			{
				size_t pos = 0;
				if ((pos = s.find(".")) != std::string::npos)
				{
					std::vector<string> splitname;
					DelimitSplit(".", s,splitname);
					if (splitname.size() == 2)
						return splitname[0];
				}
			}
			return "Unable to get file name";
		}

		int GetInt(const string& delimiter)
		{
			std::vector<std::string> lines = GetLines(file,delimiter);
			if (lines.size() == 0)
				return -1;
			string s = Delimit(delimiter, lines[0]);
			return ExtractAsInt(s);
		}

		std::vector<string> GetStrings(const string& delimiter)
		{
			std::vector<std::string> lines = GetLines(file, delimiter);
			int linesize = lines.size();
			std::vector<std::string> dl;
			if (linesize == 0)
				return dl;
			
			dl.resize(linesize);
			for (int i = 0; i < linesize; i++)
				dl[i] = Delimit(delimiter, lines[i]);
			return dl;
		}

		std::vector<string> GetLines(const string& filePath,const string& line)
		{
			std::ifstream _file(filePath);
			string l;
			std::vector<string> lines;
			if (_file.is_open())
			{
				while (std::getline(_file, l))
				{
					std::size_t found = l.find(line);
					if (found != string::npos)
						lines.push_back(l);
				}
				_file.close();
			}
			return lines;
		}

		bool GetAllLines(std::vector<string>& lines)
		{
			std::ifstream _file(file);
			//_file.open(file);
			if (!_file.good())
			{
				cout << "Serializer: error reading file: " << file << endl;
				return false;
			}
			string l;
			if (_file.is_open())
			{
				while (std::getline(_file, l))
					lines.push_back(l);
				_file.close();
			}
			return true;
		}

		void OpenForReadAppend(std::ifstream& readStream,std::ofstream& writeStream)
		{
			readStream = std::ifstream(file, std::ios_base::app | std::ios_base::out);
			writeStream = std::ofstream(file, std::ios_base::app | std::ios_base::in);
		}

		void AppendLine(std::ifstream& readStream, std::ofstream& appendStream, const string& line)
		{
			if (!readStream.good())
				return;
			string text = "";
			while (getline(readStream,text)) 
			{
				if (text == line)
					return;
			}
			appendStream << line<<"\n";
		}

		void AddString(const string& field, const string& value)
		{
			std::ostringstream ss1;
			ss1 << field.c_str() << value;
			OpenAndAddLine(string(ss1.str()));
		}

		void AddInt(const std::string& field, int value)
		{
			std::ostringstream ss1;
			ss1 << field.c_str() << value;
			OpenAndAddLine(std::string(ss1.str()));
		}

		void AddVec3(const std::string& field, const glm::vec3& vec)
		{
			std::ostringstream ss1;
			ss1 << field.c_str() << "X::" << vec.x;
			OpenAndAddLine(std::string(ss1.str()));

			std::ostringstream ss2;
			ss2 << field.c_str() << "Y::" << vec.y;
			OpenAndAddLine(std::string(ss2.str()));

			std::ostringstream ss3;
			ss3 << field.c_str() << "Z::" << vec.z;
			OpenAndAddLine(std::string(ss3.str()));
		}

		void AddVec4(const std::string& field, const glm::vec4& vec)
		{
			std::ostringstream ss1;
			ss1 << field.c_str() << "X::" << vec.x;
			OpenAndAddLine(std::string(ss1.str()));

			std::ostringstream ss2;
			ss2 << field.c_str() << "Y::" << vec.y;
			OpenAndAddLine(std::string(ss2.str()));

			std::ostringstream ss3;
			ss3 << field.c_str() << "Z::" << vec.z;
			OpenAndAddLine(std::string(ss3.str()));

			std::ostringstream ss4;
			ss4 << field.c_str() << "W::" << vec.w;
			OpenAndAddLine(std::string(ss4.str()));
		}

		static string MakeMat4Line(const char* delimit, const mat4& mat)
		{
			std::ostringstream ss1;
			ss1 << delimit << mat[0][0] 
				<< delimit << mat[0][1] 
				<< delimit << mat[0][2] 
				<< delimit << mat[0][3] 
				<< delimit << mat[1][0] 
				<< delimit << mat[1][1] 
				<< delimit << mat[1][2] 
				<< delimit << mat[1][3]
				<< delimit << mat[2][0]
				<< delimit << mat[2][1]
				<< delimit << mat[2][2]
				<< delimit << mat[2][3]
				<< delimit << mat[3][0]
				<< delimit << mat[3][1]
				<< delimit << mat[3][2]
				<< delimit << mat[3][3]
				<< delimit;
			return string(ss1.str());
		}

		static string MakeVec4Line(const char* delimit, const vec4& vec)
		{
			std::ostringstream ss1;
			ss1 << delimit << vec.x << delimit << vec.y << delimit << vec.z << delimit << vec.w << delimit;
			return string(ss1.str());
		}

		void AddQuat(const std::string& field, const glm::quat& vec)
		{
			std::ostringstream ss1;
			ss1 << field.c_str() << "X::" << vec.x;
			OpenAndAddLine(std::string(ss1.str()));

			std::ostringstream ss2;
			ss2 << field.c_str() << "Y::" << vec.y;
			OpenAndAddLine(std::string(ss2.str()));

			std::ostringstream ss3;
			ss3 << field.c_str() << "Z::" << vec.z;
			OpenAndAddLine(std::string(ss3.str()));

			std::ostringstream ss4;
			ss4 << field.c_str() << "W::" << vec.w;
			OpenAndAddLine(std::string(ss4.str()));
		}

		static vec2 ExtractAsVec2(const string& delimiter, const string& line)
		{
			vec2 pos;
			std::vector<string> lines;
			DelimitSplit(delimiter, line, lines);

			assert(lines.size() == 2);
			pos.x = std::stof(lines[0]);
			pos.y = std::stof(lines[1]);
			return pos;
		}

		static vec3 ExtractAsVec3(const string& delimiter, const string& line)
		{
			vec3 pos;
			std::vector<string> lines;
			DelimitSplit(delimiter, line, lines);

			assert(lines.size() == 3);
			pos.x = std::stof(lines[0]);
			pos.y = std::stof(lines[1]);
			pos.z = std::stof(lines[2]);
			return pos;
		}

		static vec4 ExtractAsVec4(const string& delimiter, const string& line)
		{
			vec4 pos;
			std::vector<string> lines;
			DelimitSplit(delimiter, line, lines);

			assert(lines.size() == 4);
			pos.x = std::stof(lines[0]);
			pos.y = std::stof(lines[1]);
			pos.z = std::stof(lines[2]);
			pos.w = std::stof(lines[3]);
			return pos;
		}

		static uvec4 ExtractAsUVec4(const string& delimiter, const string& line)
		{
			uvec4 pos;
			std::vector<string> lines;
			DelimitSplit(delimiter, line, lines);

			assert(lines.size() == 4);
			pos.x = std::stoul(lines[0]);
			pos.y = std::stoul(lines[1]);
			pos.z = std::stoul(lines[2]);
			pos.w = std::stoul(lines[3]);
			return pos;
		}

		//dep?  I think the camera class uses these
		static glm::vec3 ExtractVec3(const std::string& field, std::vector<std::string> lines)
		{
			glm::vec3 pos;

			std::string X = Save::Delimit(field + "X::", lines[0]);
			pos.x = Save::ExtractAsFloat(X);

			std::string Y = Save::Delimit(field + "Y::", lines[1]);
			pos.y = Save::ExtractAsFloat(Y);

			std::string Z = Save::Delimit(field + "Z::", lines[2]);
			pos.z = Save::ExtractAsFloat(Z);
			return pos;
		}

		static glm::vec4 ExtractVec4(const std::string& field, std::vector<std::string> lines)
		{
			glm::vec4 pos;

			std::string X = Save::Delimit(field + "X::", lines[0]);
			pos.x = Save::ExtractAsFloat(X);

			std::string Y = Save::Delimit(field + "Y::", lines[1]);
			pos.y = Save::ExtractAsFloat(Y);

			std::string Z = Save::Delimit(field + "Z::", lines[2]);
			pos.z = Save::ExtractAsFloat(Z);

			std::string W = Save::Delimit(field + "W::", lines[3]);
			pos.w = Save::ExtractAsFloat(W);
			return pos;
		}

		static vec4 ExtractVec4(const std::vector<string>& lines)
		{
			vec4 pos;
			assert(lines.size() > 3);
			pos.x = Save::ExtractAsFloat(lines[0]);

			pos.y = Save::ExtractAsFloat(lines[1]);

			pos.z = Save::ExtractAsFloat(lines[2]);

			pos.w = Save::ExtractAsFloat(lines[3]);
			return pos;
		}

		static glm::quat ExtractQuat(const std::string& field, std::vector<std::string> lines)
		{
			glm::quat pos;

			std::string X = Save::Delimit(field + "X::", lines[0]);
			pos.x = Save::ExtractAsFloat(X);

			std::string Y = Save::Delimit(field + "Y::", lines[1]);
			pos.y = Save::ExtractAsFloat(Y);

			std::string Z = Save::Delimit(field + "Z::", lines[2]);
			pos.z = Save::ExtractAsFloat(Z);

			std::string W = Save::Delimit(field + "W::", lines[3]);
			pos.w = Save::ExtractAsFloat(W);
			return pos;
		}

		static string SplitLine(const string& delimiter, const string line, string& remainder)
		{
			size_t pos = 0;
			string l = line;
			string token;
			while ((pos = l.find(delimiter)) != std::string::npos) {
				token = l.substr(0, pos);
				l.erase(0, pos + delimiter.length());
			}
			remainder = l;
			//std::cout << l << std::endl;
			return token;
		}

		static string SerializeInt(const string& delimiter, int i)
		{
			std::ostringstream ss;
			ss << delimiter << i << delimiter;
			return ss.str();
			//return delimiter + "||" + to_string(i) + "||" + delimiter;
		}

		static string SerializeVec2(const vec2& vec)
		{
			std::ostringstream ss;
			ss << "||" << vec.x << +"||" << vec.y << "||";
			return ss.str();
		}

		static string SerializeVec2(const string& delimiter, const vec2& vec)
		{
			std::ostringstream ss;
			ss<< delimiter << "||" << vec.x << +"||" << vec.y << "||" << delimiter;
			return ss.str();
		}

		static string SerializeVec3(const vec3& vec)
		{
			std::ostringstream ss;
			ss << "||" << vec.x << +"||" << vec.y << "||" << vec.z << "||";
			return ss.str();
		}

		static string SerializeVec3(const string& delimiter, const vec3& vec)
		{
			std::ostringstream ss;
			ss << delimiter <<"||" << vec.x << + "||" << vec.y << "||" << vec.z<< "||"<<delimiter;
			return ss.str();
		}

		static string SerializeVec4(const vec4& vec)
		{
			std::ostringstream ss;
			ss << "||" << vec.x << +"||" << vec.y << "||" << vec.z << "||" << vec.w << "||";
			return ss.str();
		}

		static string SerializeVec4(const string& delimiter, const vec4& vec)
		{
			std::ostringstream ss;
			ss << delimiter << "||" << vec.x << +"||" << vec.y << "||" << vec.z << "||" << vec.w << "||" << delimiter;
			return ss.str();
		}

		static string SerializeUVec4(const uvec4& vec)
		{
			std::ostringstream ss;
			ss << "||" << vec.x << +"||" << vec.y << "||" << vec.z << "||" << vec.w << "||";
			return ss.str();
		}

		static string SerializeUVec4(const string& delimiter, const uvec4& vec)
		{
			std::ostringstream ss;
			ss << delimiter <<"||" << vec.x << +"||" << vec.y << "||" << vec.z << "||" << vec.w << "||" <<delimiter;
			return ss.str();
		}

		static void GetAllFilesTypes(const std::string& name, std::vector<string>& v)
		{
			std::experimental::filesystem::path p(name);
			std::experimental::filesystem::directory_iterator start(p);
			std::experimental::filesystem::directory_iterator end;
			std::transform(start, end, std::back_inserter(v), path_leaf_string());
		}
	};
}
