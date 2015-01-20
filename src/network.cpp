/*
 * network.c
 *
 *  Created on: Dec 31, 2014
 *      Author: vostanin
 */

#include "network.h"
#include "fann/floatfann.h"

struct fann *ann = NULL;

static struct fann_train_data * FANN_API fann_create_train_my( unsigned int num_data, unsigned int num_input, unsigned int num_output );

static fann_train_data * prepeare_train_data( vector_mfcc_level_3 & sounds_mfcc_v )
{
	if( true == sounds_mfcc_v.empty() )
	{
		return NULL;
	}

	if( true == sounds_mfcc_v[0].empty() )
	{
		return NULL;
	}

	size_t num_data = 0, num_input = 0, num_output = 0;

	size_t sounds_mfcc_count = sounds_mfcc_v.size();

	num_input = sounds_mfcc_v[0][0].size();
	num_output = sounds_mfcc_count;

	for( size_t sounds_mfcc_i = 0 ; sounds_mfcc_i < sounds_mfcc_count ; sounds_mfcc_i++ )
	{
		num_data += sounds_mfcc_v[sounds_mfcc_i].size();
	}

	fann_train_data * train_data = fann_create_train_my( num_data, num_input, num_output );

	unsigned int data_i = 0;
	for( size_t sounds_i = 0 ; sounds_i < sounds_mfcc_count ; sounds_i++ )
	{
		if( true == sounds_mfcc_v[sounds_i].empty() )
		{
			return NULL;
		}

		size_t mfcc_v_count = sounds_mfcc_v[sounds_i].size();

		for( unsigned int mfcc_v_i = 0 ; mfcc_v_i < mfcc_v_count ; mfcc_v_i++, data_i++ )
		{
			for( unsigned int input_i = 0 ; input_i <  train_data->num_input ; input_i++ )
			{
				train_data->input[data_i][input_i] = sounds_mfcc_v[sounds_i][mfcc_v_i][input_i];
			}
			fann_type * output = train_data->output[data_i];
			memset( output, 0, num_output );
			train_data->output[data_i][sounds_i] = 1;
		}
	}

	return train_data;
}

bool network_teach( vector_mfcc_level_3 & sounds_mfcc_v )
{
	if( true == sounds_mfcc_v.empty() )
	{
		return false;
	}

	const size_t sounds_mfcc_v_count = sounds_mfcc_v.size();
	const unsigned int num_input_layers = 1;
	const unsigned int num_output_layers = 1;

	fann_train_data *train_data = prepeare_train_data( sounds_mfcc_v );

	const unsigned int num_input = train_data->num_input;
	const unsigned int num_output = train_data->num_output;
	const unsigned int num_data = train_data->num_data;
	const unsigned int num_neurons_hidden[] = { 5 };
	const unsigned int num_hidden_layers = sizeof(num_neurons_hidden)/sizeof(num_neurons_hidden[0]);
	const unsigned int num_layers = num_input_layers + num_hidden_layers + num_output_layers ;
	const float desired_error = (const float) 0.005;
	const unsigned int max_epochs = 5000;
	const unsigned int epochs_between_reports = 10;

	ann = fann_create_standard( num_layers, num_input, num_neurons_hidden[0], num_output );

	fann_train_on_data( ann, train_data, max_epochs, epochs_between_reports, desired_error);

	fann_save( ann, "nn.net" );

	for( unsigned int data_i = 0 ; data_i <  train_data->num_data ; data_i++ )
	{
			fann_type * desired_ouput = new fann_type;
			* desired_ouput = 1;
			fann_type * calc_out = fann_run( ann, train_data->input[data_i] );
			fann_type * desired = train_data->output[data_i];
			printf("desired [%d]=%f, %f, %f, %f, %f, %f, %f\n", data_i, desired[0], desired[1], desired[2], desired[3], desired[4], desired[5], desired[6]);fflush(stdout);
			printf("test    [%d]=%f, %f, %f, %f, %f, %f, %f\n", data_i, calc_out[0], calc_out[1], calc_out[2], calc_out[3], calc_out[4], calc_out[5], calc_out[6]);fflush(stdout);
	}

	return true;
}

void network_load()
{
	ann = fann_create_from_file( "nn.net" );
}

void network_test( vector_mfcc_level_2 & mfcc_v, vector<float> & outputs_result )
{

}


static struct fann_train_data * FANN_API fann_create_train_my(unsigned int num_data, unsigned int num_input, unsigned int num_output)
{
	fann_type *data_input, *data_output;
	unsigned int i;
	struct fann_train_data *data =
		(struct fann_train_data *) malloc(sizeof(struct fann_train_data));

	if(data == NULL)
	{
		fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
		return NULL;
	}

	fann_init_error_data((struct fann_error *) data);

	data->num_data = num_data;
	data->num_input = num_input;
	data->num_output = num_output;
	data->input = (fann_type **) calloc(num_data, sizeof(fann_type *));
	if(data->input == NULL)
	{
		fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
		fann_destroy_train(data);
		return NULL;
	}

	data->output = (fann_type **) calloc(num_data, sizeof(fann_type *));
	if(data->output == NULL)
	{
		fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
		fann_destroy_train(data);
		return NULL;
	}

	data_input = (fann_type *) calloc(num_input * num_data, sizeof(fann_type));
	if(data_input == NULL)
	{
		fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
		fann_destroy_train(data);
		return NULL;
	}

	data_output = (fann_type *) calloc(num_output * num_data, sizeof(fann_type));
	if(data_output == NULL)
	{
		fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
		fann_destroy_train(data);
		return NULL;
	}

	for(i = 0; i != num_data; i++)
	{
		data->input[i] = data_input;
		data_input += num_input;
		data->output[i] = data_output;
		data_output += num_output;
	}
	return data;
}
