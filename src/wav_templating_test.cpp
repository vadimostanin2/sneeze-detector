//============================================================================
// Name        : wav_templating_test.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fftw3.h>
#include <sndfile.h>
#include <algorithm>
#include <dirent.h>
#include <string.h>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <malloc.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "common.h"
#include "training_sound_config.h"
#include "record.h"
#include "sound_types.h"
#include "MainWindow.h"
//#include "Window.h"
#include "KMeans.h"
//#include "network.h"
#include "CAudioProcess.h"
using namespace std;

trainig_sounds_config_t global_sound_templates;

void sound_templates_fill( string & sounds_folder, unsigned int periods_count, vector_mfcc_level_3 & templates);
void sound_template_fill( vector<signal_type> & wav_raw, size_t channels, unsigned int periods_count, vector_mfcc_level_2 & templates, bool & init_three_buffer_counter );
void sound_template_fill( string & sounds_folder, sound_template_t & fileTemplate, unsigned int periods_count, vector_mfcc_level_2 & templates, bool & init_three_buffer_counter );
void record_template_fill( vector<signal_type> & past_signal, vector<signal_type> & present_signal, vector<signal_type> & future_signal, size_t channels, unsigned int periods_count, float threshold_amplitude, vector_mfcc_level_2 & templates );

void record_process( vector<float> & past_signal, vector<float> & present_signal, vector<float> & future_signal, float threshold_amplitude );

vector_mfcc_level_3 global_all_templates_ranges;

float threshold_distance = 0.01;

int main( int argc, char ** argv )
{
	srand( time ( 0 ) );
	training_sounds_config_get( global_sound_templates );

	string current_folder;
	get_current_folder( current_folder );
	string sounds_folder = current_folder + "/" + "train_sounds";
//FOR SOUND DATABASE LEARNING
	sound_templates_fill( sounds_folder, global_sound_templates.processing_periods_count, global_all_templates_ranges );
	load_mfcc_coefficients( global_all_templates_ranges );
	size_t removed_number = 0, remain_number = 0;
	training_sound_template_remove_nearest( global_all_templates_ranges[0],global_all_templates_ranges[1], threshold_distance * 2, removed_number, remain_number );
	save_mfcc_coefficients( global_all_templates_ranges );
	vector_mfcc_level_3 global_all_templates_ranges_result;

//	mfcc_meaning( global_all_templates_ranges, 2, 0.01, global_all_templates_ranges_result );

//	save_mfcc_coefficients( global_all_templates_ranges_result );
/////////////////////////////
	load_mfcc_coefficients( global_all_templates_ranges );

	CAudioProcess::GetInstance().init( record_process );

	mainwindow_show();

	return 0;
}

