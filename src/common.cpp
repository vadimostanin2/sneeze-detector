/*
 * common.c
 *
 *  Created on: Nov 12, 2014
 *      Author: vostanin
 */

#include "common.h"
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "Mfcc.h"
#include "dtw.h"


RangeDistance global_ranges[] = {
  {RANGE_1_E, 1},
  {RANGE_2_E, 2},
  {RANGE_4_E, 4},
  {RANGE_8_E, 8},
  {RANGE_16_E, 16},
  {RANGE_32_E, 32},
  {RANGE_64_E, 64},
  {RANGE_128_E, 128},
  {RANGE_256_E, 256},
  {RANGE_512_E, 512},
  {RANGE_1024_E, 1024},
  {RANGE_2048_E, 2048},
  {RANGE_4096_E, 4096},
  {RANGE_8192_E, 8192},
  {RANGE_16384_E, 16384},
  {RANGE_32768_E, 32768},
  {RANGE_65536_E, 65536},
  };
//
//void dump( vector<signal_type> & input )
//{
//	ofstream file( "dump.txt", ios_base::app );
//
//	for( size_t i = 0 ; i < input.size() ; i++ )
//	{
//		file << input[i] << " " << flush;
//	}
//	file << endl;
//	file.close();
//}

void dump( vector_mfcc_level_1 & input )
{
	ofstream file( "dump.txt", ios_base::app );

	for( size_t i = 0 ; i < input.size() ; i++ )
	{
		file << input[i] << " " << std::flush;
	}
	file << endl;
	file.close();
}

void dump( vector<freq_magn_type> & input )
{
	ofstream file( "dump.txt", ios_base::app );

	for( size_t i = 0 ; i < input.size() ; i++ )
	{
		file << input[i].freq << "=" << input[i].magnitude << " " << flush;
	}
	file << endl;
	file.close();
}

void save_mfcc_coefficients( vector<vector<vector<float> > > & all_mfcc_v )
{
	size_t all_mfcc_v_count = all_mfcc_v.size();
	for( int all_mfcc_v_i = 0 ; all_mfcc_v_i < all_mfcc_v_count ; all_mfcc_v_i++ )
	{
		char str_num[] = { (char)(all_mfcc_v_i + 48), '\0' };
		string filename = string( "mfcc_" ) + str_num;
		ofstream file( filename.c_str(), ios_base::trunc );

		file << all_mfcc_v[all_mfcc_v_i].size() << endl;

		for( size_t mfcc_v_i = 0 ; mfcc_v_i < all_mfcc_v[all_mfcc_v_i].size() ; mfcc_v_i++ )
		{
			size_t mfcc_v_count = all_mfcc_v[all_mfcc_v_i][mfcc_v_i].size();
			for( size_t mfcc_i = 0 ; mfcc_i < mfcc_v_count ; mfcc_i++ )
			{
				file << all_mfcc_v[all_mfcc_v_i][mfcc_v_i][mfcc_i] << flush;
				if( ( mfcc_i + 1 ) < mfcc_v_count )
				{
					file << " " << flush;
				}
			}
			file << endl;
		}
		file.close();
	}
}

void load_mfcc_coefficients( vector<vector<vector<float> > > & all_mfcc_v )
{
	all_mfcc_v.clear();

	for( size_t file_i = 0 ;  ; file_i++ )
	{
		char str_num[] = { (char)(file_i + 48), '\0' };
		string filename = string( "mfcc_" ) + str_num;

		ifstream infile( filename.c_str() );

		if( false == infile.is_open() )
		{
			break;
		}

		vector<vector<float> > mfcc_v;

		int mfcc_count = 0;

		infile >> mfcc_count;

		string line;
		while (getline(infile, line, '\n'))
		{
			if( true == line.empty() )
			{
				continue;
			}
			vector<float> mfcc;
			stringstream str_stream( line );

			while(str_stream.eof() != true)
			{
				float cc = 0;
				str_stream >> cc;
				mfcc.push_back( cc );
			}
			mfcc_v.push_back( mfcc );
		}

		all_mfcc_v.push_back( mfcc_v );
	}
}

void get_mean_two_mfcc( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, vector_mfcc_level_1 & mean )
{
	size_t mfcc_1_size = mfcc_1.size();
	size_t mfcc_2_size = mfcc_2.size();
	if( mfcc_1_size == 0 )
	{
		return;
	}
	if( mfcc_1_size != mfcc_2_size )
	{
		return;
	}
	mean.resize( mfcc_1_size, 0.0 );

	for( unsigned int mfcc_i = 0 ; mfcc_i < mfcc_1_size ; mfcc_i++ )
	{
		mean[mfcc_i] = (mfcc_1[mfcc_i] + mfcc_2[mfcc_i]) / 2;
	}
}

void mfcc_meaning( vector_mfcc_level_3 & sounds, unsigned int merging_count, float threshold, vector_mfcc_level_3 & sounds_result )
{
	size_t sound_count_size = sounds.size();

	for( unsigned int sound_i = 0 ; sound_i < sound_count_size ; sound_i++ )
	{
		vector_mfcc_level_2 & mfcc_v = sounds[sound_i];
		vector_mfcc_level_2 mfcc_v_result;
		mfcc_meaning( mfcc_v, merging_count, threshold, mfcc_v_result );
		sounds_result.push_back( mfcc_v_result );
	}
}

void mfcc_meaning( vector_mfcc_level_2 & mfcc_v, unsigned int merging_count, float threshold, vector_mfcc_level_2 & mfcc_v_result )
{
	size_t count = mfcc_v.size();
	size_t mfcc_size = mfcc_v[0].size();
	vector<unsigned int> used_mfcc;
	for( unsigned int mfcc_i = 0 ; mfcc_i < count ; mfcc_i++ )
	{
		vector_mfcc_level_1 last_mean = mfcc_v[mfcc_i];

		if( std::find( used_mfcc.begin(), used_mfcc.end(), mfcc_i) == used_mfcc.end() )
		{
			for( unsigned int merging_i = 0 ; merging_i < merging_count ; merging_i++ )
			{
				double min_value = 1000.0;
				unsigned int min_index = 0;
				double distance = 0.0;
				for( unsigned int mfcc_j = 0 ; mfcc_j < count ; mfcc_j++ )
				{
					if( std::find( used_mfcc.begin(), used_mfcc.end(), mfcc_j) == used_mfcc.end() )
					{
						mfcc_get_distance( mfcc_v[mfcc_i], mfcc_v[mfcc_j], distance );
						if( distance < min_value )
						{
							min_value = distance;
							min_index = mfcc_j;
						}
					}
				}

				used_mfcc.push_back( min_index );

				vector_mfcc_level_1 mean;

				get_mean_two_mfcc( last_mean, mfcc_v[min_index], mean );

				last_mean = mean;
			}

			mfcc_v_result.push_back( last_mean );
		}
	}
}

double complex_magnitude( complex_type cpx )
{
    double real = cpx[0];
    double im   = cpx[1];
    return sqrt( ( real * real ) + ( im * im ) );
}

