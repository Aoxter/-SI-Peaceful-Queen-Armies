#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>
#include <vector>
#include<windows.h>
#include<glut.h>
#include <freeglut.h>
#include <vector>
#include <tuple>
#include <iostream>
using namespace std;


int field_color = 1;
int chessboard_size = 5;
// int n = chessboard_size;
int x_global = chessboard_size * 100;
int y_global = chessboard_size * 80;
int x_step_global = 100;
int y_step_global = 80;
vector< tuple<int, int> > placesForQueens;
// d - size of queens, offset - change position
int d = 80;
int offset = 2;
vector<int> black_queens;
vector<int> white_queens;
int color_white = 0;
int color_black = 1;


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
	// if color is 0 then draw white box and change value of color = 1
	if (field_color == 0)
	{
		glColor3f(1, 1, 1); // white color value is 1 1 1
		field_color = 1;
	}
	// if color is 1 then draw black box and change value of color = 0
	else
	{
		glColor3f(0, 0, 0); // black color value is 0 0 0
		field_color = 0;
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

void drawFilledelipse(GLfloat x, GLfloat y, GLfloat xcenter, GLfloat ycenter)
{
	int i;
	int triangleAmount = 20; //# of triangles used to draw circle
		//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * (3.14);
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			x + ((xcenter + 1) * cos(i * twicePi / triangleAmount)),
			y + ((ycenter - 1) * sin(i * twicePi / triangleAmount))
		);
	}
	glEnd();
}

void draw_queen(int x, int y, int color)
//function to display Queen wrt to (x,y) position. (x,y) is center of the Queen
{
	if (color == 0)
		glColor3ub(255, 255, 102);
	else if (color == 1)
		glColor3ub(51, 25, 0);
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
	placesForQueens.clear();
	GLint x, y;
	int x_step = x_global / chessboard_size;
	int y_step = y_global / chessboard_size;
	if (isEvenNumber(chessboard_size)) {
		for (x = 0; x <= x_global; x += x_step)
		{
			for (y = 0; y <= y_global; y += y_step)
			{
				drawSquare(x, y + y_step_global, x + x_step_global, y + y_step_global, x + x_step_global, y, x, y);
				int mid_x = x + (x_step_global / 2);
				int mid_y = y + (y_step_global / 2);
				if(mid_y < y_global && mid_x < x_global) placesForQueens.push_back(tuple<int, int>(mid_x, mid_y));
			}
		}
	}
	else {
		for (x = 0; x < x_global; x += x_step)
		{
			for (y = 0; y < y_global; y += y_step)
			{
				drawSquare(x, y + y_step_global, x + x_step_global, y + y_step_global, x + x_step_global, y, x, y);
				int mid_x = x + (x_step_global / 2);
				int mid_y = y + (y_step_global / 2);
				placesForQueens.push_back(tuple<int, int>(mid_x, mid_y));
			}
		}
	}
	//last field added here because for don't do it
	int last_mid_x = x_global - (x_step_global / 2);
	int last_mid_y = y_global - (y_step_global / 2);
	placesForQueens.push_back(tuple<int, int>(last_mid_x, last_mid_y));
	for (vector<int>::iterator i = white_queens.begin(); i != white_queens.end(); i++)
	{
		int cord_x = get<0>(placesForQueens[*i]);
		int cord_y = get<1>(placesForQueens[*i]);
		draw_queen(cord_x, cord_y, color_white);
	}
	for (vector<int>::iterator i = black_queens.begin(); i != black_queens.end(); i++)
	{
		int cord_x = get<0>(placesForQueens[*i]);
		int cord_y = get<1>(placesForQueens[*i]);
		draw_queen(cord_x, cord_y, color_black);
	}
	/*draw_queen(50, 120, 2);
	draw_queen(150, 200, 1);*/
	// Process all OpenGL routine s as quickly as possible
	glFlush();
}

//HERE BEGINS GECODE

using namespace Gecode;
using namespace std;



IntSet* A;
vector<vector<BOOL>> wyniki_white = {};
vector<vector<BOOL>> wyniki_black = {};


