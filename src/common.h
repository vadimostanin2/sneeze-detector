/*
* common.h
 *
 *  Created on: Nov 12, 2014
 *      Author: vostanin
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <sstream>
#include <cstring>
#include <cmath>
#include <sndfile.h>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <fftw3.h>

using namespace std;

typedef struct
{
	float real;
	float im;
}complex_type_my;

typedef struct
{
	float freq;
	float magnitude;
}freq_magn_type;

#define FREQ_ANALIZE_MIN 100
#define FREQ_ANALIZE_MAX 2000

typedef float signal_type;
typedef fftwf_complex complex_type;

#define OUTPUTS_COUNT 1

typedef vector<signal_type> map_freqs_type;

typedef enum
{
	RANGE_FIRST_E = 1,
	RANGE_1_E = RANGE_FIRST_E,
	RANGE_2_E,
	RANGE_4_E,
	RANGE_8_E,
	RANGE_16_E,
	RANGE_32_E,
	RANGE_64_E,
	RANGE_128_E,
	RANGE_256_E,
	RANGE_512_E,
	RANGE_1024_E,
	RANGE_2048_E,
	RANGE_4096_E,
	RANGE_8192_E,
	RANGE_16384_E,
	RANGE_32768_E,
	RANGE_65536_E,
	RANGE_LAST_E = RANGE_65536_E
}RANGE_TYPE;

typedef vector<vector<vector<float> > > vector_mfcc_level_3;
typedef vector<vector<float> > vector_mfcc_level_2;
typedef vector<float> vector_mfcc_level_1;

struct RangeDistance
{
	RANGE_TYPE range_type;
	unsigned int distance;
};

#define FREQUENCY_PROCESSED_COUNT 15

typedef enum
{
	COPY_SIGNAL_ERROR_SUCCESS = 0,
	COPY_SIGNAL_ERROR_NEED_PREV_SIGNAL,
	COPY_SIGNAL_ERROR_NEED_NEXT_AND_PREV_SIGNAL
}COPY_SIGNAL_ERROR_E;

void dump( vector<signal_type> & input );
void dump( vector_mfcc_level_1 & input );

double complex_magnitude( complex_type cpx );

void freq_smothing(complex_type* complexes, unsigned int count, double samplerate);

void print_freq_to_out_file( const char * file );

void print_time_to_out_file( const char * file );

void add_to_frequency_out_vector( complex_type * data, unsigned int count );

void add_to_time_out_vector( float * data, unsigned int count );

bool filename_pattern_match( string & pattern, string & filename );

void read_wav_data( string & wav_file, vector<signal_type> & wav_raw, int & channels_count );
void evarage_channels( vector<signal_type> & wav_raw, unsigned int channels_count, vector<signal_type> & chunk_merged );
void save_chunk_data(vector<signal_type> & chunk_data, int index );
void normalize( vector<float> & signal, float max_val);
void negative_values_zeroes( vector<float> & signal );
void cut_zeroes( vector<float> & signal );
void movesignal_to_value( vector<signal_type> & signal, float max_val);
void movesignal_to_0_1( vector<signal_type> & signal );
void divide_on_seconds_chunks( vector<signal_type> & signal, vector<vector<signal_type> > & perioded_chunks, size_t samplerate );
void divide_on_perioded_chunks( vector<signal_type> & signal, vector<vector<signal_type> > & perioded_chunks, size_t periods_count, size_t threshold_signals_per_chunk );
void template_freq_range_fill_distance_from_each_to_next( vector<signal_type> & signal, vector_mfcc_level_1 & freq_range_template );
void template_freq_range_fill_distance_from_each_to_max( vector<signal_type> & signal, vector_mfcc_level_1 & freq_range_template );
bool freq_range_template_equal( vector_mfcc_level_1 & freq_range_template_1, vector_mfcc_level_1 & freq_range_template_2, unsigned int threshold_failed );
bool make_train_file_with_vector( vector<signal_type> & magnitudes, unsigned int num_input_neurons, unsigned int num_output_neurons, vector<int> & outputs );
float get_maximum_amplitude(vector<float> & signal, float & peak_value, unsigned int & peak_index);
bool copy_periods_before_after_signal_index( vector<signal_type> & signal, float threshold_amplitude, unsigned int signal_index, unsigned int periods_count,
											 unsigned int max_perioded_chunks_count, vector<signal_type> & out_chunk, unsigned int & new_max_index, COPY_SIGNAL_ERROR_E & error );
void trim_sides_periods_bellow_threshold( vector<signal_type> & perioded_signal, float threshold_amplitude, vector<signal_type> & perioded_signal_result );
void prepare_signal_near_peak( vector<signal_type> & present_signal, vector<signal_type> & past_signal, vector<signal_type> & future_signal, float threshold,
		unsigned int max_index, unsigned int periods_count_per_chunk, unsigned int max_chunks_count, vector<signal_type> & prepared_signal, unsigned int & new_max_index );
void merge_and_split_with_cross( vector<vector<signal_type> > & perioded_chunk, unsigned int periods_count, vector<vector<signal_type> > & perioded_crossed_chunks );
void get_time_chunk_signal_corner( vector<float> & signal, double maximum, int maximum_index, bool prev, int & corner);
bool condition_check_signal_corner_shoulder_at_max( vector<signal_type> & signal, float maximum, unsigned int maximum_index );
bool condition_check_signal_threshold( vector<signal_type> & signal, float threshold_positive, float threshold_negative, float & max_value, unsigned int & max_index );
bool condition_check_signal_threshold( vector<signal_type> & signal, float threshold, float & max_value, unsigned int & max_index );
void get_current_folder( string & folder );

void make_fft( vector<signal_type> & data, complex_type ** fft_result, fftwf_plan * plan_forward );

void destroy_fft( complex_type ** fft_result, fftwf_plan * plan_forward );

void getFrequencesWithHigherMagnitudes( size_t frequencyCount, vector<complex_type> & linear_magnitudes_input, vector<freq_magn_type> & log_magnitudes, size_t samplerate );

void zero( complex_type* cpx );
void freqfilter(vector<complex_type> & complexes, double samplerate, unsigned int left_freq, unsigned int right_freq);
void wHanning( int fftFrameSize, vector<float> & hanning );
void apply_hanning_window( vector<signal_type> & magnitudes );

void get_spectrum_magnitudes( vector<signal_type> & signal_chunk, vector<signal_type> & magnitudes);
void get_mfcc_coefficients( vector<signal_type> & signal_chunk, vector<float> & mfcc_coefficients);
void mfcc_meaning( vector_mfcc_level_2 & mfcc_v, unsigned int merging_count, float threshold, vector_mfcc_level_2 & mfcc_v_result );
void mfcc_meaning( vector_mfcc_level_3 & sounds, unsigned int merging_count, float threshold, vector_mfcc_level_3 & mfcc_v_result );

void save_mfcc_coefficients( vector<vector<vector<float> > > & all_mfcc_v );
void load_mfcc_coefficients( vector<vector<vector<float> > > & mfcc_v );
void mfcc_get_distance( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance );
void dtw_get_distance( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance );
void dtw_get_distance( float * mfcc_1, float * mfcc_2, unsigned int mfcc_1_size, unsigned int mfcc_2_size, double & distance );
void hmm_get_propability( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance );

void get_most_meet_amplitude_range( vector<signal_type> & signal, float & low_range, float & high_range );
void get_atleast_meet_amplitude_range( vector<signal_type> & signal, unsigned int minimum_peaks_count, float & low_range, float & high_range );
float get_mean_peak_amplitude( vector<signal_type> & signal );

void training_sound_template_remove_same( vector_mfcc_level_3 & all_templates_ranges, size_t & removed_number, size_t & remain_number );
void training_sound_template_remove_nearest( vector_mfcc_level_2 & templates_ranges_1, vector_mfcc_level_2 & templates_ranges_2, float min_distance, size_t & removed_number, size_t & remain_number );

#endif /* COMMON_H_ */
