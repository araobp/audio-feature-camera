
#ifdef __cplusplus
 extern "C" {
#endif
/**
  ******************************************************************************
  * @file           : app_x-cube-ai.c
  * @brief          : AI program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "ai_datatypes_defines.h"

#include "ai.h"
/*************************************************************************
  *
  */
void MX_X_CUBE_AI_Init(void)
{
    MX_UARTx_Init();
    /* USER CODE BEGIN 0 */
  /* Include
   #include "ai.h"
   */
#ifdef INFERENCE
  ai_init();
  lcd_init(&hi2c1);
#endif
    /* USER CODE END 0 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 1 */
#ifdef INFERENCE

#ifdef KEY_WORD_DETECTION
  char lcd_line1[5][16] = { ">< Restaurant ><", "<> Restaurant <>",
        "++ Restaurant ++", "** Restaurant **", "$$ Restaurant $$"};

#else
  char lcd_line1[5][16] = { "Scene           ", "Scene .         ",
        "Scene ..        ", "Scene ...       ", "Scene ....      " };
  #endif
  // Aliases of class labels.
  // Note: class labels are just number like 0, 1, 2... on CNN.
#ifdef MUSICAL_INSTRUMENT_RECOGNITION
  char class_labels[][20] = {"Piano", "Classical guitar", "Framenco guitar",
    "Blues harp", "Tin whistle", "Silence"};
  char lcd_line2[][16] = {"PIANO           ", "CLASSICAL GUITAR",
    "FRAMENCO GUITAR ", "BLUES HARP      ", "TIN WHISTLE     ",
    "SILENCE         "};
#endif
#ifdef MY_HOME_CLASSIFICATION
  char class_labels[][20] = { "chatting", "reading a book", "watching tv", "cocking",
      "hamster", "silence", "vacuum cleaner", "taking a shower",
      "washing machine", "doing the dishes", "walking", "piano", "stairs", "snack"};
  char lcd_line2[][16] = { "CHATTING        ", "READING A BOOK  ", "WATCHING TV     ",
       "COCKING         ", "HAMSTER         ", "SILENCE         ",
       "VACUUM CLEANER  ", "TAKING A SHOWER ", "WASHING MACHINE ", "DOING THE DISHES",
       "WALKING         ", "PIANO           ", "STAIRS          ", "SNACK           "};
#endif
#ifdef ENVIRONMENTAL_SOUND_CLASSIFICATION
  char class_labels[][20] = { "silence", "train", "station", "fastfood",
      "escalator", "mall", "ship", "mall2"};
    char lcd_line2[][16] = { "SILENCE         ", "TRAIN           ", "STATION         ",
                             "FASTFOOD        ", "ESCALATOR       ", "MALL            ",
                             "SHIP            ", "MALL2           "};
#endif
#ifdef KEY_WORD_DETECTION
  char class_labels[][20] = {
    "umai",
    "mazui",
    "oishii",
    "silence",
    "others",
    "a",
    "i",
    "o",
    "sushi",
    "sashimi",
    "gyouza",
    "hashi",
    "spoon",
    "fork",
    "syouyu",
    "sakana",
    "niku",
    "gohan"
  };
  char lcd_line2[][16] = {
    "     UMAI!      ",
    "     MAZUI!     ",
    "    OISHII!     ",
    "    SILENCE     ",
    "    OTHERS      ",
    "       A        ",
    "       I        ",
    "       O        ",
    "     SUSHI      ",
    "    SASHIMI     ",
    "     GYOZA      ",
    "     HASHI      ",
    "     SPOON      ",
    "      FORK      ",
    "     SYOUYU     ",
    "     SAKANA     ",
    "      NIKU      ",
    "     GOHAN      "
  };
#endif

  // Input data and output data of CNN
  ai_float in_data[AI_NETWORK_IN_1_SIZE];
  ai_float out_data[AI_NETWORK_OUT_1_SIZE] = { 0.0 };

  // Moving average
#ifndef KEY_WORD_DETECTION
  static ai_float out_hist[HISTORY_LENGTH][AI_NETWORK_OUT_1_SIZE] = { { 0.0 } };
  ai_float out_sum[AI_NETWORK_OUT_1_SIZE] = { 0.0 };
#endif
  float32_t max_value;
  static int current = 0;
  uint32_t class, max_idx;

  // Counter
  static int cnt = 0;

  int pos_start, window_start_idx, l;
  int idx_in, idx_buf;
  const int buf_length = 200 * NUM_FILTERS;

  if (start_inference) {
    l = WINDOW_LENGTH + ACTIVITY_OFFSET;
    if (pos > l) {
      pos_start = pos - l;
    } else {
      pos_start = 200 - (l - pos);
    }
    window_start_idx = pos_start * NUM_FILTERS;
    idx_in = 0;
    idx_buf = window_start_idx;
    for (int j = 0; j < WINDOW_LENGTH; j++) {
      if (idx_buf >= buf_length) {
        idx_buf = 0;
      }
      for (int i = 0; i < NUM_FILTERS; i++) {
        in_data[idx_in + i] = (ai_float) (mfsc_buffer[idx_buf + i]);
      }
      idx_in += NUM_FILTERS;
      idx_buf += NUM_FILTERS;
    }

    ai_infer(in_data, out_data);  // Inference

    arm_max_f32(out_data, AI_NETWORK_OUT_1_SIZE, &max_value, &max_idx);

#ifdef KEY_WORD_DETECTION
    class = max_idx
#else
    for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
      out_hist[current][i] = out_data[i];
      out_sum[i] = 0.0;
    }

    if (++current >= HISTORY_LENGTH) {
      current = 0;
    }

    // Output the result to LCD
    for (int j = 0; j < 5; j++) {
      for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
        out_sum[i] += out_hist[j][i];
      }
    }
    class = 0;
    for (int i = 1; i < AI_NETWORK_OUT_1_SIZE; i++) {
      if (out_sum[class] < out_sum[i]) {
        class = i;
      }
    }
#endif

    lcd_clear();
    lcd_string(lcd_line1[current], 16);
    lcd_newline();
    lcd_string(lcd_line2[class], 16);

    start_inference = false;

#ifdef INFERENCE_LOGGING
    /*
    printf("%lu,%lu,", max_idx, class);
    for (int i = 0; i < AI_NETWORK_OUT_1_SIZE-1; i++) {
      printf("%d,", (int)(out_data[i]*1000));  // 1000%
    }
    printf("%d\n", (int)(out_data[AI_NETWORK_OUT_1_SIZE-1])*1000);  // 1000%
    */
    printf("%lu\n", max_idx);
#else
    printf("\n-- Inference %d --\n", cnt++);
    for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
      printf(" %-12s%3d%%\n", class_labels[i], (int) (out_data[i] * 100));  // 100%
    }