class QueenArmies : public IntMaximizeScript {
public:
	const int size_of_board;
	SetVar unattacked_squares,
		white_squares,
		black_squares;
	BoolVarArray white_placement,
		black_placement;
	IntVar nr_queens_placed;

	enum {
		BRANCH_NAIVE,
		BRANCH_SPECIFIC
	};

	QueenArmies(const SizeOptions& opt) :
		IntMaximizeScript(opt),
		size_of_board(opt.size()),
		unattacked_squares(*this, IntSet::empty, IntSet(0, size_of_board*size_of_board)),
		white_squares(*this, IntSet::empty, IntSet(0, size_of_board*size_of_board)),
		black_squares(*this, IntSet::empty, IntSet(0, size_of_board*size_of_board)),
		white_placement(*this, size_of_board*size_of_board, 0, 1),
		black_placement(*this, size_of_board*size_of_board, 0, 1),
		nr_queens_placed(*this, 0, size_of_board*size_of_board)
	{
		// Basic rules of the model
		for (int i = size_of_board * size_of_board; i--; ) {
			// white_placement[i] means that no blacks are allowed on A[i]
			rel(*this, white_placement[i] == (unattacked_squares || A[i]));
			// Make sure blacks and whites are disjoint.
			rel(*this, !white_placement[i] || !black_placement[i]);
			// If i in unattacked_squares, then black_placement[i] has a piece.
			rel(*this, black_placement[i] == (singleton(i) <= unattacked_squares));
		}

		// Connect optimization variable to number of pieces
		linear(*this, white_placement, IRT_EQ, nr_queens_placed);
		linear(*this, black_placement, IRT_GQ, nr_queens_placed);

		// Connect cardinality of unattacked_squares to the number of white pieces.
		IntVar unknowns = expr(*this, cardinality(unattacked_squares));
		rel(*this, nr_queens_placed <= unknowns);
		linear(*this, white_placement, IRT_EQ, unknowns);

		if (opt.branching() == BRANCH_NAIVE) {
			branch(*this, white_placement, BOOL_VAR_NONE(), BOOL_VAL_MAX());
			branch(*this, black_placement, BOOL_VAR_NONE(), BOOL_VAL_MAX());
		}
		else {
			QueenBranch::post(*this);
			assign(*this, black_placement, BOOL_ASSIGN_MAX());
		}
	}
	QueenArmies(QueenArmies& s)
		: IntMaximizeScript(s), size_of_board(s.size_of_board) {
		unattacked_squares.update(*this, s.unattacked_squares);
		white_squares.update(*this, s.white_squares);
		black_squares.update(*this, s.black_squares);
		white_placement.update(*this, s.white_placement);
		black_placement.update(*this, s.black_placement);
		nr_queens_placed.update(*this, s.nr_queens_placed);
	}
	virtual Space*
		copy(void) {
		return new QueenArmies(*this);
	}
	virtual IntVar cost(void) const {
		return nr_queens_placed;
	}
	
	virtual void
		print(std::ostream& os) const {
		os << '\t';
		vector<BOOL> temp_white;
		vector<BOOL> temp_black;
		for (int i = 0; i < size_of_board*size_of_board; ++i) {
			cout << "|";
			temp_white.push_back(white_placement[i].val());
			temp_black.push_back(black_placement[i].val());
			if (white_placement[i].assigned() && white_placement[i].val()) os << "W|";
			else if (black_placement[i].assigned() && black_placement[i].val()) os << "B|";
			else if (!white_placement[i].assigned() && !black_placement[i].assigned()) os << " ";
			else os << ".|";
			if ((i + 1) % size_of_board == 0) os << std::endl << (i != (size_of_board*size_of_board - 1) ? "\t" : "");
		}
		wyniki_white.push_back(temp_white);
		wyniki_black.push_back(temp_black);
		os << "Number of white queens: " << nr_queens_placed << std::endl << std::endl;
	}
	

