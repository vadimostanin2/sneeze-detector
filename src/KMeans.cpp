/*
 * KMeans.cpp
 *
 *  Created on: Dec 10, 2014
 *      Author: vostanin
 */

#include "KMeans.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

KMeans::KMeans( vector_mfcc_level_2 & mfcc_coeffs, size_t clusters_num ) : m_clusters_num( clusters_num )
{
	for( size_t mfcc_i = 0 ; mfcc_i < mfcc_coeffs.size() ; mfcc_i ++ )
	{
		mfcc_point_t point;
		point.group = 0;
		point.m_coords = mfcc_coeffs[ mfcc_i ];
		m_mfcc_points.push_back( point );
	}

	mfcc_point_t coords;

	m_centroids_v.resize( m_clusters_num );
}

KMeans::~KMeans()
{
}

void KMeans::calcKMeans()
{
	lloyd( m_mfcc_points, m_clusters_num );
}

void KMeans::getKMeans( vector_mfcc_level_2 & centroids )
{
	centroids.clear();
	size_t centroids_count = m_centroids_v.size();
	for( size_t centroid_i = 0 ; centroid_i < centroids_count ; centroid_i ++ )
	{
		centroids.push_back( m_centroids_v[centroid_i].m_coords );
	}
}

double KMeans::randf(double m)
{
	return m * rand() / (RAND_MAX - 1.);
}

int KMeans::nearest( mfcc_point_t & point, vector<mfcc_point_t> & centroid, int n_cluster, double *d2)
{
	int i, min_i;
	vector<mfcc_point_t>::iterator curr_centroid_iter;
	double distance, min_distance;

	for ( curr_centroid_iter = centroid.begin(), i = 0; i < n_cluster; i++, curr_centroid_iter ++)
	{
		min_distance = HUGE_VAL;
		min_i = point.group;
		for ( curr_centroid_iter = centroid.begin(), i = 0; i < n_cluster; i++, curr_centroid_iter ++)
		{
			mfcc_get_distance( (*curr_centroid_iter).m_coords, point.m_coords, distance );
			if (min_distance > distance )
			{
				min_distance = distance; min_i = i;
			}
		}
	}
	if (d2) *d2 = min_distance;
	return min_i;
}

void KMeans::kpp( vector<mfcc_point_t> & points, vector<mfcc_point_t> & centroids_v, int n_centroids )
{
	int i, j;

	int points_count = points.size();

	int n_cluster;
	double sum, *d = (double*)malloc(sizeof(double) * points_count);

	vector<mfcc_point_t>::iterator curr_point_iter;
	vector<mfcc_point_t>::iterator curr_centroid_iter = centroids_v.begin();
	centroids_v[0] = points[ rand() % points_count ];
	size_t coords_count = centroids_v[0].m_coords.size();
	for (n_cluster = 1; n_cluster < n_centroids; n_cluster++)
	{
		sum = 0;
		curr_point_iter = points.begin();
		for( j = 0 ; j < points_count; j++, curr_point_iter++)
		{
			nearest( (*curr_point_iter), centroids_v, n_cluster, d + j);
			sum += d[j];
		}
		sum = randf(sum);
		curr_point_iter = points.begin();
		for( j = 0 ; j < points_count; j++, curr_point_iter++)
		{
			if ((sum -= d[j]) > 0)
			{
				continue;
			}
			centroids_v[n_cluster] = points[j];
			break;
		}
	}
	curr_point_iter = points.begin();
	for( j = 0 ; j < points_count; j++, curr_point_iter++)
	{
		(*curr_point_iter).group = nearest( (*curr_point_iter), centroids_v, n_cluster, 0);
	}
	free(d);
}

void KMeans::lloyd( vector<mfcc_point_t> & points, int n_cluster )
{
	int i, j, min_i;
	int points_count = points.size();
	int changed;

	/* assign init grouping randomly */
	//for_len p->group = j % n_cluster;

	vector<mfcc_point_t>::iterator curr_centroid_iter;
	vector<mfcc_point_t>::iterator curr_point_iter;

	/* or call k++ init */
	kpp( points, m_centroids_v, n_cluster);

	do {
		/* group element for centroids are used as counters */

		size_t coords_count = 0;
		for ( curr_centroid_iter = m_centroids_v.begin(), i = 0; i < n_cluster; i++, curr_centroid_iter ++)
		{
			coords_count = (*curr_centroid_iter).m_coords.size();
			(*curr_centroid_iter).group = 0;
			coords_count = (*curr_centroid_iter).m_coords.size();
			memset( &(*curr_centroid_iter).m_coords[0], 0, (*curr_centroid_iter).m_coords.size() * sizeof(float));
			coords_count = (*curr_centroid_iter).m_coords.size();
		}
		curr_point_iter = points.begin();
		for( j = 0 ; j < points_count; j++, curr_point_iter++)
		{
			curr_centroid_iter = m_centroids_v.begin() + (*curr_point_iter).group;
			(*curr_centroid_iter).group++;
			size_t coords_count = (*curr_centroid_iter).m_coords.size();
			for( size_t coord_i = 0 ; coord_i < coords_count ; coord_i ++ )
			{
				(*curr_centroid_iter).m_coords[coord_i] += (*curr_point_iter).m_coords[coord_i];
			}
		}
		for ( curr_centroid_iter = m_centroids_v.begin(), i = 0; i < n_cluster; i++, curr_centroid_iter ++)
		{
			size_t coords_count = (*curr_centroid_iter).m_coords.size();
			for( size_t coord_i = 0 ; coord_i < coords_count ; coord_i ++ )
			{
				(*curr_centroid_iter).m_coords[coord_i] /= (*curr_centroid_iter).group;
			}
		}

		changed = 0;
		/* find closest centroid of each point */
		curr_point_iter = points.begin();
		for( j = 0 ; j < points_count ; j++, curr_point_iter++)
		{
			min_i = nearest( (*curr_point_iter), m_centroids_v, n_cluster, 0);
			if (min_i != (*curr_point_iter).group)
			{
				changed++;
				(*curr_point_iter).group = min_i;
			}
		}
	} while (changed > (points_count >> 10)); /* stop when 99.9% of points are good */

	for ( curr_centroid_iter = m_centroids_v.begin(), i = 0; i < n_cluster; i++, curr_centroid_iter ++)
	{
		(*curr_centroid_iter).group = i;
	}
}
