/*
 * training_sound_config.cpp
 *
 *  Created on: Nov 19, 2014
 *      Author: vostanin
 */

#include "training_sound_config.h"
#include <string.h>

static void training_sounds_config_add( trainig_sounds_config_t & config, sound_recognition_type_e type, std::string & folder, std::string & file_mask )
{
	trainig_config_map::iterator iter = config.trainig_sounds.find( type );
	if( iter != config.trainig_sounds.end() )
	{
		sound_template_t & snd_template = (*iter).second;
		snd_template.files.push_back( file_mask );
	}
	else
	{
		sound_template_t snd_template;

		snd_template.type = type;
		snd_template.files.push_back( file_mask );
		strcpy( snd_template.folder, folder.c_str() );

		config.trainig_sounds[type] = snd_template;

	}
}

static bool training_sounds_config_init( trainig_sounds_config_t & config )
{
//	std::string relative_folder = "train_sounds/";
	std::string relative_folder;
	std::string file_mask = "";

	relative_folder = "16000";
	file_mask = "o.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "e.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "e_2.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "silence_all.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "breath_noise.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "knocks.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_sh.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sh-sh.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sh-sh.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_2.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_3.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_4.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_5.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_6.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_7.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_8.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_9.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_10.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_11.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_12.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_13.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_14.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "noise_15.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_OTHER, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneezing_fake_1.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneezing_real_1.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template_full.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template_full_2.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template_full_noised.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template_full_noised_2.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	relative_folder = "16000";
	file_mask = "sneeze_template_full_noised_3.wav";
	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

//	relative_folder = "16000";
//	file_mask = "sneeze_template_full_noised_4.wav";
//	training_sounds_config_add( config, (sound_recognition_type_e)SOUND_SNEEZING, relative_folder, file_mask );

	config.processing_periods_count = 60;

	return true;
}

void training_sounds_config_get( trainig_sounds_config_t & config )
{
	static trainig_sounds_config_t config_instance;
	static bool init = training_sounds_config_init( config_instance );

	if( true == init )
	{
		config.trainig_sounds.clear();
		config.trainig_sounds = config_instance.trainig_sounds;
		config.processing_periods_count = config_instance.processing_periods_count;
	}
}
