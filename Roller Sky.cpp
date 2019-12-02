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

constexpr auto M_PI = 3.14159265358979323846;

char* filetobuf(const char* file);	// ���� �б� �Լ�
GLuint initshader();				// ���̴� ���α׷� ����� �Լ�
GLint shaderProgramID;				// ���̴� ���α׷�

void MakeBuffer();					// ���� ����� �Լ�
GLuint GameMap_VAO[3];				// VAO ����
GLuint GameMap_VBO[6];				// VBO ����
GLuint GameMap_EBO[3];				// EBO ����
GLUquadricObj* qobj;

GLvoid drawScene(GLvoid);			// �׸��� �Լ�
GLvoid Reshape(int w, int h);		// �ٽ� �׸��� �Լ�

double NDC_x_Change(double x);		// ���� ��ǥ ����ȭ ��Ű�� �Լ�
double NDC_y_Change(double y);		// ���� ��ǥ ����ȭ ��Ű�� �Լ�

void Keyboard(unsigned char key, int x, int y);		// Ű���� ��ɾ� �޴� �Լ�
void SpecialKeyboard(int key, int x, int y);		// Ư�� Ű���� ��ɾ� �޴� �Լ�
void Timerfunction(int value);						// Ÿ�̸�

double camera_value = 5.0;
double camera_x_transfer = 0.0;
double camera_y_transfer = camera_value;
double camera_z_transfer = camera_value;

double camera_x_change = camera_x_transfer;
double camera_y_change = camera_y_transfer;
double camera_z_change = camera_z_transfer;


double character_x_transfer = 0.0;
double character_y_transfer = 0.0;
double character_z_transfer = 0.0;

double character_x_pivot = 0.0;
double character_y_pivot = 0.75;
double character_z_pivot = 0.0;

int e_check = 0;

