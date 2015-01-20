/*
 * record.h
 *
 *  Created on: Dec 1, 2014
 *      Author: vostanin
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <RtAudio.h>
#include "common.h"
#include <vector>
#include <iostream>
using namespace std;

typedef void (*record_process_callback)( vector<float> & past_signal, vector<float> & present_signal, vector<float> & future_signal, float threshold_amplitude );

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData );

bool record_init(RtAudio& adc, record_process_callback callback);

void record_stop(RtAudio& adc);

#endif /* RECORD_H_ */
