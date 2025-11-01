#ifndef WEB_H
#define WEB_H

#include <globals.h>

void send_key_status();
void init_web(uint8_t *task_ptr,
              key_data *kd_ptr, 
              unsigned long *learn_date_ptr, 
              JsonDocument *cfg,
              uint8_t *test);
void handle_web();

#endif 