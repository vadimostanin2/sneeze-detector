/*
 * sound_type.h
 *
 *  Created on: Nov 19, 2014
 *      Author: vostanin
 */

#ifndef SOUND_TYPE_H_
#define SOUND_TYPE_H_

#include <string>
using namespace std;

typedef enum
{
	SOUND_NONE = -1,
	SOUND_FIRST = 0,
	SOUND_OTHER = SOUND_FIRST + 0,
	SOUND_SNEEZING  = SOUND_FIRST + 1,
	SOUND_LAST = SOUND_SNEEZING,
}sound_recognition_type_e;

void sound_type_to_string( sound_recognition_type_e type, string & name );


#endif /* SOUND_TYPE_H_ */
