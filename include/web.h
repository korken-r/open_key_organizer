#ifndef WEB_H
#define WEB_H

#include <globals.h>

void send_status();
void init_web(uint8_t *task_ptr, key_data *kd_ptr, unsigned long *learn_date);
void handle_web();

#endif 