	class QueenBranch : public Brancher {
	private:
		mutable int start;
		class Choice : public Gecode::Choice {
		public:
			int pos;
			bool val;
			Choice(const Brancher& b, int pos0, bool val0)
				: Gecode::Choice(b, 2), pos(pos0), val(val0) {}
			virtual void archive(Archive& e) const {
				Gecode::Choice::archive(e);
				e << pos << val;
			}
		};

		QueenBranch(Home home)
			: Brancher(home), start(0) {}
		QueenBranch(Space& home, QueenBranch& b)
			: Brancher(home, b), start(b.start) {}

	public:
		virtual bool status(const Space& home) const {
			const QueenArmies& q = static_cast<const QueenArmies&>(home);
			for (int i = start; i < q.size_of_board*q.size_of_board; ++i)
				if (!q.white_placement[i].assigned()) {
					start = i;
					return true;
				}
			return false;
		}
		virtual Gecode::Choice* choice(Space& home) {
			const QueenArmies& q = static_cast<const QueenArmies&>(home);
			int maxsize = -1;
			int pos = -1;

			for (int i = start; i < q.size_of_board*q.size_of_board; ++i) {
				if (q.white_placement[i].assigned()) continue;
				IntSetRanges ai(A[i]);
				SetVarUnknownRanges qU(q.unattacked_squares);
				Iter::Ranges::Inter<IntSetRanges, SetVarUnknownRanges> r(ai, qU);
				int size = Iter::Ranges::size(r);
				if (size > maxsize) {
					maxsize = size;
					pos = i;
				}
			}

			assert(pos != -1);
			return new Choice(*this, pos, true);
		}
		virtual Choice* choice(const Space&, Archive& e) {
			int pos, val;
			e >> pos >> val;
			return new Choice(*this, pos, val);
		}
		virtual ExecStatus commit(Space& home, const Gecode::Choice& _c,
			unsigned int a) {
			QueenArmies& q = static_cast<QueenArmies&>(home);
			const Choice& c = static_cast<const Choice&>(_c);
			bool val = (a == 0) ? c.val : !c.val;
			return me_failed(Int::BoolView(q.white_placement[c.pos]).eq(q, val))
				? ES_FAILED
				: ES_OK;
		}
		
		virtual void print(const Space&, const Gecode::Choice& _c,
			unsigned int a,
			std::ostream& o) const {
			const Choice& c = static_cast<const Choice&>(_c);
			bool val = (a == 0) ? c.val : !c.val;
			o << "w[" << c.pos << "] = " << val;
		}
		
		virtual Actor* copy(Space& home) {
			return new (home) QueenBranch(home, *this);
		}
		static void post(QueenArmies& home) {
			(void) new (home) QueenBranch(home);
		}
		virtual size_t dispose(Space&) {
			return sizeof(*this);
		}
	};
};

int pos(int i, int j, int n) {
	return i * n + j;
}