#endif

#ifdef BLE
    uint8_t max_idx_uint8_t;
    max_idx_uint8_t = (uint8_t)max_idx;
    sendData(&max_idx_uint8_t, 1);
#endif
  }
#endif
    /* USER CODE END 1 */
}

/* Multiple network support --------------------------------------------------*/

#include <string.h>
#include "ai_datatypes_defines.h"

static const ai_network_entry_t networks[AI_MNETWORK_NUMBER] = {
    {
        .name = (const char *)AI_NETWORK_MODEL_NAME,
        .config = AI_NETWORK_DATA_CONFIG,
        .ai_get_info = ai_network_get_info,
        .ai_create = ai_network_create,
        .ai_destroy = ai_network_destroy,
        .ai_get_error = ai_network_get_error,
        .ai_init = ai_network_init,
        .ai_run = ai_network_run,
        .ai_forward = ai_network_forward,
        .ai_data_weights_get_default = ai_network_data_weights_get,
        .params = { AI_NETWORK_DATA_WEIGHTS(0),
                AI_NETWORK_DATA_ACTIVATIONS(0)},
    },
};

struct network_instance {
     const ai_network_entry_t *entry;
     ai_handle handle;
     ai_network_params params;
};

/* Number of instance is aligned on the number of network */
AI_STATIC struct network_instance gnetworks[AI_MNETWORK_NUMBER] = {0};

