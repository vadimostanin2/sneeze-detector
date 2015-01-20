#include <vector>
#include <cmath>

#define DISCRETE 0
#define CONTINUOUS 1

using namespace std;

class DTW {
public:
	vector< vector<float> > mGamma;
	vector< vector<double> > mGamma_cont;

	enum {
		INF = 100000000
	};
	int min( int x, int y, int z );
	double min( double x, double y, double z );

	DTW( int n, int m ): mGamma( n, vector<float>( m, INF ) ) {};
	DTW( int n, int m, int flag ): mGamma_cont( n, vector<double>( m, INF ) ) {};
	~DTW() {};

	float run( vector<float> v, vector<float> w );
	float run( float * v_1, float * v_2, unsigned int v_1_length, unsigned int v_2_length );
	double run( double** v, int vlength, double** w, int wlength, int dim );
};
