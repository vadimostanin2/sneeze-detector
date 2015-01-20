/*
 * record.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: vostanin
 */

#include "record.h"
#include "training_sound_config.h"
#include "MainWindow.h"
#include <stdlib.h>

const unsigned int channels_count = 1;
unsigned int sampleRate = 16000;
const unsigned int desiredBufferFrames = sampleRate; // 8000 sample frames

static RtAudio * global_adc = NULL;

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData )
{
	static vector<float> past_signal;
	static vector<float> present_signal;
	static vector<float> future_signal;



    if ( status )
    {
        cout << "Stream overflow detected!" << endl << flush ;

        return 0;
    }

    static unsigned int time_signal_init = 1;
    float *buffer = (float *) inputBuffer;
    record_process_callback callback = (record_process_callback )userData;

    if( time_signal_init == 1 )
    {
    	present_signal.assign( buffer, buffer + nBufferFrames );
    	time_signal_init++;
    	return 0;
    }
    else if( time_signal_init == 2 )
    {
    	future_signal.assign( buffer, buffer + nBufferFrames );
    	time_signal_init++;
    	return 0;
    }

    past_signal = present_signal;
    present_signal = future_signal;
    future_signal.clear();
    future_signal.assign( buffer, buffer + nBufferFrames );

    vector<float> signal( present_signal.begin(), present_signal.end() );

    const float threshold_amplitude = 0.1;
    unsigned int max_index = 0;
    float max_value = 0.0;
    if( false == condition_check_signal_threshold( signal, threshold_amplitude, max_value, max_index ) )
    {
    	cout << "max_amplitude=" << max_value << endl;
    	sneeze_status_set( SOUND_STATUS_PREPARED_E );
    	return 0;
    }

    cout << "max_amplitude=" << max_value << endl;

    global_adc->stopStream();

    callback( past_signal, present_signal, future_signal, threshold_amplitude );

    global_adc->startStream();

    return 0;
}

bool record_init( RtAudio& adc, record_process_callback callback )
{
    int device_count = adc.getDeviceCount();
    if ( device_count < 1 )
    {
        std::cout << "\nNo audio devices found!\n";
        return false;
    }
    bool stream_started = false;
    for( int devices_i = 0 ; devices_i < device_count ; devices_i++ )
    {
        RtAudio::StreamParameters parameters;
        parameters.deviceId = devices_i;//adc.getDefaultInputDevice();
        parameters.nChannels = channels_count;
        parameters.firstChannel = 0;

        unsigned int bufferFrames = desiredBufferFrames;
        try
        {
            adc.openStream( NULL, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, record, (void*)callback );
            if( bufferFrames != desiredBufferFrames )
            {
            	adc.closeStream();
            	continue;
            }
            stream_started = true;
            break;
        }
        catch ( RtAudioError& e )
        {
            e.printMessage();
            adc.closeStream();
            continue;
        }
    }
    if( stream_started == true )
    {
        std::cout << "\nRecording ...";
        adc.startStream();
        global_adc = & adc;
    }

    return stream_started;
}

void record_stop(RtAudio& adc)
{
    try
    {
        // Stop the stream
        adc.stopStream();
    }
    catch (RtAudioError& e)
    {
        e.printMessage();
    }
    if ( adc.isStreamOpen() )
    {
        adc.closeStream();
    }
}