AI_DECLARE_STATIC
ai_bool ai_mnetwork_is_valid(const char* name,
        const ai_network_entry_t *entry)
{
    if (name && (strlen(entry->name) == strlen(name)) &&
            (strncmp(entry->name, name, strlen(entry->name)) == 0))
        return true;
    return false;
}

AI_DECLARE_STATIC
struct network_instance *ai_mnetwork_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst))
            return inst;
        else if ((!inst) && (gnetworks[i].entry == NULL))
            return &gnetworks[i];
    }
    return NULL;
}

AI_DECLARE_STATIC
void ai_mnetwork_release_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst)) {
            gnetworks[i].entry = NULL;
            return;
        }
    }
}

AI_API_ENTRY
const char* ai_mnetwork_find(const char *name, ai_int idx)
{
    const ai_network_entry_t *entry;

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry))
            return entry->name;
        else {
            if (!idx--)
                return entry->name;
        }
    }
    return NULL;
}

AI_API_ENTRY
ai_error ai_mnetwork_create(const char *name, ai_handle* network,
        const ai_buffer* network_config)
{
    const ai_network_entry_t *entry;
    const ai_network_entry_t *found = NULL;
    ai_error err;
    struct network_instance *inst = ai_mnetwork_handle(NULL);

    if (!inst) {
        err.type = AI_ERROR_ALLOCATION_FAILED;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry)) {
            found = entry;
            break;
        }
    }

    if (!found) {
        err.type = AI_ERROR_INVALID_PARAM;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    if (network_config == NULL)
        err = found->ai_create(network, found->config);
    else
        err = found->ai_create(network, network_config);
    if ((err.code == AI_ERROR_CODE_NONE) && (err.type == AI_ERROR_NONE)) {
        inst->entry = found;
        inst->handle = *network;
        *network = (ai_handle*)inst;
    }

    return err;
}

AI_API_ENTRY
ai_handle ai_mnetwork_destroy(ai_handle network)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        ai_handle hdl = inn->entry->ai_destroy(inn->handle);
        if (hdl != inn->handle) {
            ai_mnetwork_release_handle(inn);
            network = AI_HANDLE_NULL;
        }
    }
    return network;
}

AI_API_ENTRY
ai_bool ai_mnetwork_get_info(ai_handle network, ai_network_report* report)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_info(inn->handle, report);
    else
        return false;
}

AI_API_ENTRY
ai_error ai_mnetwork_get_error(ai_handle network)
{
    struct network_instance *inn;
    ai_error err;
    err.type = AI_ERROR_INVALID_PARAM;
    err.code = AI_ERROR_CODE_NETWORK;

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_error(inn->handle);
    else
        return err;
}

AI_API_ENTRY
ai_bool ai_mnetwork_init(ai_handle network, const ai_network_params* params)
{
    struct network_instance *inn;
    ai_network_params par;

    /* TODO: adding check ai_buffer activations/weights shape coherence */

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        par = inn->entry->params;
        if (params->activations.n_batches)
            par.activations = params->activations;
        else
            par.activations.data = params->activations.data;
        if (params->params.n_batches)
            par.params = params->params;
        else
            par.params.data = inn->entry->ai_data_weights_get_default();
        return inn->entry->ai_init(inn->handle, &par);
    }
    else
        return false;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_run(ai_handle network, const ai_buffer* input,
        ai_buffer* output)
{
    struct network_instance* inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_run(inn->handle, input, output);
    else
        return 0;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_forward(ai_handle network, const ai_buffer* input)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_forward(inn->handle, input);
    else
        return 0;
}

AI_API_ENTRY
 int ai_mnetwork_get_private_handle(ai_handle network,
         ai_handle *phandle,
         ai_network_params *pparams)
 {
     struct network_instance* inn;
     inn =  ai_mnetwork_handle((struct network_instance *)network);
     if (inn && phandle && pparams) {
         *phandle = inn->handle;
         *pparams = inn->params;
         return 0;
     }
     else
         return -1;
 }
#ifdef __cplusplus
}
#endif
