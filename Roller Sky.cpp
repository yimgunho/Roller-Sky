#include <GL/glew.h> 
#include <GL/freeglut.h> 
#include <GL/freeglut_ext.h> 
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <atlfile.h>
#include <cmath>
#include <GL/glm/glm.hpp> 
#include <GL/glm/ext.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

char* filetobuf(const char* file);	// ���� �б� �Լ�
GLuint initshader();				// ���̴� ���α׷� ����� �Լ�
GLint shaderProgramID;				// ���̴� ���α׷�

void MakeBuffer();					// ���� ����� �Լ�
GLuint GameMap_VAO[2];				// VAO ����
GLuint GameMap_VBO[4];				// VBO ����
GLuint GameMap_EBO[2];				// EBO ����

GLvoid drawScene(GLvoid);			// �׸��� �Լ�
GLvoid Reshape(int w, int h);		// �ٽ� �׸��� �Լ�

double NDC_x_Change(double x);		// ���� ��ǥ ����ȭ ��Ű�� �Լ�
double NDC_y_Change(double y);		// ���� ��ǥ ����ȭ ��Ű�� �Լ�

void Keyboard(unsigned char key, int x, int y);		// Ű���� ��ɾ� �޴� �Լ�
void SpecialKeyboard(int key, int x, int y);		// Ư�� Ű���� ��ɾ� �޴� �Լ�

int change = 1;
float camera_x = 0.0;
float camera_y = 10.0;
float camera_z = 10.0;

GLfloat GameMap[] = {
	/////////////////////////// ����ü ���� 8 ��
	0.5f,  0.5f, 0.5f,  
	0.5f, -0.5f, 0.5f, 
	-0.5f, -0.5f, 0.5f, 
	-0.5f,  0.5f, 0.5f,   

	0.5f,  0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f, 
	-0.5f,  0.5f, -0.5f,   

	////////////////////////// �׵θ� ����ü�� ���� �߰� 8��

	0.5f, -0.5f, 0.5f,  
	0.5f, -0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f, 0.5f, 

	0.5f,  0.5f, 0.5f, 
	-0.5f,  0.5f, 0.5f,  
	-0.5f,  0.5f, -0.5f,   
	0.5f,  0.5f, -0.5f,  
};
unsigned int GameMap_Index[] = {
	/////////////////////////////// ���� �� ebo ����� ���� �ε���
	0, 3, 1,
	1, 3, 2,
	2, 3, 7,
	2, 7, 6,
	6, 7, 5,
	5, 7, 4,
	5, 4, 0,
	5, 0, 1,
	0, 4, 3,
	3, 4, 7,
	1, 2, 5,
	2, 6, 5
};

GLfloat GameMap_Tile_Colors[] = {
	/////////////////////////////////// ���� �� Ÿ�� �÷� ���
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f
};

GLfloat GameMap_Edge_Colors[] = {
	/////////////////////////////////// ���� �� �׵θ� �÷� ������

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f
};

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 100);								// �������� ��ġ ���� 
	glutInitWindowSize(1000, 800);									// �������� ũ�� ���� 
	glutCreateWindow("Roller Sky");


	//--- GLEW �ʱ�ȭ�ϱ� 
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ 
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glEnable(GL_DEPTH_TEST);						// ���� �˻�
	shaderProgramID = initshader();					// ���̴� ���α׷� ����				
	glGenVertexArrays(2, GameMap_VAO);				// VAO ����
	glGenBuffers(4, GameMap_VBO);					// VBO ����
	glGenBuffers(2, GameMap_EBO);					// EBO ����

	glutDisplayFunc(drawScene);						// �׸��� �Լ�

	glutKeyboardFunc(Keyboard);						// Ű���� �Է�
	glutSpecialFunc(SpecialKeyboard);				// ����� Ű���� �Է�


	glutReshapeFunc(Reshape);						// �ٽ� �׸��� �Լ�
	glutMainLoop();
}