void zero( complex_type & cpx )
{
    cpx[0] = 0.0;
    cpx[1] = 0.0;
}

void freqfilter(vector<complex_type> & complexes, double samplerate, unsigned int left_freq, unsigned int right_freq)
{
    sf_count_t i;
    sf_count_t samples2;
    size_t count = complexes.size();

    samples2 = count / 2;

    for( i=0; i < count ; i++ )
    {
        double freq = ((double)i * samplerate / (double)count);
        if( freq < left_freq || freq > right_freq )
        {
            zero( complexes[i] );
        }
    }
}

bool filename_pattern_match( string & filename, string & pattern )
{
	bool result = true;

	const char * str_filename = filename.c_str();
	const char * str_pattern = pattern.c_str();
	const char delim[2] = { '*', '\0' };
	char * pch = NULL;
	pch = strtok( (char*)str_pattern, delim );
	while (pch != NULL)
	{
		if( strstr( str_filename, pch ) == NULL )
		{
			result = false;
			break;
		}

		pch = strtok( NULL, "*" );
	}

	return result;
}


void read_wav_data( string & wav_file, vector<signal_type> & wav_raw, int & channels_count )
{
	SF_INFO info_in;
	SNDFILE * fIn = sf_open( wav_file.c_str(), SFM_READ, &info_in );

	if( fIn == NULL )
	{
		return;
	}

	int nTotalRead = 0;
	int nRead = 0;

	channels_count = info_in.channels;

	do
	{
		vector<signal_type> temp;
		temp.resize(info_in.frames * info_in.channels);
		nRead = sf_readf_float( fIn, &temp[0], info_in.frames - nTotalRead );


		wav_raw.insert( wav_raw.end(), temp.begin(), temp.begin() + nRead * info_in.channels );

		nTotalRead += nRead;
	}
	while( nTotalRead < info_in.frames );

	sf_close(fIn);
}

void evarage_channels( vector<signal_type> & wav_raw, unsigned int channels_count, vector<signal_type> & chunk_merged )
{
	if( channels_count == 1 )
	{
		chunk_merged.clear();
		chunk_merged.assign( wav_raw.begin(), wav_raw.end() );
	}
	else
	{
		unsigned int wav_raw_size = wav_raw.size();

		chunk_merged.resize( wav_raw_size / channels_count );

		for( unsigned int wav_raw_i = 0 ; wav_raw_i < wav_raw_size ; wav_raw_i += channels_count )
		{
			float sum_amplitude = 0.0;
			for( unsigned int channel_i = 0 ; channel_i < channels_count ; channel_i ++ )
			{
				double val = wav_raw[wav_raw_i + channel_i];
				sum_amplitude += val;
			}
			chunk_merged[ wav_raw_i / channels_count ] = sum_amplitude / channels_count;
		}
	}
}

void save_chunk_data(vector<signal_type> & chunk_data, int index )
{
	SF_INFO info_out = { 0 };
	info_out.channels = 1;
	info_out.samplerate = 16000;
	unsigned int minutes = 60*24;
	int seconds = minutes * 60;
	info_out.frames = info_out.samplerate * info_out.channels * seconds;
	info_out.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;

	char outfilepath[BUFSIZ] = {'\0'};
	sprintf(outfilepath, "%d.wav", index);

	SNDFILE * fOut = NULL;
	fOut = sf_open( outfilepath, SFM_WRITE, &info_out );
	sf_writef_float( fOut, &chunk_data[0], chunk_data.size() );
	sf_close(fOut);
}


void normalize( vector<signal_type> & signal, float max_val)
{
    // recherche de la valeur max du signal
    float max=0.f;
    size_t size = signal.size();
    for(size_t i=0 ; i<size ; i++)
    {
        if (abs(signal[i])>max) max=abs(signal[i]);
    }
    // ajustage du signal
    float ratio = max_val/max;
    for(size_t i=0 ; i<size ; i++)
    {
        signal[i] = signal[i]*ratio;
    }
}

void movesignal_to_value( vector<signal_type> & signal, float val)
{
    // recherche de la valeur max du signal
    float max=0.f;
    size_t size = signal.size();
    for(size_t i=0 ; i<size ; i++)
    {
        signal[i] += val;
    }
}

void negative_values_zeroes( vector<float> & signal )
{
    size_t size = signal.size();
    for(size_t i=0 ; i<size ; i++)
    {
        if( signal[i] < 0 )
		{
			signal[i] = 0;
		}
    }
}

void cut_zeroes( vector<float> & signal )
{
    size_t size = signal.size();
    unsigned int insert_i = 0;
    size_t search_i = 0;
    for( search_i = 0 ; search_i < size ; search_i++)
    {
        if( signal[search_i] != 0 )
		{
        	signal[insert_i] = signal[search_i];
        	insert_i++;
		}
    }
}

void get_current_folder( string & folder )
{
	char cwd[BUFSIZ] = { '\0' };
	getcwd( cwd, BUFSIZ );
	folder = cwd;
}

void movesignal_to_0_1( vector<signal_type> & signal )
{
	normalize( signal, 0.5f );
	movesignal_to_value( signal, 0.5f );
}

void period_start_get( vector<signal_type> & signal, size_t & signal_index )
{
	size_t signal_length = signal.size();
	float last_value = 0.0;
	for( size_t signal_i = 0 ; signal_i < signal_length ; signal_i++ )
	{
		signal_type value = signal[signal_i];
		if( value >= 0 && last_value < 0 )
		{
			signal_index = signal_i;
			break;
		}
		last_value = value;
	}
}

bool period_next_get( vector<signal_type> & signal, unsigned int index, unsigned int & estimate_signals, vector<signal_type> & perioded_chunk )
{
	size_t signal_length = signal.size();
	float last_value = 0.0;
	estimate_signals = 0;
	bool found = false;
	size_t zero_counter = 0;
	for( size_t signal_i = index ; signal_i < signal_length ; signal_i++ )
	{
		signal_type value = signal[signal_i];

		if( zero_counter >= 2 )
		{
			found = true;
			perioded_chunk.clear();
			break;
		}

		if( ( value >= 0 && last_value < 0 ) )
		{
			size_t chunk_size = perioded_chunk.size();
			if( chunk_size >= 1 )
			{
				found = true;
			}
			else
			{
				perioded_chunk.clear();
			}

			break;
		}
		else if ( value != 0 )
		{
			perioded_chunk.push_back( value );
			estimate_signals++;
			zero_counter = 0;
		}
		else if ( value == 0 )
		{
			zero_counter++;
		}

		last_value = value;
	}

	return found;
}

