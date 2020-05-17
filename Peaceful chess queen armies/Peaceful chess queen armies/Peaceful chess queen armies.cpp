#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>

using namespace Gecode;
using namespace std;

IntSet* A;

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
			// w[i] means that no blacks are allowed on A[i]
			rel(*this, white_placement[i] == (unattacked_squares || A[i]));
			// Make sure blacks and whites are disjoint.
			rel(*this, !white_placement[i] || !black_placement[i]);
			// If i in U, then b[i] has a piece.
			rel(*this, black_placement[i] == (singleton(i) <= unattacked_squares));
		}

		// Connect optimization variable to number of pieces
		linear(*this, white_placement, IRT_EQ, nr_queens_placed);
		linear(*this, black_placement, IRT_GQ, nr_queens_placed);

		// Connect cardinality of U to the number of black pieces.
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
		for (int i = 0; i < size_of_board*size_of_board; ++i) {
			if (white_placement[i].assigned() && white_placement[i].val()) os << "W";
			else if (black_placement[i].assigned() && black_placement[i].val()) os << "B";
			else if (!white_placement[i].assigned() && !black_placement[i].assigned()) os << " ";
			else os << ".";
			if ((i + 1) % size_of_board == 0) os << std::endl << (i != (size_of_board*size_of_board - 1) ? "\t" : "");
		}
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
			// No non-assigned orders left
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
	int x = 0;
	cout << "Podaj rozmiar planszy : " << endl;
	cin >> x;
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
	A = new IntSet[max(n*n, 25)];
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
	return 0;
}