GLfloat Square[] = {
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
unsigned int Square_Index[] = {
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

GLfloat Square_Tile_Colors[] = {
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

GLfloat Character_Colors[] = {
	/////////////////////////////////// ĳ���� �÷� �Ķ���

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f
};

GLfloat Square_Edge_Colors[] = {
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shaderProgramID = initshader();					// ���̴� ���α׷� ����				
	glGenVertexArrays(3, GameMap_VAO);				// VAO ����
	glGenBuffers(6, GameMap_VBO);					// VBO ����
	glGenBuffers(3, GameMap_EBO);					// EBO ����

	qobj = gluNewQuadric();							// �� ����

	glutDisplayFunc(drawScene);						// �׸��� �Լ�

	glutKeyboardFunc(Keyboard);						// Ű���� �Է�
	glutSpecialFunc(SpecialKeyboard);				// ����� Ű���� �Է�
	glutTimerFunc(0, Timerfunction, 1);				// Ÿ�̸�

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
	glm::vec3 cameraPos = glm::vec3(camera_x_transfer + character_x_pivot, camera_y_transfer, camera_z_transfer + character_z_pivot);
	glm::vec3 cameraDirection = glm::vec3(character_x_pivot, character_y_pivot, character_z_pivot);
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

	glm::mat4 GameMap_change = glm::mat4(1.0f);					// ���Ӹ� ��ȯ
	glm::mat4 Character_change = glm::mat4(1.0f);				// ĳ���� ��ȯ
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");


	for (double i = 0.0; i < 10.0; i += 1.0) 
	{
		GameMap_change = glm::mat4(1.0f);														// �ʱ�ȭ 
		GameMap_change = glm::translate(GameMap_change, glm::vec3(-4.5 + i * 1.0f, 0.0, -5.5));	// x ��ǥ ����

		for (double j = 0.0; j < 10.0; j += 1.0) 
		{
			GameMap_change = glm::translate(GameMap_change, glm::vec3(0.0, 0.0, 1.0));			// y ��ǥ ����
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(GameMap_change));		// ���̴��� ������ //

			///////////////���� �� Ÿ��////////////////////

			glBindVertexArray(GameMap_VAO[0]);

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

	////////////////////// ���� ĳ���� �� //////////////////
	glBindVertexArray(GameMap_VAO[2]);
	//Character_change = glm::rotate(Character_change, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	Character_change = glm::translate(Character_change, glm::vec3(character_x_pivot, character_y_pivot, character_z_pivot));
	Character_change = glm::scale(Character_change, glm::vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Character_change));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}

void Timerfunction(int value) {
	if (e_check == 0 && camera_x_transfer != 0.0) 
	{
		camera_x_transfer += camera_x_change;
		camera_z_transfer += camera_z_change;
	}
	if (e_check == 1 && camera_x_transfer != camera_value)
	{
		camera_x_transfer += camera_x_change;
		camera_z_transfer += camera_z_change;
	}
	if (e_check == 2 && camera_x_transfer != 0.0)
	{
		camera_x_transfer += camera_x_change;
		camera_z_transfer += camera_z_change;
	}
	if (e_check == 3 && camera_x_transfer != -camera_value)
	{
		camera_x_transfer += camera_x_change;
		camera_z_transfer += camera_z_change;
	}

	if (e_check == 0 || e_check == 2)
	{
		if (character_x_pivot < character_x_transfer)
			character_x_pivot += 0.5;
		if (character_x_pivot > character_x_transfer)
			character_x_pivot -= 0.5;
	}
	if (e_check == 1 || e_check == 3)
	{
		if (character_z_pivot < character_z_transfer)
			character_z_pivot += 0.5;
		if (character_z_pivot > character_z_transfer)
			character_z_pivot -= 0.5;
	}
	

	glutPostRedisplay();
	glutTimerFunc(100, Timerfunction, 1);
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
	case 'P':
		exit(0);
		break;

	case 'z':
	case 'Z':
		camera_y_transfer += 1.0f;
		glutPostRedisplay();
		break;

	case 'x':
	case 'X':
		camera_y_transfer -= 1.0f;
		glutPostRedisplay();
		break;

	case 'e':
	case 'E':
		if (e_check == 0 && camera_x_transfer == 0.0 && camera_z_transfer == camera_value)
		{
			e_check = 1;
			camera_x_change = 0.5;
			camera_z_change = -0.5;

		}

		if (e_check == 1 && camera_x_transfer == camera_value && camera_z_transfer == 0.0)
		{
			e_check = 2;
			camera_x_change = -0.5;
			camera_z_change = -0.5;
		}

		if (e_check == 2 && camera_x_transfer == 0.0 && camera_z_transfer == -camera_value)
		{
			e_check = 3;
			camera_x_change = -0.5;
			camera_z_change = 0.5;
		}

		if (e_check == 3 && camera_x_transfer == -camera_value && camera_z_transfer == 0.0)
		{
			e_check = 0;
			camera_x_change = 0.5;
			camera_z_change = 0.5;
		}

		glutPostRedisplay();
		break;

	case 'q':
	case 'Q':
		if (e_check == 0 && camera_x_transfer == 0.0 && camera_z_transfer == camera_value)
		{
			e_check = 3;
			camera_x_change = -0.5;
			camera_z_change = -0.5;

		}

		if (e_check == 3 && camera_x_transfer == -camera_value && camera_z_transfer == 0.0)
		{
			e_check = 2;
			camera_x_change = 0.5;
			camera_z_change = -0.5;
		}

		if (e_check == 2 && camera_x_transfer == 0.0 && camera_z_transfer == -camera_value)
		{
			e_check = 1;
			camera_x_change = 0.5;
			camera_z_change = 0.5;
		}

		if (e_check == 1 && camera_x_transfer == camera_value && camera_z_transfer == 0.0)
		{
			e_check = 0;
			camera_x_change = -0.5;
			camera_z_change = 0.5;
		}
		glutPostRedisplay();
		break;

	case 'a':
	case 'A':
		if(e_check == 0)
			character_x_transfer -= 1.0;
		if (e_check == 1)
			character_z_transfer += 1.0;
		if (e_check == 2)
			character_x_transfer += 1.0;
		if (e_check == 3)
			character_z_transfer -= 1.0;
		glutPostRedisplay();
		break;

	case 'd':
	case 'D':
		if (e_check == 0)
			character_x_transfer += 1.0;
		if (e_check == 1)
			character_z_transfer -= 1.0;
		if (e_check == 2)
			character_x_transfer -= 1.0;
		if (e_check == 3)
			character_z_transfer += 1.0;
		glutPostRedisplay();
		break;
	}
}

void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		camera_z_transfer -= 1.0f;;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		camera_z_transfer += 1.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		camera_x_transfer += 1.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		camera_x_transfer -= 1.0f;
		glutPostRedisplay();
		break;
	}
}

void MakeBuffer()
{
	//////////////���� �� Ÿ��////////////////

	glBindVertexArray(GameMap_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Square_Index), Square_Index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square_Tile_Colors), Square_Tile_Colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Square_Index), Square_Index, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);


	/////////////////���� �� �׵θ�///////////////

	glBindVertexArray(GameMap_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square_Edge_Colors), Square_Edge_Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	/////////////////ĳ����///////////////

	glBindVertexArray(GameMap_VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Square_Index), Square_Index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GameMap_VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Character_Colors), Character_Colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameMap_EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Square_Index), Square_Index, GL_STATIC_DRAW);
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