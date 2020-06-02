#include<windows.h>
#include<glut.h>


int c = 0;
int chessboard_size = 10;
int army_size = 1;
void init()
{
	// For displaying the window color
	glClearColor(0, 1, 1, 0);
	// Choosing the type of projection
	glMatrixMode(GL_PROJECTION);
	// for setting the transformation which here is 2D
	gluOrtho2D(0, 800, 0, 600);
}

void drawSquare(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3, GLint x4, GLint y4)
{
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
	int x_step = 800 / chessboard_size;
	int y_step = 600 / chessboard_size;
	for (x = 0; x <= 800; x += x_step)
	{
		for (y = 0; y <= 600; y += y_step)
		{
			drawSquare(x, y + 75, x + 100, y + 75, x + 100, y, x, y);
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
	glutInitWindowPosition(100, 100);
	// Set display window width and height
	glutInitWindowSize(800, 600);
	// Create display window with the given title
	glutCreateWindow("Chess Board using OpenGL in C++");
	// Execute initialization procedure
	init();
	// Send graphics to display window
	glutDisplayFunc(chessboard);
	// Display everything and wait.
	glutMainLoop();
}