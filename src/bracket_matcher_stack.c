#include "bracket_matcher_stack.h"
#include <stdio.h>

Bracket_Matcher_t* allocate_bracket_matcher() {
    Bracket_Matcher_t* matcher = calloc(sizeof(Bracket_Matcher_t), 1);
    return matcher;
}

size_t bracket_matcher_push_bracket(Bracket_Matcher_t* matcher) {
    Bracket_Matcher_Stack_t* current_stack = &matcher->stack;

    while (current_stack->p_next != NULL) {
        current_stack = current_stack->p_next;
        printf("push does not equal!\n");
    }

    if (current_stack->next_label_index >= BRACKET_MATCHER_BLOCK_ELEMENT_COUNT) {
        current_stack->p_next = calloc(sizeof(Bracket_Matcher_Stack_t), 1);
        current_stack = current_stack->p_next;
    }

    current_stack->stack_block[current_stack->next_label_index] = matcher->current_label;

    current_stack->next_label_index++;
    matcher->current_label++;

    return matcher->current_label - 1;
}

size_t bracket_matcher_pop_bracket(Bracket_Matcher_t* matcher) {
    Bracket_Matcher_Stack_t* current_stack = &matcher->stack;
    Bracket_Matcher_Stack_t* prev_to_current_stack;

    while (current_stack->p_next != NULL) {
        printf("pop not equal!\n");
        prev_to_current_stack = current_stack;
        current_stack = current_stack->p_next;
    }

    size_t matching_index = current_stack->stack_block[current_stack->next_label_index - 1];

    if (current_stack->next_label_index == 0 && &matcher->stack != current_stack) {
        free(current_stack);
        prev_to_current_stack->p_next = NULL;
        printf("freedom!!\n");
    } else {
        current_stack->next_label_index--;
    }

    return matching_index;
}

static void free_bracket_stack(Bracket_Matcher_Stack_t* stack) {
    if(stack->p_next != NULL) {
        printf("free right here\n");
        fflush(stdout);
        free_bracket_stack(stack->p_next);
    }

    printf("free default");
    fflush(stdout);
}

void free_bracket_matcher(Bracket_Matcher_t* matcher) {
    free_bracket_stack(&matcher->stack);
    free(matcher);
}