#include <stdint.h>
#include <stdbool.h>
#include <microkit.h>
#include <stddef.h>
#include "printf.h"
#include "wordle.h"

#define CLIENT_CHANNEL_ID 0
#define VMM_CHANNEL_ID 1
/*
 * Here we initialise the word to "hello", but later in the tutorial
 * we will actually randomise the word the user is guessing.
 */
char word[WORD_LENGTH] = { 'h', 'e', 'l', 'l', 'o' };

bool is_character_in_word(char *word, int ch) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (word[i] == ch) {
            return true;
        }
    }

    return false;
}

enum character_state char_to_state(int ch, char *word, uint64_t index) {
    if (ch == word[index]) {
        return CORRECT_PLACEMENT;
    } else if (is_character_in_word(word, ch)) {
        return INCORRECT_PLACEMENT;
    } else {
        return INCORRECT;
    }
}

void init(void) {
    microkit_dbg_puts("WORDLE SERVER: starting\n");
}

void notified(microkit_channel channel) {}

microkit_msginfo protected(microkit_channel ch, microkit_msginfo msginfo){
    switch (ch) {
        case CLIENT_CHANNEL_ID:
            int len = microkit_msginfo_get_count(msginfo);
            for(int i = 0; i < len; i++){
                char c = microkit_mr_get(i);
                enum character_state res_state = char_to_state(c, word, i);
                microkit_mr_set(i, res_state);
            }
            return microkit_msginfo_new(0, len);
            break;

        case VMM_CHANNEL_ID:
            for(int i = 0; i < WORD_LENGTH; i++){
                word[i] = microkit_mr_get(i);
            }
            break;
        
        default:
            break;
    }

    return microkit_msginfo_new(0, 0);
    
}
