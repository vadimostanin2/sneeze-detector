/*
 * sound_processing_status.h
 *
 *  Created on: Jan 12, 2015
 *      Author: vostanin
 */

#ifndef SOUND_PROCESSING_STATUS_H_
#define SOUND_PROCESSING_STATUS_H_

typedef enum
{
	SOUND_STATUS_PREPARED_E,
    SOUND_STATUS_PROCESSING_E,
    SOUND_STATUS_RECOGNIZED_SNEEZE_E,
    SOUND_STATUS_RECOGNIZED_NOISE_E
} sound_processing_status_e;

#endif /* SOUND_PROCESSING_STATUS_H_ */
