#ifndef FSM_H
#define FSM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//
typedef int state_t;
typedef struct fsm fsm_t;
typedef struct fsm_trans fsm_trans_t;

typedef uint8_t (*fsm_input_func_t)(fsm_t *);
typedef void (*fsm_output_func_t)(fsm_t *);

//
struct fsm_trans {
    state_t orig_state;                        /* Origin state */
    fsm_input_func_t in;                       /* Input function */
    state_t dest_state;                        /* Destination state */
    fsm_output_func_t out;                     /* Output function */
};

struct fsm {
    state_t current_state;                     /* FSM current state */
    fsm_trans_t *tt;                           /* FSM transitions */
};

//
fsm_t *fsm_new(fsm_trans_t *tt);
void fsm_init(fsm_t *this, fsm_trans_t *tt);
void fsm_fire(fsm_t *this);


#ifdef __cplusplus
} //end extern "C"
#endif /* __cplusplus */

#endif /* FSM_H */
