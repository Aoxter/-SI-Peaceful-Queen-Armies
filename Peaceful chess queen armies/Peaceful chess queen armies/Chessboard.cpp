#include<windows.h>
#include<glut.h>
#include <iostream>
using namespace std;

int c = 1;
int chessboard_size = 10;
// n must be +1 larger than chessboard size
int n = chessboard_size;
int x_global = n * 100;
int y_global = n * 80;
int x_step_global = 100;
int y_step_global = 80;
int army_size = 1;

bool isEvenNumber(int number) {
	if (number % 2 == 0)
		return true;
	else
		return false;
}

void init()
{
	// For displaying the window color
	glClearColor(0, 1, 1, 0);
	// Choosing the type of projection
	glMatrixMode(GL_PROJECTION);
	// for setting the transformation which here is 2D
	gluOrtho2D(0, x_global, 0, y_global);
}

void drawSquare(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3, GLint x4, GLint y4)
{
	if (c == 0) std::cout << "white - ";
	else std::cout << "black - ";
	std::cout << " x1 " << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2 << " x3 " << x3 << " y3 " << y3 << " x4 " << x4 << " y4 " << y4 << std::endl;
	// if color is 0 then draw white box and change value of color = 1
	if (c == 0)
	{
		glColor3f(1, 1, 1); // white color value is 1 1 1
		c = 1;
	}
	// if color is 1 then draw black box and change value of color = 0
	else
	{
		glColor3f(0, 0, 0); // black color value is 0 0 0
		c = 0;
	}

	// Draw Square
	glBegin(GL_POLYGON);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glVertex2i(x3, y3);
	glVertex2i(x4, y4);
	glEnd();
}
void chessboard()
{
	glClear(GL_COLOR_BUFFER_BIT); // Clear display window
	GLint x, y;
	int x_step = x_global / chessboard_size;
	int y_step = y_global / chessboard_size;
	if (isEvenNumber(chessboard_size)) {
		for (x = 0; x <= x_global; x += x_step)
		{
			for (y = 0; y <= y_global; y += y_step)
			{
				drawSquare(x, y + y_step_global, x + x_step_global, y + y_step_global, x + x_step_global, y, x, y);
			}
		}
	}
	else {
		for (x = 0; x < x_global; x += x_step)
		{
			for (y = 0; y < y_global; y += y_step)
			{
				drawSquare(x, y + y_step_global, x + x_step_global, y + y_step_global, x + x_step_global, y, x, y);
			}
		}
	}	
	// Process all OpenGL routine s as quickly as possible
	glFlush();
}

int main(int agrc, char** argv)
{
	// Initialize GLUT
	glutInit(&agrc, argv);
	// Set display mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// Set top - left display window position.
	glutInitWindowPosition(0,0);
	// Set display window width and height
	glutInitWindowSize(x_global, y_global);
	// Create display window with the given title
	glutCreateWindow("Chess Board using OpenGL in C++");
	// Execute initialization procedure
	init();
	// Send graphics to display window
	glutDisplayFunc(chessboard);
	// Display everything and wait.
	glutMainLoop();
}