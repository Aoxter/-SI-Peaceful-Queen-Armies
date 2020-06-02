#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>
#include <vector>

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
tmain(int argc, char* argv[]) {
	while (1)
	{
		wyniki_white.clear();
		wyniki_black.clear();
		int x = 0;
		cout << "Podaj rozmiar planszy : " << endl;
		cin >> x;
		if (x <=2 ) {
			cout<<"Board too small"<<endl;
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
			// DISPLAYING FOR TEST
			cout << "Black" << endl;
			for (auto row = wyniki_black.begin(); row != wyniki_black.end(); row++) {
				for (auto col = row->begin(); col != row->end(); col++) {
					cout << *col;
				}
				cout << endl;
			}
			cout << endl << "White" << endl;
			for (auto row = wyniki_white.begin(); row != wyniki_white.end(); row++) {
				for (auto col = row->begin(); col != row->end(); col++) {
					cout << *col;
				}
				cout << endl;
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
		}
	}
	return 0;
}