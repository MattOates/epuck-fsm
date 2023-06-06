#include <stdlib.h>;
#include "fsm.h";

/**
 *   Allocate some memory to store our FSM and set the default values.
 *   \param fsm a NULL pointer to the Finite State Machine to be created.
 */
void fsmCreate( fsm_t * fsm ) {
	fsm = ( fsm_t * ) malloc( sizeof( fsm_t ) );
	fsm->num_states = 0;
	fsm->current = NULL;
	fsm->head = NULL;
	fsm->tail = NULL;
}

/**
 *   Free up all memory for a given FSM, including all linked states and the fsm struct itself.
 *   \param fsm a pointer to the Finite State Machine to be destroyed.
 */
void fsmDestroy( fsm_t * fsm ) {
	fsmDestroyStatesFromState( fsm->head );
	free( fsm );
}

/**
 * A function compatible with fsm_iterable_func_t to decrament the index of a given state.
 */
void fsmDecStateIndex(fsm_state_t * state) {
    state->index--;
}

/**
 * A function compatible with fsm_iterable_func_t to incrament the index of a given state.
 */
void fsmIncStateIndex(fsm_state_t * state) {
    state->index++;
}

/**
 *   Recursive function to get a state at a desired index from a given start state.
 */
fsm_state_t fsmGetStateAtIndexFromState(fsm_index_t desired, fsm_state_t start) {
    if (desired == start->index) {
        return start;
    }
    /*Go "left" along the list*/
    else if (desired < start->index) {
        if (start->prev == NULL) return NULL;
        return fsmGetStateAtIndexFromState(desired, start->prev);
    }
    /*Go "right" along the list*/
    else {
        if (start->next == NULL) return NULL;
        return fsmGetStateAtIndexFromState(desired, start->next);
    }
}

/**
 *   Get a desired state by index.
 *   TODO Optimise this function to pick the end with shortest path to get the desired state.
 */
fsm_state_t fsmGetStateAtIndex(fsm_t fsm, fsm_index_t desired) {
    /*Work out the distance in hops to the desired state
       from the current and tail states in the fsm*/
    fsm_index_t dist_from_current = labs(fsm->current - desired);
    fsm_index_t dist_from_end = labs(fsm->tail - desired);

    /*Default to head which means distance to state is desired hops away.*/
    fsm_state_t start = fsm->head;

    /*Should we change to starting from the current state?*/
    if (desired > dist_from_current) start = fsm->current;

    /*Should we change to starting from the tail state in the list?*/
    if ((dist_from_current > dist_from_end) && (desired > dist_from_end)) start = fsm->tail;

    /*Recurse along the desired direction to retrieve the state we wanted.*/
    return fsmGetStateAtIndexFromState(desired, start)
}

/**
 *   Perform an iterable function over a contiguous subset of the state list
 *   given a start and stop index.
 */
fsmIterateFromIndexToIndex ( fsm_index_t start, fsm_index_t end, fsm_iterable_func_t function) {
    fsm_state_t * current = fsmGetStateAtIndex(start);

    /* Makesure we have the next state to begin with
       as "function" might alter the next value. */
    next = current->next;

    /* Keep going over the list until we are inspecting the end state. */
    while(current->index <= end) {
        function(current);
        current = next;
        next = current->next;
    }

}

/**
 *   Perform an iterable function over a contiguous subset of the state list
 *   given a start and stop state.
 */
fsmIterateFromStateToState ( fsm_state_t start, fsm_state_t end, fsm_iterable_func_t function) {
    fsm_state_t * current = start;

    /* Makesure we have the next state to begin with
       as "function" might alter the next value. */
    next = current->next;

    /* Keep going over the list until we are inspecting the end state. */
    while(current->index <= end) {
        function(current);
        current = next;
        next = current->next;
    }
}

/**
 *   Recurse from a given starting state and free all memory allocated for following states. Useful if doing slice and append type crossover.
 */
void fsmDestroyStatesFromState( fsm_state_t * state) {

	/* Return if asked to destroy an empty point in the linked list, perhaps the head is empty. */
	if (state == NULL) return;

	/* If the current state has allocated states following free those first. */
	if (state->next != NULL) {
		fsmDestroyStatesFromState( state->next );
	}

	/* Otherwise free all the memory for the current state and return. */
	free( state->transitions );
	free( state->transition_probs );
	free( state );
}

/**
 *   Append a state to the end of the state list.
 */
fsm_index_t fsmAppendState(fsm_t * fsm, fsm_state_t * state) {
	if (fsm->head == NULL) {
		state->prev = NULL;
		fsm->head = state;
	} else {
		state->prev = fsm->tail;
		fsm->tail->next = state;
	}
	state->next = NULL;
	fsm->tail = state;
	state->index = fsm->num_states;
	fsm->num_states++;
}

/**
 *   Insert a state in the state list after the given reference.
 */
fsm_index_t fsmInsertStateAfterState( fsm_state_t * state, fsm_state_t * after) {

}

/**
 *   Insert a state in the state list after the given index.
 */
fsm_index_t fsmInsertStateAfterIndex( fsm_state_t * state, fsm_index_t after) {

}

/**
 *   Insert a state in the state list after the given index.
 */
void fsmRemoveState(fsm_state_t * state) {
    /*Link previous and next node.*/
    state->prev->next = state->next;
    state->next->prev = state->prev;

    fsmIterateFromIndex() {

    }

    /*Free the memory for this state*/
	free( state->transitions );
	free( state->transition_probs );
	free( state );
}

/**
 *   Insert a state in the state list after the given index.
 */
void fsmRemoveStateAtIndex(fsm_index_t index) {}
