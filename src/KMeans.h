/*
 * KMeans.h
 *
 *  Created on: Dec 10, 2014
 *      Author: vostanin
 */

#ifndef KMEANS_H_
#define KMEANS_H_

#include "common.h"

typedef struct
{
	vector_mfcc_level_1 m_coords;
	int group;
} mfcc_point_t;
//
//typedef struct
//{
//	vector_mfcc_level_2 m_coords;
//	int group;
//} mfcc_group_t;

class KMeans
{
	vector<mfcc_point_t> m_centroids_v;
	size_t m_clusters_num;
	vector<mfcc_point_t> m_mfcc_points;
	int nearest( mfcc_point_t & poins, vector<mfcc_point_t> & m_centroids_v, int n_cluster, double *d2);
	void kpp( vector<mfcc_point_t> & points, vector<mfcc_point_t> & cent, int n_centroids );
	void lloyd( vector<mfcc_point_t> & points, int n_cluster );
	double randf(double m);
public:
	KMeans( vector_mfcc_level_2 & mfcc_coeff, size_t clusters_num );
	virtual ~KMeans();

	void calcKMeans();

	void getKMeans( vector_mfcc_level_2 & centroids );
};

#endif /* KMEANS_H_ */
