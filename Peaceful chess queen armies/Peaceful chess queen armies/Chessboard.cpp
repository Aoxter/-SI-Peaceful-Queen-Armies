#include<windows.h>
#include<glut.h>
#include <iostream>
using namespace std;

int c = 1;
int chessboard_size = 4;
// n must be +1 larger than chessboard size
int n = chessboard_size;
int x_global = n * 100;
int y_global = n * 80;
int x_step_global = 100;
int y_step_global = 80;
int army_size = 1;
int d, offset = 2;

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
	std::cout << "drawing square " << std::endl;
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

void triangle(int a, int b, int c, int d, int e, int f) //display unfilled triangle
{
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2i(a, b);
	glVertex2i(c, d);
	glVertex2i(e, f);
	glEnd();


}
void rectangle(int a, int b, int c, int d) //display filled rectangle
{
	glBegin(GL_POLYGON);
	glVertex2i(a, b);
	glVertex2i(c, b);
	glVertex2i(c, d);
	glVertex2i(a, d);
	glEnd();

}
void circle(int x, int y, int r) //display filled circle of radius 'r'
{

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(r);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();

}

void queen(int x, int y, int color)
//function to display Queen wrt to (x,y) position. (x,y) is center of the Queen
{
	std::cout << "Hello2" << std::endl;
	if (color == 0)
		glColor3ub(57, 94, 144);
	else if (color == 1)
		glColor3ub(30, 5, 34);
	else
		glColor3ub(36, 185, 26);
	x = x * d + offset + d / 2;
	y = y * d + offset + d / 2;
	glLineWidth(1);
	glPointSize(1);
	triangle(x - d / 25, y + d / 2.778, x + d / 25, y + d / 2.778, x, y + d / 2.439);
	rectangle(x - d / 11.11, y + d / 2.778, x + d / 11.11, y + d / 3.333);
	circle(x, y + d / 5, d / 5.555);
	rectangle(x - d / 25, y + d / 10, x + d / 25, y + d / 20);
	rectangle(x - d / 11.11, y + d / 20, x + d / 11.11, y);
	rectangle(x - d / 16.667, y, x + d / 16.667, y - d / 4.166);
	rectangle(x - d / 11.11, y - d / 4.167, x + d / 11.111, y - d / 3.448);
	rectangle(x - d / 9.09, y - d / 3.448, x + d / 9.09, y - d / 2.564);
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
	std::cout << "Hello" << std::endl;
	queen(50, 40, 2);
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