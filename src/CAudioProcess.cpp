/*
 * CAudioProcess.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: vostanin
 */

#include "CAudioProcess.h"
#include "MainWindow.h"

CAudioProcess::CAudioProcess(): m_audio_process_callback( NULL ), m_channels_count( 1 ), m_sampleRate( 16000 )
{
}

CAudioProcess::~CAudioProcess()
{
}

CAudioProcess & CAudioProcess::GetInstance()
{
	static CAudioProcess instance;
	return instance;
}

int CAudioProcess::record_callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData )
{
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
    	CAudioProcess::GetInstance().getPresentBuffer().assign( buffer, buffer + nBufferFrames );
    	time_signal_init++;
    	return 0;
    }
    else if( time_signal_init == 2 )
    {
    	CAudioProcess::GetInstance().getFutureBuffer().assign( buffer, buffer + nBufferFrames );
    	time_signal_init++;
    	return 0;
    }

    CAudioProcess::GetInstance().getPastBuffer() = CAudioProcess::GetInstance().getPresentBuffer();
    CAudioProcess::GetInstance().getPresentBuffer() = CAudioProcess::GetInstance().getFutureBuffer();
    CAudioProcess::GetInstance().getFutureBuffer().clear();
    CAudioProcess::GetInstance().getFutureBuffer().assign( buffer, buffer + nBufferFrames );

    vector<float> signal( CAudioProcess::GetInstance().getPresentBuffer().begin(), CAudioProcess::GetInstance().getPresentBuffer().end() );

    float low, high;
    unsigned int minimum_peaks_count = 50;
    get_atleast_meet_amplitude_range( signal, minimum_peaks_count, low, high );

    const float threshold_amplitude = high;
    unsigned int max_index = 0;
    float max_value = 0.0;
    bool check_peak = condition_check_signal_threshold( signal, threshold_amplitude, max_value, max_index );
    if( false == check_peak )
    {
    	cout << "max_amplitude=" << max_value << endl << flush;

    	sneeze_status_set( SOUND_STATUS_PREPARED_E );
    	return 0;
    }

    cout << "max_amplitude=" << max_value << endl << flush;

    CAudioProcess::GetInstance().stop();

    callback( CAudioProcess::GetInstance().getPastBuffer(), CAudioProcess::GetInstance().getPresentBuffer(), CAudioProcess::GetInstance().getFutureBuffer(), threshold_amplitude );

    CAudioProcess::GetInstance().start();

    return 0;
}

bool CAudioProcess::init( record_process_callback audio_process_callback )
{
    int device_count = m_adc.getDeviceCount();
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
        parameters.nChannels = m_channels_count;
        parameters.firstChannel = 0;

        const unsigned int desiredBufferFrames = m_sampleRate; // 8000 sample frames

        unsigned int bufferFrames = desiredBufferFrames;
        try
        {
        	RtAudioCallback lpCb = (RtAudioCallback)&CAudioProcess::record_callback;

            m_adc.openStream( NULL, &parameters, RTAUDIO_FLOAT32, m_sampleRate, &bufferFrames, (RtAudioCallback)lpCb, (void*)audio_process_callback );
            if( bufferFrames != desiredBufferFrames )
            {
            	m_adc.closeStream();
            	continue;
            }
            stream_started = true;
            break;
        }
        catch ( RtAudioError& e )
        {
            e.printMessage();
            m_adc.closeStream();
            continue;
        }
    }
    if( stream_started == true )
    {
        std::cout << "\nRecording ...";
        m_adc.startStream();

        m_audio_process_callback = audio_process_callback;
    }

    return stream_started;
}

void CAudioProcess::stop()
{
    try
    {
        // Stop the stream
        m_adc.stopStream();
    }
    catch (RtAudioError& e)
    {
        e.printMessage();
    }
}

void CAudioProcess::close()
{
	try
	{
		if ( m_adc.isStreamOpen() )
		{
			m_adc.closeStream();
		}
	}
	catch( RtAudioError & e )
	{
		;
	}
}

void CAudioProcess::start()
{
	try
	{
		if ( m_adc.isStreamOpen() )
		{
			m_adc.startStream();
		}
	}
	catch( RtAudioError & e )
	{
		;
	}
}

vector<float> & CAudioProcess::getPastBuffer()
{
	return m_past_signal;
}

vector<float> & CAudioProcess::getPresentBuffer()
{
	return m_present_signal;
}

vector<float> & CAudioProcess::getFutureBuffer()
{
	return m_future_signal;
}