int
main(int argc, char* argv[]) {
	while (1)
	{
		wyniki_white.clear();
		wyniki_black.clear();
		int x = 0;
		cout << "Board size : " << endl;
		cin >> x;
		int searched_queens = 0;
		cout << "How many queens: " << endl;
		cin >> searched_queens;
		chessboard_size = x;
		x_global = chessboard_size * 100;
		y_global = chessboard_size * 80;
		if (x <=2 ) {
			cout<<"Board too small"<<endl;
			continue;
		}
		SizeOptions opt("QueenArmies");
		opt.size(x);
		opt.branching(QueenArmies::BRANCH_SPECIFIC);
		opt.branching(QueenArmies::BRANCH_NAIVE, "naive");
		opt.branching(QueenArmies::BRANCH_SPECIFIC, "specific");
		opt.solutions(0);
		opt.parse(argc, argv);

		// Set up the A-sets
		// A[i] will contain the values attacked by a queen at position i
		int n = opt.size();
		A = new IntSet[max(n*n, 0)];
		int *p = new int[n*n];
		int pn = 0;
		for (int i = n; i--; ) {
			for (int j = n; j--; ) {
				int dir[][2] = {
				  { 0,  1},
				  { 1,  1},
				  { 1,  0},
				  { 0, -1},
				  {-1, -1},
				  {-1,  0},
				  { 1, -1},
				  {-1,  1}
				};
				p[pn++] = pos(i, j, n);
				for (int k = 8; k--; ) {
					for (int l = 0; l < n
						&& 0 <= (i + l * dir[k][0]) && (i + l * dir[k][0]) < n
						&& 0 <= (j + l * dir[k][1]) && (j + l * dir[k][1]) < n; ++l) {
						p[pn++] = pos(i + l * dir[k][0], j + l * dir[k][1], n);
					}
				}

				A[pos(i, j, n)] = IntSet(p, pn);

				pn = 0;
			}
		}
		delete[] p;

		IntMaximizeScript::run<QueenArmies, BAB, SizeOptions>(opt);
		if (wyniki_white.empty() != 1)
		{
			int smallest = count(wyniki_white[0].begin(), wyniki_white[0].end(), 1);
			cout << smallest << endl;
			for (int i = smallest - 1; i > 0; i--) {
				auto temp_vec = wyniki_white[0];
				auto position = std::find(temp_vec.begin(), temp_vec.end(), 1);
				int index = position - temp_vec.begin();
				temp_vec.erase(temp_vec.begin() + index);
				temp_vec.insert(temp_vec.begin(), 0);
				wyniki_white.insert(wyniki_white.begin(), temp_vec);

				temp_vec = wyniki_black[0];
				position = std::find(temp_vec.begin(), temp_vec.end(), 1);
				index = position - temp_vec.begin();
				temp_vec.erase(temp_vec.begin() + index);
				temp_vec.insert(temp_vec.begin(), 0);
				wyniki_black.insert(wyniki_black.begin(), temp_vec);
			}
			if (wyniki_white.size() < searched_queens) {
				cout << "There is no answer for this number of queens" << endl;
				continue;
			}
			white_queens.clear();
			black_queens.clear();
			// DISPLAYING FOR TEST
			// cout << "Black" << endl;
			int which = 1;
			int counter = 0;
			for (auto row = wyniki_black.begin(); row != wyniki_black.end(); row++) {
				for (auto col = row->begin(); col != row->end(); col++) {
					if (*col == 1 && which==searched_queens) {
						black_queens.push_back(counter);
						// cout << counter;
					}
					counter++;
					// cout << *col;
				}
				// cout << endl;
				counter = 0;
				which++;
			}
			which = 1;
			// cout << endl << "White" << endl;
			for (auto row = wyniki_white.begin(); row != wyniki_white.end(); row++) {
				for (auto col = row->begin(); col != row->end(); col++) {
					if (*col == 1 && which == searched_queens) {
						white_queens.push_back(counter);
						// cout << counter;
					}
					counter++;
					// cout << *col;
				}
				// cout << endl;
				counter = 0;
				which++;
			}
			
			// HERE IS DISPLAYING
			for (int row = 0; row < wyniki_white.size(); row++){
				for (int i = 0; i < n*n; i++) {
					cout << "|";
					if (wyniki_white[row][i] == 1) {
						cout << "W|";
					}
					else {
						if (wyniki_black[row][i] == 1) {
							cout << "B|";
						}
						else {
							cout << ".|";
						}
					}
					if ((i + 1) % n == 0) {
						cout << endl;
					}
				}
				cout << "---------" << endl;
			}
			

			
			
			// Initialize GLUT
			glutInit(&argc, argv);
			// Set display mode
			glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
			// Set top - left display window position.
			glutInitWindowPosition(0, 0);
			// Set display window width and height
			glutInitWindowSize(x_global, y_global);
			// Create display window with the given title
			glutCreateWindow("Chess Board using OpenGL in C++");
			// Execute initialization procedure
			init();

			// Send graphics to display window
			glutDisplayFunc(chessboard);
			glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
			// Display everything and wait.
			glutMainLoop();
			
		}
	}
	return 0;
}