bool period_prev_get( vector<signal_type> & signal, unsigned int index, unsigned int & estimate_signals, vector<signal_type> & perioded_chunk )
{
	float last_value = 0.0;
	estimate_signals = 0;
	bool found = false;
	for( int signal_i = index ; signal_i >= 0 ; signal_i-- )
	{
		signal_type value = signal[signal_i];

		if( ( value >= 0 && last_value < 0 ) || ( last_value == 0 && value == 0 ) )
		{
			size_t chunk_size = perioded_chunk.size();
			if( chunk_size >= 1 )
			{
				found = true;
			}
			else
			{
				perioded_chunk.clear();
			}
			break;
		}
		else
		{
			perioded_chunk.push_back( value );
			estimate_signals++;
		}

		last_value = value;
	}

	return found;
}

void divide_on_perioded_chunks( vector<signal_type> & signal, vector<vector<signal_type> > & perioded_chunks, size_t periods_count, size_t threshold_signals_per_chunk )
{
	size_t signal_length = signal.size();

	size_t signal_start = 0;
	period_start_get( signal, signal_start );

	for( size_t signal_i = signal_start ; signal_i < signal_length ;  )
	{
		vector<signal_type> chunk;
		for( size_t period_i = 0 ; period_i < periods_count ; period_i++ )
		{
			vector<signal_type> period;
			unsigned int estimate_signals = 0;
			period_next_get( signal, signal_i, estimate_signals, period );

			if( false == period.empty() )
			{
				chunk.insert( chunk.end(), period.begin(), period.end() );
			}

			signal_i += estimate_signals;
		}
		size_t chunk_size = chunk.size();
		size_t perioded_chunks_count = perioded_chunks.size();
		if( chunk_size >= threshold_signals_per_chunk )
		{
			perioded_chunks.push_back( chunk );
		}
		else
		{
			break;
		}
	}
}

void divide_on_seconds_chunks( vector<signal_type> & signal, vector<vector<signal_type> > & perioded_chunks, size_t samplerate )
{
	size_t signal_length = signal.size();

	unsigned int seconds_count = signal_length/samplerate;

	for( size_t signal_i = 0 ; signal_i < seconds_count ; signal_i ++ )
	{
		vector<signal_type> chunk( samplerate );
		memcpy( &chunk[0], &signal[ signal_i * samplerate ], samplerate * sizeof( float ) );

		perioded_chunks.push_back( chunk );
	}
}

bool make_train_file_with_vector( vector<signal_type> & magnitudes, unsigned int num_input_neurons, unsigned int num_output_neurons, vector<int> & outputs )
{
	string header_out_file_name = "freqs_train_header.txt";
	string data_out_file_name = "freqs_train_data.txt";

	int num_data = 0;
	static bool global_first_open = true;
	if( true == global_first_open )
	{
		unlink( header_out_file_name.c_str() );
		unlink( data_out_file_name.c_str() );
		unlink( "../freqs_train.txt" );
	}
	else
	{
		ifstream header_file( header_out_file_name.c_str(), ios_base::in );
		if( true == header_file.is_open() )
		{
			header_file >> num_data;
			header_file.close();
		}
		else
		{
			cout << strerror(errno) << endl;
		}
	}

	ofstream header_out_file( header_out_file_name.c_str(), ios_base::out );
	ofstream data_out_file( data_out_file_name.c_str(), ios_base::app );
	ofstream train_file( "../freqs_train.txt", ios_base::trunc );

	if( false == header_out_file.is_open() )
	{
		return false;
	}

	if( false == data_out_file.is_open() )
	{
		return false;
	}

	if( false == train_file.is_open() )
	{
		return false;
	}

////////////PREPARE HEADER
	if( true == global_first_open )
	{
		num_data = 1;
		header_out_file<<num_data << " " << num_input_neurons << " " << num_output_neurons << endl << flush;
	}
	else
	{
		string line;
		stringstream str_stream;
		num_data = num_data + 1;
		str_stream << num_data << " " << num_input_neurons << " " << num_output_neurons;
		line = str_stream.str();

//		cout<< line << endl;

		header_out_file << line << endl << flush;
	}
////////////PREPARE DATA
	data_out_file << flush;

	unsigned int directions_count = magnitudes.size();

	for( unsigned int direction_i = 0 ; direction_i < directions_count ; direction_i++ )
	{
		vector<signal_type>::value_type val = magnitudes[direction_i];
		data_out_file << val << flush;

		if( ( direction_i + 1) < directions_count )
		{
			data_out_file << " " << flush;
		}
	}
	data_out_file << endl;
	const size_t outputs_count = outputs.size();
	for( size_t output_i = 0 ; output_i < outputs_count ; output_i++ )
	{
		if( true == outputs[output_i] )
		{
			data_out_file << 1 << flush;
		}
		else if( false == outputs[output_i] )
		{
			data_out_file << 0 << flush;
		}
		if( ( output_i + 1) < outputs_count )
		{
			data_out_file << " " << flush;
		}
	}

	data_out_file << endl;
	data_out_file << flush;
	data_out_file.close();

	std::string line;

	{
		ifstream header_in_file( header_out_file_name.c_str(), ios_base::in );
		while (std::getline(header_in_file, line))
		{
			train_file << line << endl << flush;
		}
	}

	{
		ifstream data_in_file( data_out_file_name.c_str(), ios_base::in );
		while (std::getline(data_in_file, line))
		{
			train_file << line << endl << flush;
		}
	}

	global_first_open = false;

	return true;
}

bool freq_range_template_equal( vector_mfcc_level_1 & freq_range_template_1, vector_mfcc_level_1 & freq_range_template_2, unsigned int threshold_failed )
{
	size_t freq_count_1 = freq_range_template_1.size();
	size_t freq_count_2 = freq_range_template_2.size();
	if( freq_count_1 != freq_count_2 )
	{
		return false;
	}

	if( freq_count_1 == 0 )
	{
		return false;
	}

	int res = memcmp( &freq_range_template_1[0], &freq_range_template_2[0], freq_count_1 * sizeof( freq_range_template_1[0] ) );

	if( res != 0 )
	{
		return false;
	}
/*

	int sum_diff = 0;

	for( size_t range_i = 0 ; range_i < freq_count_1 ; range_i++ )
	{
		vector_mfcc_level_1::value_type * freq_range_template_2_ptr = &freq_range_template_2[0];
		vector_mfcc_level_1::value_type type_1 = freq_range_template_1[range_i];
		vector_mfcc_level_1::value_type type_2 = freq_range_template_2[range_i];

		double pow_1 = log2( type_1 );
		double pow_2 = log2( type_2 );

		if( type_1 != type_2 )
		{
			int diff_pow = (int)pow_1 - pow_2;
			sum_diff += diff_pow;
			if( sum_diff > threshold_failed )
			{
				return false;
			}
		}
	}
*/
	return true;
}

bool compare( const freq_magn_type & _1, const freq_magn_type & _2 )
{
	return _1.freq < _2.freq;
}

