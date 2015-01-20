/*
 * training_sound_config.h
 *
 *  Created on: Nov 19, 2014
 *      Author: vostanin
 */

#ifndef TRAINING_SOUND_CONFIG_H_
#define TRAINING_SOUND_CONFIG_H_

#include <stdio.h>

#include "sound_types.h"
#include "common.h"
#include "recordcallback.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

typedef struct
{
	sound_recognition_type_e type;
	vector<string> files;
	char folder[BUFSIZ];
	float error;
}sound_template_t;

typedef std::map<sound_recognition_type_e, sound_template_t > trainig_config_map;

typedef struct
{
	trainig_config_map trainig_sounds;

	record_process_callback record_callback;
	unsigned int processing_periods_count;
}trainig_sounds_config_t;

void training_sounds_config_get( trainig_sounds_config_t & config );
bool training_sounds_get_sound_type( unsigned int sound_index, sound_recognition_type_e & out_type );


#endif /* TRAINING_SOUND_CONFIG_H_ */
