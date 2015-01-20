#include "dtw.h"
#include <iostream>
using namespace std;

int DTW::min( int x, int y, int z )
{
	if( ( x <= y ) && ( x <= z ) ) return x;
	if( ( y <= x ) && ( y <= z ) ) return y;
	if( ( z <= x ) && ( z <= y ) ) return z;

	return 0;
}

double DTW::min( double x, double y, double z )
{
	if( ( x <= y ) && ( x <= z ) ) return x;
	if( ( y <= x ) && ( y <= z ) ) return y;
	if( ( z <= x ) && ( z <= y ) ) return z;

	return 0;
}

void calcDistance( std::vector< std::vector< float > > & table, const std::vector< float > &seq_1, const std::vector< float > &seq_2 )
{
	size_t len_1 = seq_1.size();
	size_t len_2 = seq_2.size();

    for( size_t i = 0; i< len_1; i++ )
    {
        for( size_t j = 0; j < len_2; j++ )
        {
            table[i][j] = fabs( seq_1.at(i) - seq_2.at(j) );
        }
    }
}

void calcDistance( float ** table, float * seq_1, float * seq_2, unsigned int seq_1_size, unsigned int seq_2_size )
{
    for( size_t i = 0; i< seq_1_size; i++ )
    {
        for( size_t j = 0; j < seq_2_size; j++ )
        {
            table[i][j] = fabs( seq_1[i] - seq_2[j] );
        }
    }
}

float DTW::run( float * v_1, float * v_2, unsigned int v_1_length, unsigned int v_2_length )
{
//	std::vector< std::vector< float > > table_distances( v_1_length, vector<float>( v_2_length, 0.0 ) );
	float ** table_distances = new float*[v_1_length];
	for( unsigned int i = 0; i < v_1_length ; i++ )
	{
		table_distances[i] = new float[v_2_length];
	}
	calcDistance( table_distances, v_1, v_2, v_1_length, v_2_length );



	this->mGamma[0][0] = table_distances[0][0];

	for( size_t i = 1; i < v_1_length; i++ )
	{
		this->mGamma[i][0] = table_distances[i][0] + this->mGamma[i - 1][0];
	}

	for( size_t i = 1; i < v_2_length; i++ )
	{
		this->mGamma[0][i] = table_distances[0][i] + this->mGamma[0][i - 1];
	}


	for( size_t i = 1; i < v_1_length; i++ )
	{
		for( size_t j = 1; j < v_2_length; j++ )
		{
			mGamma[i][j] = table_distances[i][j] + min( mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1] );
		}
	}

	for( unsigned int i = 0; i < v_1_length ; i++ )
	{
		delete table_distances[i];
	}
	delete table_distances;

	return mGamma[v_1_length - 1][v_2_length - 1];
}

float DTW::run( vector<float> v_1, vector<float> v_2 )
{
	return run( &v_1[0], &v_2[0], v_1.size(), v_2.size() );
}

double DTW::run( double** v, int vlength, double** w, int wlength, int dim )
{
	double cost = 0.0;

	for( int i = 1; i < wlength; i++ )
	{
		this->mGamma_cont[0][i] = INF;
	}
	for( int i = 1; i < vlength; i++ )
	{
		this->mGamma_cont[i][0] = INF;
	}
	this->mGamma_cont[0][0] = 0;

	for( int i = 1; i < vlength; i++ )
	{
		for( int j = 1; j < wlength; j++ )
		{
			cost = 0;
			for( int k = 0; k < dim; k++ )
			{
			//	cout << v[i][k] << " - " << w[j][k] << endl;
				cost += abs( v[i][k] - w[j][k] ) * abs( v[i][k] - w[j][k] );
			}
			mGamma_cont[i][j] = cost + min( mGamma_cont[i-1][j], mGamma_cont[i][j-1], mGamma_cont[i-1][j-1] );
		}
	}
	return mGamma_cont[vlength - 1][wlength - 1];
}
