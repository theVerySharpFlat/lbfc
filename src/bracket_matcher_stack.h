#ifndef BRACKET_MATCHER_STACK_H
#define BRACKET_MATCHER_STACK_H

#include <stdlib.h>

#define BRACKET_MATCHER_BLOCK_ELEMENT_COUNT 1

typedef struct {
    size_t next_label_index;

    size_t stack_block[BRACKET_MATCHER_BLOCK_ELEMENT_COUNT];

    struct Bracket_Matcher_Stack_t* p_next;
} Bracket_Matcher_Stack_t;

typedef struct {
    size_t current_label;
    Bracket_Matcher_Stack_t stack;
} Bracket_Matcher_t;

Bracket_Matcher_t* allocate_bracket_matcher();

void free_bracket_matcher(Bracket_Matcher_t* matcher);

/**
 * @return The new label number
 */
size_t bracket_matcher_push_bracket(Bracket_Matcher_t* matcher);

/**
 * @return The matching opening label
 */
size_t bracket_matcher_pop_bracket(Bracket_Matcher_t* matcher);

#endif