GLvoid drawScene() // �ݹ� �Լ�: ��� 
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// ���� ���� ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// ��� �ʱ�ȭ

	MakeBuffer();										// ���� ����
	glUseProgram(shaderProgramID);						// ���̴� ���α׷� ���


	/////////////////////// ���� ��ȯ ////////////////////// ��ü ����
	glm::vec3 cameraPos = glm::vec3(camera_x, camera_y, camera_z);
	glm::vec3 cameraDirection = glm::vec3(camera_x, camera_y - 10.0f, camera_z - 10.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	////////////////////// ���� ��ȯ ////////////////////// ��ü ����
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	/////////////////////// ���� �� Ÿ�� ���� ///////////////////////////

	/////////////////////// ���� ��ȯ //////////////////////

	glm::mat4 translate_GameMap_x = glm::mat4(1.0f);		// x ��ǥ �ʱ�ȭ
	glm::mat4 translate_GameMap_y = glm::mat4(1.0f);		// y ��ǥ �ʱ�ȭ
	glm::mat4 GameMap_change = glm::mat4(1.0f);				// ���� ��ȯ �ʱ�ȭ

	for (int i = -5.0f; i < 5.0f; i+=1.0f) {
		for (int j = -5.0f; j < 5.0f; j += 1.0f) {
			translate_GameMap_x = glm::mat4(1.0f);			// x ��ǥ �ʱ�ȭ 
			translate_GameMap_y = glm::mat4(1.0f);			// y ��ǥ �ʱ�ȭ

			translate_GameMap_x = glm::translate(translate_GameMap_x,	// x ��ǥ ����
				glm::vec3(i * 1.0f, 0.0, 0.0));
			translate_GameMap_x = glm::translate(translate_GameMap_x,	// y ��ǥ ����
				glm::vec3(0.0, 0.0, j * 1.0f));

			GameMap_change = translate_GameMap_x * translate_GameMap_y;	// ��ȭ ����

			// ���̴��� ������ //
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GameMap_change));

			///////////////���� �� Ÿ��////////////////////

			glBindVertexArray(GameMap_VAO[0]);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			////////////////////���� �� �׵θ�///////////

			glBindVertexArray(GameMap_VAO[1]);

			glLineWidth(5.0);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			glDrawArrays(GL_LINE_LOOP, 4, 4);
			glDrawArrays(GL_LINE_LOOP, 8, 4);
			glDrawArrays(GL_LINE_LOOP, 12, 4);
		}
		
	}

	glutSwapBuffers();
}


void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;

	case 'z':
	case 'Z':
		camera_y += 1.0f;
		glutPostRedisplay();
		break;

	case 'x':
	case 'X':
		camera_y -= 1.0f;
		glutPostRedisplay();
		break;
	}
}

void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		camera_z -= 1.0f;;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		camera_z += 1.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		camera_x += 1.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		camera_x -= 1.0f;
		glutPostRedisplay();
		break;
	}
}

void MakeBuffer()
{
	//////////////���� �� Ÿ��////////////////

	glBindVertexArray(GameMap_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GameMap), GameMap, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GameMap_Index), GameMap_Index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GameMap_Tile_Colors), GameMap_Tile_Colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GameMap_Index), GameMap_Index, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);


	/////////////////���� �� �׵θ�///////////////

	glBindVertexArray(GameMap_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GameMap), GameMap, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GameMap_Edge_Colors), GameMap_Edge_Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

}

////////////////////////////////////// �ݹ� �Լ�: �ٽ� �׸��� 

GLvoid Reshape(int w, int h) 
{
	glViewport(0, 0, w, h);
}

///////////////////////////// ����ȭ ��Ű�� �Լ�

double NDC_x_Change(double x) {
	return (x * 2) / 800 - 1.0;
}
double NDC_y_Change(double y) {
	return 1.0 - (y * 2) / 600;
}

///////////// ���̴� ����� �Լ� ///////////////////

GLuint initshader() {
	char* vertexsource, * fragmentsource;
	vertexsource = filetobuf("vertex.glvs");
	fragmentsource = filetobuf("fragment.glfs");

	GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexshader);

	GLint IsCompiled_VS;
	GLint maxLength;
	char* vertexInfoLog;
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
	if (IsCompiled_VS == FALSE) {
		glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
		vertexInfoLog = (char*)malloc(maxLength);
		glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
		free(vertexInfoLog);
		return false;
	}

	GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentshader);

	GLint IsCompiled_FS;
	char* fragmentInfoLog;
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
	if (IsCompiled_FS == FALSE) {
		glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
		fragmentInfoLog = (char*)malloc(maxLength);
		glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
		free(fragmentInfoLog);
		return false;
	}

	GLuint shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vertexshader);
	glAttachShader(shaderprogram, fragmentshader);
	glLinkProgram(shaderprogram);
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	GLint IsLinked;
	char* shaderProgramInfoLog;
	glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int*)&IsLinked);
	if (IsLinked == FALSE) {
		glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
		shaderProgramInfoLog = (char*)malloc(maxLength);
		glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);
		free(shaderProgramInfoLog);
		return false;
	}

	return shaderprogram;
}

char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;

	fopen_s(&fptr, file, "rb");
	if (!fptr) // Return NULL on failure 
		return NULL;

	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 

	return buf; // Return the buffer 
}