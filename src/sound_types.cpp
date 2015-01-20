/*
 * sound_type.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: vostanin
 */

#include "sound_types.h"

void sound_type_to_string( sound_recognition_type_e type, string & name )
{
	switch( type )
	{
		case SOUND_SNEEZING:
			name = "Sneezing";
		break;

		case SOUND_OTHER:
			name = "Noise";
		break;

		default:
			name = "Unknown";
		break;
	}
}
