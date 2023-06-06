#ifndef FSM_H
#define FSM_H

/**
 *   \author Matt Oates (mattoates@gmail.com)
 *   \version 1.0
 *   \date    2010-01-14
 *   \bug None so far.
 *   \warning This library hasn't been tested yet.
 *
 *   A library for constructing Finite State Machines. Originally for a behavioural state controller for a swarm of e-puck robots.
 *   Uses a doubly linked list to iterate over all states (might be slow if you have a vast state space).
 *   Using a flat array or hash of function pointers will be much faster than this implementation, however this was designed for an
 *   evolutionary algorithm where linear and dynamic treatment of the states for the sake of Crossover was a design consideration.
 */

/*Wrap the C for C++ use*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define a type for indexing the FSM linked list.
 */
typedef long fsm_index_t;

/**
 *   A packet of parameters available to a given behavioural state function above and beyond the robots state.
 */
typedef struct fsm_param_t {
	/* User defined data to be passed to a state function can be put here. */
	int value;
} fsm_param_t;

/**
 *   Define a behaviour to be a function pointer returning an int and taking a robot and allele value as the only parameters.
 *   The returned int value is the index of the next behavioural state to be executed in the FSM.
 */
typedef fsm_index_t (* fsm_behaviour_t) ( fsm_param_t * value );

/**
 *    Define a type for functions that can be iterated over all states.
 */
typedef fsm_iterable_func_t (* void) ( fsm_state_t * state );

/**
 *   A behavioural state is defined as having a given allele value and a function.
 */
typedef struct fsm_state_t {
	fsm_param_t param;
	fsm_behaviour_t behaviour;
	fsm_index_t num_transitions;
	fsm_index_t * transitions;
	float * transition_probs;

	/*Following members are for double linked list functionality*/
	fsm_index_t index;
	struct fsm_state_t * prev;
	struct fsm_state_t * next;
	} fsm_state_t;

/**
 *   Define an FSM as a doubley linked list of states.
 */
typedef struct fsm_t {
	fsm_index_t num_states;
	fsm_state_t * current;
	fsm_state_t * head;
	fsm_state_t * tail;
} fsm_t;

void createFSM( fsm_t * fsm );
void fsmDestroy( fsm_t * fsm );
void fsmDestroyStatesFromState( fsm_state_t * state );
fsm_index_t fsmAppendState( fsm_t * fsm, fsm_state_t * state );
fsm_index_t fsmInsertStateAfterState( fsm_state_t * state, fsm_state_t * after );
fsm_index_t fsmInsertStateAfterIndex( fsm_state_t * state, fsm_index_t after  ) ;
void fsmRemoveState( fsm_state_t * state );
void fsmRemoveStateAtIndex( fsm_index_t index );

/* End C++ Wrapping */
#ifdef __cplusplus
}
#endif

#endif
