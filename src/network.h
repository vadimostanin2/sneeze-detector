/*
 * network.h
 *
 *  Created on: Dec 31, 2014
 *      Author: vostanin
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "common.h"

bool network_teach( vector_mfcc_level_3 & sounds_mfcc_v );

void network_test( vector_mfcc_level_2 & mfcc_v, vector<float> & outputs_result );

void network_load();


#endif /* NETWORK_H_ */