void record_process( vector<float> & past_signal, vector<float> & present_signal, vector<float> & future_signal, float threshold_amplitude )
{
	vector_mfcc_level_2 recorded_templates;
	record_template_fill( past_signal, present_signal, future_signal, 1, global_sound_templates.processing_periods_count, threshold_amplitude, recorded_templates );

	size_t recorded_templates_count = recorded_templates.size();

	if( recorded_templates_count == 0 )
	{
		return;
	}

	size_t sound_count = global_all_templates_ranges.size();

	double min_sound_distance = 100000;
	int min_sound_distance_index = -1;

	sneeze_status_set( SOUND_STATUS_PROCESSING_E );

	size_t sound_i = 0;
	vector<float> min_distances( sound_count );
	for(  ; sound_i < sound_count ; sound_i++ )
	{
		vector_mfcc_level_2 & prepared_templates = global_all_templates_ranges[sound_i];
		size_t prepared_templates_count = prepared_templates.size();
		float min_distance_per_sound = 10000.0;
		for( size_t prepared_templates_i = 0 ; prepared_templates_i < prepared_templates_count ; prepared_templates_i++ )
		{
			vector_mfcc_level_1 & prepared_v = prepared_templates[prepared_templates_i];

			for( size_t recorded_templates_i = 0 ; recorded_templates_i < recorded_templates_count ; recorded_templates_i++ )
			{
				vector_mfcc_level_1 & recorded_v = recorded_templates[recorded_templates_i];
				double distance = 0.0f;
//				mfcc_get_distance( recorded_v, prepared_v, distance );
				dtw_get_distance( recorded_v, prepared_v, distance );

				if( distance < min_sound_distance )
				{
					min_sound_distance = distance;
					min_sound_distance_index = sound_i;
				}
				if( distance < min_distance_per_sound )
				{
					min_distance_per_sound = distance;
				}
			}
		}
		min_distances[sound_i] = min_distance_per_sound;
	}

	double min_dist_diff = std::abs( min_distances[1] - min_distances[0] );

	if( min_dist_diff < 0.0005 )
	{
		min_sound_distance_index = (int)SOUND_SNEEZING;
	}

	if( min_sound_distance_index != -1 && min_sound_distance <= threshold_distance )
	{
		string sound_name;
		sound_recognition_type_e sound_type = (sound_recognition_type_e)min_sound_distance_index;
		sound_type_to_string( sound_type, sound_name );


		switch( sound_type )
		{
			case SOUND_OTHER:
				cout<< "Matched " << sound_name << "=" << min_sound_distance_index << "=" << min_sound_distance << "; min dist to Sneeze = " << min_distances[SOUND_SNEEZING] << endl;
				sneeze_status_set( SOUND_STATUS_RECOGNIZED_NOISE_E );
				break;
			case SOUND_SNEEZING:
			{
				static int sneeze_index = 0;
				sneeze_index++;
				save_chunk_data( present_signal, sneeze_index );
				cout<< "Matched " << sound_name << "=" << min_sound_distance_index << "=" << min_sound_distance << "; min dist to Noise = " << min_distances[SOUND_OTHER] << endl;
				sneeze_status_set( SOUND_STATUS_RECOGNIZED_SNEEZE_E );
//				popen( "paplay bless_you.wav", "r" );
			}
				break;
		}
	}
	else
	{
		sneeze_status_set( SOUND_STATUS_RECOGNIZED_NOISE_E );
		cout << "; min_distance=" << min_sound_distance << endl;
	}
}

const int desired_sample_rate = 16000;

void sound_template_fill( vector<signal_type> & wav_raw, size_t channels, unsigned int periods_count, vector_mfcc_level_2 & templates, bool & init_three_buffer_counter )
{
	vector<signal_type> merged_wav_raw;
	evarage_channels( wav_raw, channels, merged_wav_raw );

	vector<vector<signal_type> > seconded_chunks;
	const size_t threshold_signals_per_period = 1;
	divide_on_seconds_chunks( merged_wav_raw, seconded_chunks, desired_sample_rate );

	size_t seconded_count = seconded_chunks.size();

	static int fill_three_buffer_counter = 0;
	if( init_three_buffer_counter )
	{
		fill_three_buffer_counter = 0;
		init_three_buffer_counter = false;
	}

	static vector<float> past_signal( desired_sample_rate, 0.0 );
	static vector<float> present_signal( desired_sample_rate, 0.0 );
	static vector<float> future_signal( desired_sample_rate, 0.0 );

	for( size_t seconded_i = 0 ; seconded_i < seconded_count ; seconded_i++ )
	{
		vector<signal_type> & seconded_chunk = seconded_chunks[seconded_i];

		if( fill_three_buffer_counter == 0 )
		{
			present_signal = seconded_chunk;
			fill_three_buffer_counter++;
			continue;
		}
		else if( fill_three_buffer_counter == 1 )
		{
			future_signal = seconded_chunk;
			fill_three_buffer_counter++;
			continue;
		}

		past_signal = present_signal;
		present_signal = future_signal;
		future_signal = seconded_chunk;

//		save_chunk_data( perioded_chunk, 0, 16000 );



		const float threshold = 0.1;
		unsigned int max_index = 0;
		float max_value = 0.0;
		if( false == condition_check_signal_threshold( present_signal, threshold, max_value, max_index ) )
		{
			continue;
		}

//		if( false == condition_check_signal_corner_shoulder_at_max( present_signal, max_value, max_index ) )
//		{
//			continue;
//		}

		unsigned int new_max_index = 0;
		vector<signal_type> interested_chunk;
		unsigned int max_perioded_chunks_count = 1000;
		prepare_signal_near_peak( present_signal, past_signal, future_signal, threshold, max_index, global_sound_templates.processing_periods_count, max_perioded_chunks_count, interested_chunk, new_max_index );

		vector<vector<signal_type> > perioded_chunks;
		divide_on_perioded_chunks( interested_chunk, perioded_chunks, global_sound_templates.processing_periods_count, threshold_signals_per_period );

		vector<vector<signal_type> > crossed_perioded_chunks;
		merge_and_split_with_cross( perioded_chunks, global_sound_templates.processing_periods_count, crossed_perioded_chunks );


		size_t perioded_chunks_count = crossed_perioded_chunks.size();
		for( size_t perioded_i = 0 ; perioded_i < perioded_chunks_count ; perioded_i++ )
		{
			vector_mfcc_level_1 templat;
			vector<float> mfcc_coefs;
			vector<signal_type> & perioded_chunk = crossed_perioded_chunks[perioded_i];

			normalize( perioded_chunk, 0.5 );

			get_mfcc_coefficients( perioded_chunk, mfcc_coefs );

			templates.push_back( mfcc_coefs );
		}

	}
}

