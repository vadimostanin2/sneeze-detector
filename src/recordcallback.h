/*
 * recordcallback.h
 *
 *  Created on: Jan 6, 2015
 *      Author: vostanin
 */

#ifndef RECORDCALLBACK_H_
#define RECORDCALLBACK_H_

#include <vector>
using namespace std;

typedef void (*record_process_callback)( vector<float> & past_signal, vector<float> & present_signal, vector<float> & future_signal, float threshold_amplitude );


#endif /* RECORDCALLBACK_H_ */
