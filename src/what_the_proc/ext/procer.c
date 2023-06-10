/**
* 
* MIT License
* 
* Copyright (c) 2023 Arun a.k.a beyonddream

* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/event.h>

#include "include/procer.h"

process_infos_s *procer_process_info_all_init(void)
{
    process_infos_s *process_infos = malloc(sizeof(*process_infos));

    if (process_infos == NULL) {
        printf("procer_process_info_all_init::malloc failed.\n");
        exit(1);
    }

    SLIST_INIT(&process_infos->proc_info);
    
    return process_infos;
}

void procer_process_info_all_deinit(process_infos_s *process_infos)
{
    
    SLIST_EMPTY(&process_infos->proc_info);
    free(process_infos);
    
    return;
}


const char *procer_get_name(void) 
{
    const char *current_process_name = "what_the_proc";
    
    return current_process_name;
}

process_infos_s *procer_get_process_info_all(void)
{
    struct kevent events[MAX_EVENTS];
    int kq;

    kq = kqueue();
    
    if (kq == -1) {
        printf("Failed to create kqueue.\n");
        return NULL;
    }

    /* Register the process creation and destruction events */
    struct kevent event;
    EV_SET(&event, 1, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT | NOTE_EXEC, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        printf("Failed to register for process events.\n");
        return NULL;
    }    
    
    for(;;) {
        int event_count = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (event_count == -1) {
            printf("Error in kevent.\n");
            return false;
        }

        struct kevent event;
        for (int i = 0; i < event_count; i++) {
            event = events[i];
            pid_t pid = event.ident;
            if (event.fflags & NOTE_EXIT) {
                printf("Process with PID (%d) has ended.\n", pid);
            } else if (event.fflags & NOTE_EXEC) {
                printf("Process with PID (%d) has started.\n", pid);
            }
        }
    }

    return NULL;
}   