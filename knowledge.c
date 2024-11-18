/* -----------------------------------------------------------------------------
   Chatbot knowledge base functions.
   Team ID:      
   Team Name:    
   Filename:     knowledge.c
   Version:      2024-1.0
   Description:  C source for helper function file in ICT1503C Project.
   Module:       ICT1503C
   Skeleton 
   Prepared by:  Nicholas H L Wong
   Organisation: Singapore Institute of Technology
   Division:     Infocomm Technology
   Credits:      Parts of this project are based on materials contributed to by 
                 other SIT colleagues.

   -----------------------------------------------------------------------------
 */

/*
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1503C.h"

// Initialize the knowledge base and count
KnowledgeEntry *knowledge_base = NULL;
int knowledge_count = 0;  // Track the number of entries in the knowledge base

// Function to resize the knowledge base dynamically
int knowledge_resize(int new_size) {
    KnowledgeEntry *new_base = realloc(knowledge_base, new_size * sizeof(KnowledgeEntry));
    if (new_base == NULL) {
        return KB_NOMEM;  // If memory allocation fails
    }
    knowledge_base = new_base;
    return KB_OK;
}

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {

    if (!(strcmp(intent, "what") == 0 || strcmp(intent, "where") == 0 || strcmp(intent, "who") == 0)) {
        return KB_INVALID;
    }
    
    // Loop through knowledge base
    for (int i = 0; i < knowledge_count; i++) {
        if (strcmp(knowledge_base[i].intent, intent) == 0 &&
            strcmp(knowledge_base[i].entity, entity) == 0) {
            strncpy(response, knowledge_base[i].response, n);
            return KB_OK;
        }
    }

    return KB_NOTFOUND;
}

/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {
    if (strcmp(intent, "what") != 0 && strcmp(intent, "where") != 0 && strcmp(intent, "who") != 0) {
        return KB_INVALID;
    }

    // Resize the knowledge base array to accommodate the new entry
    if (knowledge_resize(knowledge_count + 1) != KB_OK) {
        return KB_NOMEM;  // Return error if memory allocation fails
    }

    for (int i = 0; i < knowledge_count; i++) {
        if (strcmp(knowledge_base[i].intent, intent) == 0 &&
            strcmp(knowledge_base[i].entity, entity) == 0) {
            printf("Duplicate detected. Skipping: %s=%s\n", entity, response);
            return 1; // Duplicate entry found
        }
    }

    // Add the new entry
    strncpy(knowledge_base[knowledge_count].intent, intent, MAX_INTENT);
    strncpy(knowledge_base[knowledge_count].entity, entity, MAX_ENTITY);
    strncpy(knowledge_base[knowledge_count].response, response, MAX_RESPONSE);

    knowledge_count++;  // Increment the count of knowledge entries
    return KB_OK;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
    char line[MAX_INPUT], intent[MAX_INTENT] = "";
    int count = 0;

    // Read file line by line
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '[') { 
            // Capture the intent (section header)
            sscanf(line, "[%31[^]]]", intent);
        } else {
            char entity[MAX_ENTITY], response[MAX_RESPONSE];
            // Capture entity and response
            if (sscanf(line, "%63[^=]=%255[^\n]", entity, response) == 2) {
                if (knowledge_put(intent, entity, response) == KB_OK) {
                    count++;
                }
            }
        }
    }
    return count;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
    free(knowledge_base);
    knowledge_base = NULL;
    knowledge_count = 0;
}

void knowledge_sort() {
    for (int i = 0; i < knowledge_count - 1; i++) {
        for (int j = i + 1; j < knowledge_count; j++) {
            if (strcmp(knowledge_base[i].intent, knowledge_base[j].intent) > 0) {
                KnowledgeEntry temp = knowledge_base[i];
                knowledge_base[i] = knowledge_base[j];
                knowledge_base[j] = temp;
            }
        }
    }
}

void knowledge_write(FILE *f) {
    printf("Writing knowledge to file...\n");

    const char *current_intent = NULL;
    for (int i = 0; i < knowledge_count; i++) {
        if (current_intent == NULL || strcmp(current_intent, knowledge_base[i].intent) != 0) {
            fprintf(f, "\n[%s]\n", knowledge_base[i].intent);
            current_intent = knowledge_base[i].intent;
        }
        fprintf(f, "%s=%s\n", knowledge_base[i].entity, knowledge_base[i].response);
    }
}

