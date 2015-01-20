/*
 * CAudioProcess.h
 *
 *  Created on: Jan 5, 2015
 *      Author: vostanin
 */

#ifndef CAUDIOPROCESS_H_
#define CAUDIOPROCESS_H_

#include <RtAudio.h>
#include "common.h"
#include "recordcallback.h"
#include <vector>
#include <iostream>
using namespace std;

class CAudioProcess {
public:
	CAudioProcess();
	virtual ~CAudioProcess();

	bool init( record_process_callback callback );

	void stop();
	void start();
	void close();
	static CAudioProcess & GetInstance();

	vector<float> & getPastBuffer();
	vector<float> & getPresentBuffer();
	vector<float> & getFutureBuffer();

private:


	static int record_callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		            double streamTime, RtAudioStreamStatus status, void *userData );

	record_process_callback m_audio_process_callback;
	RtAudio m_adc;
	const unsigned int m_channels_count;
	const int m_sampleRate;

	vector<float> m_past_signal;
	vector<float> m_present_signal;
	vector<float> m_future_signal;
};

#endif /* CAUDIOPROCESS_H_ */