void template_freq_range_fill_distance_from_each_to_next( vector<signal_type> & signal, vector_mfcc_level_1 & freq_range_template )
{
	freq_range_template.clear();
	complex_type * complexes = NULL;
	fftwf_plan plan;
	make_fft( signal, &complexes, &plan );

	const int freqs_range_count = FREQUENCY_PROCESSED_COUNT;

	size_t complex_count = signal.size();

	vector<complex_type> complexes_v( complexes, complexes + complex_count );

	freqfilter( complexes_v, 16000, 50, 8000 );
	vector<freq_magn_type> max_freqs_v;
	getFrequencesWithHigherMagnitudes( freqs_range_count, complexes_v, max_freqs_v, 16000 );


	if( max_freqs_v.empty() == true )
	{
		return;
	}
	std::sort(max_freqs_v.begin(), max_freqs_v.end(), compare );

	signal_type prev_freq = max_freqs_v[0].freq;

	size_t freq_count = max_freqs_v.size();

	size_t range_count = sizeof( global_ranges )/ sizeof( global_ranges[0] );

	for( size_t freq_i = 0 ; freq_i < freq_count ; freq_i++ )
	{
		freq_magn_type & curr_freq_magn = max_freqs_v[freq_i];
		float distance = abs( curr_freq_magn.freq - prev_freq );

		bool found = false;
		for( size_t range_i = 0 ; range_i < range_count ; range_i++ )
		{
			const int distance_threshold = global_ranges[range_i].distance;
			if( distance < distance_threshold )
			{
				found = true;
				freq_range_template.push_back( global_ranges[range_i].range_type );
//				cout<< distance_threshold << ", " << flush;
				break;
			}
		}
		prev_freq = curr_freq_magn.freq;
	}
//	cout << endl;

	freq_count = freq_range_template.size();


	destroy_fft( &complexes, &plan );
}

void template_freq_range_fill_distance_from_each_to_max( vector<signal_type> & signal, vector_mfcc_level_1 & freq_range_template )
{
	freq_range_template.clear();
	complex_type * complexes = NULL;
	fftwf_plan plan;
	make_fft( signal, &complexes, &plan );

	const int freqs_range_count = FREQUENCY_PROCESSED_COUNT;

	size_t complex_count = signal.size();

	vector<complex_type> complexes_v( complexes, complexes + complex_count );

	freqfilter( complexes_v, 16000, 50, 8000 );
	vector<freq_magn_type> max_freqs_v;
	getFrequencesWithHigherMagnitudes( freqs_range_count, complexes_v, max_freqs_v, 16000 );


	if( max_freqs_v.empty() == true )
	{
		return;
	}

	signal_type prev_freq = max_freqs_v[0].freq;

	size_t freq_count = max_freqs_v.size();


	size_t range_count = sizeof( global_ranges )/ sizeof( global_ranges[0] );

	for( size_t freq_i = 0 ; freq_i < freq_count ; freq_i++ )
	{
		freq_magn_type & curr_freq_magn = max_freqs_v[freq_i];
		float distance = abs( curr_freq_magn.freq - prev_freq );

		for( size_t range_i = 0 ; range_i < range_count ; range_i++ )
		{
			const int distance_threshold = global_ranges[range_i].distance;
			if( distance < distance_threshold )
			{
				freq_range_template.push_back( global_ranges[range_i].range_type );
				break;
			}
		}
	}

	freq_count = freq_range_template.size();

	destroy_fft( &complexes, &plan );
}

void make_fft( vector<signal_type> & data, complex_type ** fft_result, fftwf_plan * plan_forward )
{
	size_t data_count = data.size();
	*fft_result  = ( complex_type* ) fftwf_malloc( sizeof( complex_type ) * 2 * data_count );

	*plan_forward  = fftwf_plan_dft_r2c_1d( data_count, &data[0], *fft_result, FFTW_ESTIMATE );

	// Perform the FFT on our chunk
	fftwf_execute( *plan_forward );
}

void destroy_fft( complex_type ** fft_result, fftwf_plan * plan_forward )
{
	fftwf_destroy_plan( *plan_forward );
	*plan_forward = 0;
	fftwf_free( *fft_result );
	*fft_result = NULL;
}

void getFrequencesWithHigherMagnitudes( size_t frequencyCount, vector<complex_type> & complexes, vector<freq_magn_type> & freqs, size_t samplerate )
{
	double prev_max_magnitude = 1000000.0;
	size_t prev_max_magnitude_index = 0.0;
	size_t freqs_count = complexes.size();

	if( freqs_count < frequencyCount )
	{
		return;
	}

	static signal_type * freqs_ptr = (signal_type*)calloc( samplerate, sizeof( signal_type ) );
	memset( freqs_ptr, 0, samplerate * sizeof( signal_type ) );

	for( size_t bins_counter_i = 0 ; bins_counter_i < frequencyCount ; bins_counter_i++ )
	{
		float max_magnitude = 0.0;
		size_t max_magnitude_index = 0.0;
		for( size_t freq_i = 0 ; freq_i < freqs_count ; freq_i++ )
		{
			float magnitude = complex_magnitude( complexes[freq_i] );
			if( max_magnitude < magnitude && magnitude < prev_max_magnitude )
			{
				max_magnitude = magnitude;
				max_magnitude_index = freq_i;
			}
		}
		float freq = ((float)max_magnitude_index * samplerate / (float)freqs_count);
		freq_magn_type value = { freq, max_magnitude };
		freqs.push_back( value );

		prev_max_magnitude = max_magnitude;
		prev_max_magnitude_index = max_magnitude_index;
	}

	freqs_count = freqs.size();

//	free( freqs_ptr );
}

float get_minimum_amplitude( vector<float> & signal, float & peak_value, unsigned int & peak_index)
{
    unsigned int count_i = 0;
    float minimum = 1000.0;
    int maximum_index = 0.0;
    unsigned int count = signal.size();
    for( count_i = 1 ; count_i < count ; count_i++ )
    {
        if(signal[count_i] < 0)
        {
        	float curr = signal[count_i];

        	if( minimum < curr )
			{
        		minimum = curr;
				maximum_index = count_i;
			}
        }
    }
    peak_value = minimum;
    peak_index = maximum_index;
    return minimum;
}

float get_maximum_amplitude( vector<float> & signal, float & peak_value, unsigned int & peak_index)
{
    int count_i = 0;
    float maximum = 0.0;
    int maximum_index = 0.0;
    int count = signal.size();
    for( count_i = 1 ; count_i < ( count - 1 ) ; count_i++ )
    {
        if(signal[count_i] > 0)
        {
            float prev = signal[count_i - 1];
            float curr = signal[count_i];
            float next = signal[count_i + 1];
            if( prev < curr && next < curr)
            {
                if( maximum < curr )
                {
                    maximum = curr;
                    maximum_index = count_i;
                }
            }
        }
    }
    peak_value = maximum;
    peak_index = maximum_index;
    return maximum;
}