void record_template_fill( vector<signal_type> & past_signal, vector<signal_type> & present_signal, vector<signal_type> & future_signal, size_t channels, unsigned int periods_count, float threshold_amplitude, vector_mfcc_level_2 & templates )
{
	vector<signal_type> merged_wav_raw;
	evarage_channels( present_signal, channels, merged_wav_raw );

	const size_t threshold_signals_per_period = 1;

//		save_chunk_data( perioded_chunk, 0, 44100 );

	unsigned int max_index = 0;
	float max_value = 0.0;
	if( false == condition_check_signal_threshold( present_signal, threshold_amplitude, max_value, max_index ) )
	{
		return;
	}

	unsigned int new_max_index = 0;
	vector<signal_type> interested_chunk;
	unsigned int max_chunks_count = 1000;
	prepare_signal_near_peak( present_signal, past_signal, future_signal, threshold_amplitude, max_index, periods_count, max_chunks_count, interested_chunk, new_max_index );

	vector<vector<signal_type> > perioded_chunks;
	divide_on_perioded_chunks( interested_chunk, perioded_chunks, periods_count, threshold_signals_per_period );

	vector<vector<signal_type> > crossed_perioded_chunks;
	merge_and_split_with_cross( perioded_chunks, periods_count, crossed_perioded_chunks );

	vector_mfcc_level_1 mfcc_coeffs;
//		template_freq_range_fill_distance_from_each_to_next( perioded_chunk, templat );
	size_t perioded_chunks_count = crossed_perioded_chunks.size();
	for( size_t perioded_i = 0 ; perioded_i < perioded_chunks_count ; perioded_i++ )
	{
		vector<signal_type> & perioded_chunk = crossed_perioded_chunks[perioded_i];

		apply_hanning_window( perioded_chunk );

		normalize( perioded_chunk, 0.5 );

//			template_freq_range_fill_distance_from_each_to_max( perioded_chunk, templat );
		get_mfcc_coefficients( perioded_chunk, mfcc_coeffs );

		templates.push_back( mfcc_coeffs );
	}
}

void sound_template_fill( string & sounds_folder, sound_template_t & sound_template, unsigned int periods_count, vector_mfcc_level_2 & templates, bool & init_three_buffer_counter )
{
	vector<string>::iterator template_begin = sound_template.files.begin();
	vector<string>::iterator template_end = sound_template.files.end();
	vector<string>::iterator template_iter = template_begin;

	vector_mfcc_level_2 templates_per_sound_type;

	string filefolder = sound_template.folder;

	for(  ; template_iter != template_end ; template_iter++ )
	{
		string filename = (*template_iter);

		vector<signal_type> wav_raw;
		int channels = 0;

		string wav_name = sounds_folder + "/" + filefolder + "/" + filename;

		read_wav_data( wav_name, wav_raw, channels );

		sound_template_fill( wav_raw, channels, periods_count, templates, init_three_buffer_counter );
	}
}

void sound_templates_fill( string & sounds_folder, unsigned int periods_count, vector_mfcc_level_3 & templates )
{
	bool init_three_buffer_counter = true;
	for( int type_i = SOUND_FIRST ; type_i <= SOUND_LAST ; type_i++ )
	{
		trainig_config_map::iterator sound_template_iter = global_sound_templates.trainig_sounds.find((sound_recognition_type_e)type_i);
		if( sound_template_iter == global_sound_templates.trainig_sounds.end() )
		{
			continue;
		}
		sound_template_t & sound_template = (*sound_template_iter).second;

		vector_mfcc_level_2 templates_per_sound_type;

		sound_template_fill( sounds_folder, sound_template, periods_count, templates_per_sound_type, init_three_buffer_counter );

		templates.push_back( templates_per_sound_type );
	}

	save_mfcc_coefficients( templates );
}
