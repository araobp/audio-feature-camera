#ifndef __AI_H__
#define __AI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ai_platform.h"
#include "dsp.h"

/**
 * Note on AI inference processing.
 *
 * Since the auto-generated "app_x-cube-ai.c" is not flexible enough,
 * I decided to implement common AI routines in "ai.h" and "ai.c".
 *
 * "ai.c" and "app_x-cube-ai.c" refer to the following global variables
 * define in "main.c":
 * - int8_t mfsc_buffer[NUM_FILTERS * 200];
 * - int8_t mfcc_buffer[NUM_FILTERS * 200];
 * - int32_t mfsc_power[200];
 * - int pos;
 */
extern int8_t mfsc_buffer[NUM_FILTERS * 200];
#ifndef FEATURE_MFSC
extern int8_t mfcc_buffer[NUM_FILTERS * 200];
#endif
extern int32_t mfsc_power[200];
extern int pos;

/**
 * Enable/disable AI
 */
#define INFERENCE

/**
 * Use case definition
 */
//#define MUSICAL_INSTRUMENT_RECOGNITION
#define KEY_WORD_DETECTION
//#define ENVIRONMENTAL_SOUND_CLASSIFICATION

/**
 * Feature definition
 */
#define FEATURE_MFSC
//#define FEATURE_MFCC
//#define CUTOFF 13U
#define WINDOW_LENGTH 64U

/*
 * Moving average of inference results
 */
#define HISTORY_LENGTH 5U

/**
 * Beam forming
 */
#define DISABLE_BEAMFORMING

/*--- Function prototypes ---*/
bool voice_active(int length, int32_t threshold);
int ai_init(void);
void ai_infer(ai_float *input_data, ai_float* output_data);

#ifdef __cplusplus
}
#endif

#endif /* __AI_H__ */