void trim_sides_periods_bellow_threshold( vector<signal_type> & perioded_signal, float threshold_amplitude, vector<signal_type> & perioded_signal_result )
{
	unsigned int estimate_signals_sum = 0;

	unsigned int signal_count = perioded_signal.size();

	{
		vector<signal_type> copy_src_signal = perioded_signal;
		bool found_first_above_threshold = false;

		for( unsigned int period_i = 0 ;  ; period_i++ )
		{
			int start_index = (signal_count - estimate_signals_sum - 1);
			vector<signal_type> period;

			if( (start_index < 0) )
			{
				break;
			}
			unsigned int estimate_signals = 0;
			period_prev_get( copy_src_signal, start_index, estimate_signals, period );
			estimate_signals_sum += estimate_signals;

			if( true == period.empty() )
			{
				break;
			}
			float max_value = 0.0;
			unsigned int max_index = 0;
			if( true == condition_check_signal_threshold( period, threshold_amplitude, max_value, max_index ) )
			{
				found_first_above_threshold = true;
			}
			if( true == found_first_above_threshold )
			{
				perioded_signal_result.clear();
				perioded_signal_result.assign( copy_src_signal.begin(), copy_src_signal.begin() + ( signal_count - estimate_signals_sum ) );
				break;
			}
		}
	}

	std::reverse( perioded_signal_result.begin(), perioded_signal_result.end() );


	{
		vector<signal_type> copy_src_signal = perioded_signal_result;
		bool found_first_above_threshold = false;

		estimate_signals_sum = -1;
		for( unsigned int period_i = 0 ;  ; period_i++ )
		{
			unsigned int start_index = ( estimate_signals_sum + 1 );
			vector<signal_type> period;
			if( (start_index >= signal_count) )
			{
				break;
			}
			unsigned int estimate_signals = 0;
			period_next_get( copy_src_signal, start_index, estimate_signals, period );
			estimate_signals_sum += estimate_signals;

			if( true == period.empty() )
			{
				break;
			}
			float max_value = 0.0;
			unsigned int max_index = 0;
			if( true == condition_check_signal_threshold( period, threshold_amplitude, max_value, max_index ) )
			{
				found_first_above_threshold = true;
			}
			if( true == found_first_above_threshold )
			{
				perioded_signal_result.clear();
				size_t copy_src_signal_count = copy_src_signal.size();
				perioded_signal_result.assign( copy_src_signal.begin() + estimate_signals_sum, copy_src_signal.end() );
				break;
			}
		}
	}

	size_t copy_src_signal_count = perioded_signal_result.size();

//	save_chunk_data( perioded_signal_result, 3 );

}

bool copy_periods_before_after_signal_index( vector<signal_type> & signal, float threshold_amplitude, unsigned int signal_index, unsigned int periods_count,
											 unsigned int max_perioded_chunks_count, vector<signal_type> & all_perioded_chunk, unsigned int & out_new_signal_index,
											 bool no_matter_bounds, COPY_SIGNAL_ERROR_E & error )
{
	unsigned int estimate_signals = 0;
	error = COPY_SIGNAL_ERROR_SUCCESS;

	size_t signal_count = signal.size();
	size_t all_perioded_chunk_count = 0;
	unsigned int estimate_signals_sum = 0;

	for( unsigned int periods_i = 0 ; periods_i < max_perioded_chunks_count ; periods_i++ )
	{
		bool need_break = false;

		vector<signal_type> one_perioded_chunk;
		for( unsigned int period_i = 0 ; period_i < periods_count ; period_i++ )
		{
			int start_index = (signal_index - estimate_signals_sum - 1);
			vector<signal_type> period;

			if( start_index < 0 )
			{
				if( false == no_matter_bounds )
				{
					error = COPY_SIGNAL_ERROR_NEED_PREV_SIGNAL;
					all_perioded_chunk.clear();
					return false;
				}
				else
				{
					need_break = true;
					break;
				}
			}
			period_prev_get( signal, start_index, estimate_signals, period );
			if( false == period.empty() )
			{
				one_perioded_chunk.insert( one_perioded_chunk.end(), period.begin(), period.end() );
			}
			else
			{
				need_break = true;
				break;
			}
			estimate_signals_sum += estimate_signals;
		}

		float max_value = 0.0;
		unsigned int max_index = 0;
		if( false == condition_check_signal_threshold( one_perioded_chunk, threshold_amplitude, max_value, max_index ) )
		{
			break;
		}
		else
		{
			all_perioded_chunk.insert( all_perioded_chunk.end(), one_perioded_chunk.begin(), one_perioded_chunk.end() );
			all_perioded_chunk_count = all_perioded_chunk.size();
		}

		if( true == need_break )
		{
			break;
		}
	}
	all_perioded_chunk_count = all_perioded_chunk.size();
	std::reverse( all_perioded_chunk.begin(), all_perioded_chunk.end() );

	all_perioded_chunk.push_back( signal[signal_index] );
	out_new_signal_index = all_perioded_chunk.size() - 1;

	estimate_signals_sum = 0;

	for( unsigned int periods_i = 0 ; periods_i < max_perioded_chunks_count ; periods_i++ )
	{
		bool need_break = false;

		vector<signal_type> one_perioded_chunk;
		for( unsigned int period_i = 0 ; period_i < periods_count ; period_i++ )
		{
			unsigned int start_index = (signal_index + estimate_signals_sum + 1);
			vector<signal_type> period;
			if( start_index >= signal_count )
			{
				if( false == no_matter_bounds )
				{
					error = COPY_SIGNAL_ERROR_NEED_NEXT_AND_PREV_SIGNAL;
					all_perioded_chunk.clear();
					return false;
				}
				else
				{
					need_break = true;
					break;
				}
			}
			period_next_get( signal, start_index, estimate_signals, period );
			if( false == period.empty() )
			{
				one_perioded_chunk.insert( one_perioded_chunk.end(), period.begin(), period.end() );
			}
			else
			{
				need_break = true;
				break;
			}
			estimate_signals_sum += estimate_signals;
		}

		size_t one_perioded_chunk_count = one_perioded_chunk.size();

		float max_value = 0.0;
		unsigned int max_index = 0;
		if( false == condition_check_signal_threshold( one_perioded_chunk, threshold_amplitude, max_value, max_index ) )
		{
			break;
		}
		else
		{
			all_perioded_chunk.insert( all_perioded_chunk.end(), one_perioded_chunk.begin(), one_perioded_chunk.end() );
		}

		if( true == need_break )
		{
			break;
		}
	}
	vector<signal_type> trimmed_perioded_signal;
	all_perioded_chunk_count = all_perioded_chunk.size();

	trim_sides_periods_bellow_threshold( all_perioded_chunk, threshold_amplitude, trimmed_perioded_signal );

	all_perioded_chunk.swap( trimmed_perioded_signal );

	all_perioded_chunk_count = all_perioded_chunk.size();

	error = COPY_SIGNAL_ERROR_SUCCESS;
	return true;
}

void autoCorrelation( vector<signal_type> & signal, vector<signal_type> & correlated_signal )
{
    size_t signal_length = signal.size();

    correlated_signal.resize( signal_length );
    float sum = 0;
    size_t i = 0, j = 0;

    for( i = 0 ; i < signal_length ; i++ )
    {
        sum=0;
        for( j = 0 ; j < signal_length - i ;  j++ )
        {
            sum += signal[ j ] * signal[ j + i ];
        }
        correlated_signal[ i ] = sum;
    }
}

float get_average_amplitude( vector<float> & signal, unsigned int range_min_frame, unsigned int range_max_frame)
{
    double average_amplitude_before_maximum = 0.0;
    double sum_amplitudes_before_maximum = 0.0;
    int significant_values_count_in_range_before = 0;
    unsigned int count_i = 0;
    for( count_i = range_min_frame ; count_i < ( range_max_frame - 1 ) ; count_i++ )
    {
        if(signal[count_i] > 0)
        {
            double prev = signal[count_i - 1];
            double curr = signal[count_i];
            double next = signal[count_i + 1];
            if( prev < curr && next < curr)
            {
                significant_values_count_in_range_before++;
                sum_amplitudes_before_maximum += signal[count_i];
            }
        }
    }

    average_amplitude_before_maximum = sum_amplitudes_before_maximum/significant_values_count_in_range_before;

    return average_amplitude_before_maximum;
}

void get_time_chunk_signal_corner( vector<float> & signal, double maximum, int maximum_index, bool prev, int & corner)
{
    {
    	size_t signal_count = signal.size();
        int bound_frame_near_maximum = 0;
        float average_amplitude_near_maximum = 0.0;
        float sin_value = 0.0;
        float katet_2 = 0.0;
        float delta_katet_1 = 0.0;
        float hipothenuze = 0.0;
        if( prev )
        {
            bound_frame_near_maximum = 1;// cause of 0-1 in function bellow

            average_amplitude_near_maximum = get_average_amplitude( signal, bound_frame_near_maximum, maximum_index);

            katet_2 = 1;
            delta_katet_1 = (maximum - average_amplitude_near_maximum) * katet_2;

            hipothenuze = std::sqrt( delta_katet_1*delta_katet_1 + katet_2*katet_2);

            sin_value = katet_2 / hipothenuze;
        }
        else
        {
            bound_frame_near_maximum = signal_count - 1;

            average_amplitude_near_maximum = get_average_amplitude( signal, maximum_index, bound_frame_near_maximum);

            katet_2 = (signal_count - maximum_index) / 2;
            delta_katet_1 = (maximum - average_amplitude_near_maximum) * katet_2;

            hipothenuze = std::sqrt( delta_katet_1*delta_katet_1 + katet_2*katet_2);

            sin_value = katet_2 / hipothenuze;
        }




        corner = (asin(sin_value) * 180 / M_PI - 45 ) * 2;
    }
}

bool condition_check_signal_threshold( vector<signal_type> & signal, float threshold_positive, float & peak_value, unsigned int & peak_index )
{
	peak_index = 0;

	get_maximum_amplitude( signal, peak_value, peak_index );


	cout << "max_amplitude=" << peak_value << endl;

	if( peak_value >= threshold_positive )
	{
		return true;
	}

	return false;
}

bool condition_check_signal_corner_shoulder_at_max( vector<signal_type> & signal, float maximum, unsigned int maximum_index )
{
	float high_threshold = 60;

	bool previous_corner = true;
	int corner_before = 0;
	get_time_chunk_signal_corner( signal, maximum, maximum_index, previous_corner, corner_before);

	if( corner_before > high_threshold )
	{
		cout << "corner_before=" << corner_before << " FAILED" << endl;
		return false;
	}

	previous_corner = false;
	int corner_after = 0;
	get_time_chunk_signal_corner( signal, maximum, maximum_index, previous_corner, corner_after);

	if( corner_after > high_threshold )
	{
		cout<< "corner_after=" << corner_after << " FAILED" << endl;
		return false;
	}

	cout<< "corner_after=" << corner_after << "; corner_before=" << corner_before << " SUCCESS" << endl;

	return true;
}

// Generate window function (Hanning) see http://www.musicdsp.org/files/wsfir.h
void wHanning( int fftFrameSize, vector<float> & hanning )
{
	hanning.resize( fftFrameSize );
	for( int k = 0; k < fftFrameSize ; k++)
	{
		hanning[k] = (-1) * 0.5 * cos( 2.0 * M_PI * (double)k / (double)fftFrameSize) + 0.5;
	}
}

void apply_hanning_window( vector<signal_type> & magnitudes )
{
	vector<float> hanning;
	const size_t magnitudes_count = magnitudes.size();
	wHanning( magnitudes_count, hanning );
//	float * hanning_pointer = &hanning[0];
	signal_type * magn_ptr = &magnitudes[0];
	for( size_t magnitude_i = 0; magnitude_i < magnitudes_count ; magnitude_i++)
	{
		float curr_magnitude = magnitudes[magnitude_i];
		double hann = hanning[magnitude_i];
		magnitudes[magnitude_i] = curr_magnitude * hann;
	}
}

void get_spectrum_magnitudes( vector<signal_type> & signal_chunk, vector<signal_type> & magnitudes)
{
	complex_type * fft_result = NULL;
	fftwf_plan plan = NULL;
	make_fft( signal_chunk, &fft_result, &plan );

	size_t FFTLen = signal_chunk.size();
	size_t specLen = FFTLen / 2;

	magnitudes.resize( specLen );

	for( size_t fft_i = 0 ; fft_i < specLen ; fft_i++ )
	{
		magnitudes[fft_i] = complex_magnitude( fft_result[fft_i] );
	}

	destroy_fft( &fft_result, &plan );
}

void get_mfcc_coefficients( vector<signal_type> & signal_chunk, vector<float> & mfcc_coefficients)
{
	vector<signal_type> magnitudes;
	get_spectrum_magnitudes( signal_chunk, magnitudes );

	size_t specLen = magnitudes.size();

	Mfcc mfcc;
	int numCoeffs = 14; // number of MFCC coefficients to calculate
	int numFilters = 32; // number of filters for MFCC calc

	mfcc_coefficients.resize( numCoeffs );

	mfcc.init( 16000, numFilters, specLen, numCoeffs );

	mfcc.getCoefficients( &magnitudes[0], &mfcc_coefficients[0] );
}

void mfcc_get_distance( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance )
{
	distance = 0.0f;
	size_t mfcc_1_size = mfcc_1.size();
	size_t mfcc_2_size = mfcc_2.size();
	if( mfcc_1_size == 0 )
	{
		return;
	}
	if( mfcc_1_size != mfcc_2_size )
	{
		return;
	}

	double square_sum = 0;

	for( size_t mfcc_i  = 0 ; mfcc_i < mfcc_1_size ; mfcc_i++ )
	{
		double delta = mfcc_1[mfcc_i] - mfcc_2[mfcc_i];
		double square = delta * delta;
		square_sum += square;
	}

	distance = sqrt( square_sum );
}

void dtw_get_distance( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance )
{
	distance = 0.0f;
	float * mfcc_1_ptr = &mfcc_1[0];
	float * mfcc_2_ptr = &mfcc_2[0];
	dtw_get_distance( mfcc_1_ptr, mfcc_2_ptr, mfcc_1.size(), mfcc_2.size(), distance );
}

void dtw_get_distance( float * mfcc_1, float * mfcc_2, unsigned int mfcc_1_size, unsigned int mfcc_2_size, double & distance )
{
	distance = 0.0f;
	if( mfcc_1_size == 0 )
	{
		return;
	}
	if( mfcc_1_size != mfcc_2_size )
	{
		return;
	}
	DTW dtw( mfcc_1_size, mfcc_2_size );

	distance = dtw.run( mfcc_1, mfcc_2, mfcc_1_size, mfcc_2_size );
}

void hmm_get_propability( vector_mfcc_level_1 & mfcc_1, vector_mfcc_level_1 & mfcc_2, double & distance )
{

}

void prepare_signal_near_peak( vector<signal_type> & present_signal, vector<signal_type> & past_signal, vector<signal_type> & future_signal, float threshold, unsigned int max_index, unsigned int periods_count_per_chunk, unsigned int max_chunks_count, vector<signal_type> & prepared_signal, unsigned int & new_max_index )
{
	COPY_SIGNAL_ERROR_E error = COPY_SIGNAL_ERROR_SUCCESS;
	COPY_SIGNAL_ERROR_E last_error = error;

	int temp_max_index = max_index;

	vector<signal_type> signal = present_signal;

	bool no_mater_bounds = false;

	while( true )
	{
		vector<signal_type> perioded_signal;
		bool bCopy = copy_periods_before_after_signal_index(signal, threshold, temp_max_index, periods_count_per_chunk, max_chunks_count, perioded_signal, new_max_index, no_mater_bounds, error );

		if( bCopy == true && error == COPY_SIGNAL_ERROR_SUCCESS )
		{
			size_t perioded_signal_count = perioded_signal.size();
			prepared_signal = perioded_signal;

			last_error = error;

			break;
		}
		else if( error == COPY_SIGNAL_ERROR_NEED_PREV_SIGNAL )
		{
			if( last_error == error )
			{
				no_mater_bounds = true;
			}
			else
			{
				no_mater_bounds = false;
			}

			signal.clear();
			signal.insert( signal.begin(), past_signal.begin(), past_signal.end() );
			signal.insert( signal.end(), present_signal.begin(), present_signal.end() );

			temp_max_index = max_index + past_signal.size();

			last_error = error;
		}
		else if( error == COPY_SIGNAL_ERROR_NEED_NEXT_AND_PREV_SIGNAL )
		{
			if( last_error == error )
			{
				no_mater_bounds = true;
			}
			else
			{
				no_mater_bounds = false;
			}
			signal.clear();
			signal.insert( signal.begin(), past_signal.begin(), past_signal.end() );
			signal.insert( signal.end(), present_signal.begin(), present_signal.end() );
			signal.insert( signal.end(), future_signal.begin(), future_signal.end() );

			last_error = error;
		}
		else
		{
			prepared_signal.clear();

			last_error = error;
			break;
		}
	}
}

void merge_and_split_with_cross( vector<vector<signal_type> > & perioded_chunks, unsigned int periods_count, vector<vector<signal_type> > & perioded_crossed_chunks )
{
	vector<signal_type> signal;

	size_t perioded_count = perioded_chunks.size();

	for( size_t perioded_i = 0; perioded_i < perioded_count ; perioded_i++ )
	{
		vector<signal_type> & perioded_chunk = perioded_chunks[perioded_i];
		signal.insert( signal.end(), perioded_chunk.begin(), perioded_chunk.end() );
	}

	size_t signal_length = signal.size();

	unsigned int perioded_offset_signal_index = 0;

	size_t signal_i = 0;
	for( size_t perioded_i = 0 ; signal_i < signal_length ; perioded_i++ )
	{
		vector<signal_type> cross_perioded_chunk;
		unsigned int start_copy_index = perioded_offset_signal_index;
		for( size_t period_i = 0 ; period_i < periods_count ; period_i++ )
		{
			unsigned int estimate_signals = 0;
			vector<signal_type> period;
			signal_i = start_copy_index;
			period_next_get( signal, start_copy_index, estimate_signals, period );
			if( false == period.empty() )
			{
				cross_perioded_chunk.insert( cross_perioded_chunk.end(), period.begin(), period.end() );
			}
			else
			{
				break;
			}
			signal_i += estimate_signals;
			start_copy_index += estimate_signals;

			if( period_i == ( periods_count/2 - 1) )
			{
				perioded_offset_signal_index = signal_i;
			}
		}

		if( false == cross_perioded_chunk.empty() )
		{
			perioded_crossed_chunks.push_back( cross_perioded_chunk );
		}
		else
		{
			continue;
		}
	}
}

struct CRange
{
    double min_value;
    double max_value;
    CRange():min_value(0), max_value(0.1){}
};

int get_amplitude_range_index( vector<CRange> ranges, double amplitude)
{
    int index = 0;
    size_t range_count = ranges.size();
    for( index = 0 ; index < range_count ; index++ )
    {
        if( amplitude > ranges[index].min_value && amplitude < ranges[index].max_value )
        {
            break;
        }
    }

    return index;
}

void get_most_meet_amplitude_range( vector<signal_type> & signal, float & low_range, float & high_range )
{
    float min_range_amplitude_value = 0.0;
    float max_range_amplitude_value = 1.0;
    float range_delta = 0.01;

    const int ranges_count = (max_range_amplitude_value - min_range_amplitude_value) / range_delta;

    vector<CRange> ranges( ranges_count );

    for(int i = 0 ; i < ranges_count ; i++ )
    {
        ranges[i].min_value = min_range_amplitude_value + range_delta*i;
        ranges[i].max_value = min_range_amplitude_value + range_delta*( i + 1 );
    }

    int most_meet_range_index = 0;

    const unsigned int max_amplitude_ranges_count = ranges_count;
    vector<int> range_arr( max_amplitude_ranges_count );
    size_t count = signal.size();
    for( unsigned int signal_i = 1 ; signal_i < ( count -1 ) ; signal_i++ )
    {
        if( signal[signal_i] > 0 )
        {
            double prev = signal[signal_i - 1];
            double curr = signal[signal_i];
            double next = signal[signal_i + 1];
            if( prev < curr && next < curr)
            {
                int range_index = 0;
                range_index = get_amplitude_range_index( ranges, signal[signal_i] );
                range_arr[range_index]++;
            }
        }
    }
    double temp_max = 0.0;
    for( unsigned int range_i = 0 ; range_i < max_amplitude_ranges_count ; range_i++ )
    {
        int range_meet = range_arr[range_i];
        if( temp_max < range_meet )
        {
            temp_max = range_meet;
            most_meet_range_index = range_i;
        }
    }
    low_range = ranges[most_meet_range_index].min_value;
    high_range = ranges[most_meet_range_index].max_value;
}

void get_atleast_meet_amplitude_range( vector<signal_type> & signal, unsigned int minimum_peaks_count, float & low_range, float & high_range )
{
    float min_range_amplitude_value = 0.0;
    float max_range_amplitude_value = 1.0;
    float range_delta = 0.01;

    const int ranges_count = (max_range_amplitude_value - min_range_amplitude_value) / range_delta;

    vector<CRange> ranges( ranges_count );

    for(int i = 0 ; i < ranges_count ; i++ )
    {
        ranges[i].min_value = min_range_amplitude_value + range_delta*i;
        ranges[i].max_value = min_range_amplitude_value + range_delta*( i + 1 );
    }

    const int max_amplitude_ranges_count = ranges_count;
    vector<int> range_arr( max_amplitude_ranges_count );
    size_t count = signal.size();
    for( unsigned int signal_i = 1 ; signal_i < ( count -1 ) ; signal_i++ )
    {
        if( signal[signal_i] > 0 )
        {
            double prev = signal[signal_i - 1];
            double curr = signal[signal_i];
            double next = signal[signal_i + 1];
            if( prev < curr && next < curr)
            {
                int range_index = 0;
                range_index = get_amplitude_range_index( ranges, signal[signal_i] );
                range_arr[range_index]++;
            }
        }
    }
    int minimum_meet_range_index = 0;
    for( int range_i = max_amplitude_ranges_count - 1 ; range_i >= 0 ; range_i-- )
    {
        int range_meet_count = range_arr[range_i];
        if( range_meet_count >= minimum_peaks_count )
        {
        	minimum_meet_range_index = range_i;
            break;
        }
    }
    low_range = ranges[minimum_meet_range_index].min_value;
    high_range = ranges[minimum_meet_range_index].max_value;
}

float get_mean_peak_amplitude( vector<signal_type> & signal )
{
    size_t count = signal.size();
    double sum_peak = 0.0;
    unsigned int peak_counter = 0;
    for( unsigned int signal_i = 1 ; signal_i < ( count -1 ) ; signal_i++ )
    {
        if( signal[signal_i] > 0 )
        {
            double prev = signal[signal_i - 1];
            double curr = signal[signal_i];
            double next = signal[signal_i + 1];
            if( prev < curr && next < curr)
            {
                sum_peak += curr;
                peak_counter += 1;
            }
        }
    }
    return (float)sum_peak/peak_counter;
}

void training_sound_template_remove_same( vector_mfcc_level_3 & all_templates_ranges, size_t & removed_number, size_t & remain_number )
{
	vector_mfcc_level_3::iterator sounds_begin_1 = all_templates_ranges.begin();
	vector_mfcc_level_3::iterator sounds_end_1 = all_templates_ranges.end();
	vector_mfcc_level_3::iterator sounds_iter_1 = sounds_begin_1;
	removed_number = 0;
	remain_number = 0;

	size_t remove_counter = 0;

	for(  ; sounds_iter_1 != sounds_end_1 ; sounds_iter_1++ )
	{
		bool bErased_in_sound = false;

		vector_mfcc_level_2 & templates = (*sounds_iter_1);
		vector_mfcc_level_2::iterator templates_begin_1 = templates.begin();
		vector_mfcc_level_2::iterator templates_end_1 = templates.end();
		vector_mfcc_level_2::iterator templates_iter_1 = templates_begin_1;
		for(  ; templates_iter_1 != templates_end_1 ;  )
		{
			vector_mfcc_level_1 & template_1 = (*templates_iter_1);

			bool bErased = false;



			vector_mfcc_level_3::iterator sounds_begin_2 = all_templates_ranges.begin();
			vector_mfcc_level_3::iterator sounds_end_2 = all_templates_ranges.end();
			vector_mfcc_level_3::iterator sounds_iter_2 = sounds_begin_2;

			for(  ; sounds_iter_2 != sounds_end_2 ; sounds_iter_2++ )
			{
				vector_mfcc_level_2 & templates = (*sounds_iter_2);
				vector_mfcc_level_2::iterator templates_begin_2 = templates.begin();
				vector_mfcc_level_2::iterator templates_end_2 = templates.end();
				vector_mfcc_level_2::iterator templates_iter_2 = templates_begin_2;
				for(  ; templates_iter_2 != templates_end_2 ;  )
				{
					if( templates_iter_1 == templates_iter_2 )
					{
						templates_iter_2++;
						continue;
					}
					vector_mfcc_level_1 & template_2 = (*templates_iter_2);
					if( freq_range_template_equal( template_1, template_2, 0 ) )
					{
						templates_iter_2 = templates.erase( templates_iter_2 );
						remove_counter++;
						bErased = true;
						break;
					}
					else
					{
						templates_iter_2++;
					}
				}
				if( true == bErased )
				{
					bErased_in_sound = true;
					break;
				}
			}

			if( true == bErased )
			{
				templates_begin_1 = templates.begin();
				templates_iter_1 = templates_begin_1;
				bErased_in_sound = false;
				removed_number ++;
			}
			else
			{
				templates_iter_1++;
			}

		}

		if( false == bErased_in_sound )
		{
			remain_number += templates.size();
		}
	}
}

void training_sound_template_remove_nearest( vector_mfcc_level_2 & templates_ranges_1, vector_mfcc_level_2 & templates_ranges_2, float min_distance, size_t & removed_number, size_t & remain_number )
{
	removed_number = 0;
	remain_number = 0;

	size_t remove_counter = 0;

	vector_mfcc_level_2::iterator templates_begin_1 = templates_ranges_1.begin();
	vector_mfcc_level_2::iterator templates_end_1 = templates_ranges_1.end();
	vector_mfcc_level_2::iterator templates_iter_1 = templates_begin_1;

	for(  ; templates_iter_1 != templates_end_1 ;  )
	{
		vector_mfcc_level_1 & template_1 = (*templates_iter_1);

		bool bErased = false;

		vector_mfcc_level_2::iterator templates_begin_2 = templates_ranges_2.begin();
		vector_mfcc_level_2::iterator templates_end_2 = templates_ranges_2.end();
		vector_mfcc_level_2::iterator templates_iter_2 = templates_begin_2;

		for(  ; templates_iter_2 != templates_end_2 ;  )
		{
			vector_mfcc_level_1 & template_2 = (*templates_iter_2);
			double distance = 0;
			mfcc_get_distance( template_1, template_2, distance );
			if( distance < min_distance )
			{
				templates_iter_2 = templates_ranges_2.erase( templates_iter_2 );
				templates_iter_1 = templates_ranges_1.erase( templates_iter_1 );
				remove_counter++;
				bErased = true;
				break;
			}
			else
			{
				templates_iter_2++;
			}
		}
		if( false == bErased )
		{
			templates_iter_1++;
		}
	}
	remain_number = 0;
	remain_number += templates_ranges_1.size();
	remain_number += templates_ranges_2.size();

	removed_number = remove_counter;
}
