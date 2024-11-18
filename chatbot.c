#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chat1503C.h"

// Function to convert a string to lowercase
void to_lower_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

const char *chatbot_botname() {
    return "Chatbot";
}

const char *chatbot_username() {
    return "User";
}

int chatbot_main(int inc, char *inv[], char *response, int n) {

    FILE *knowledge_file = fopen("ICT1503C_Project_Sample.ini", "r"); 
    if (knowledge_file == NULL) {
        snprintf(response, n, "Error: Unable to open file.");
        return 0;
    }

    // Read the knowledge base from the file
    knowledge_read(knowledge_file);
    fclose(knowledge_file); 

    to_lower_case(inv[0]);

    if (chatbot_is_exit(inv[0]))
        return chatbot_do_exit(inc, inv, response, n);
    else if (chatbot_is_load(inv[0]))
        return chatbot_do_load(inc, inv, response, n);
    else if (chatbot_is_save(inv[0]))
        return chatbot_do_save(inc, inv, response, n);
    else if (chatbot_is_question(inv[0]))
        return chatbot_do_question(inc, inv, response, n);
    else if (chatbot_is_reset(inv[0]))
        return chatbot_do_reset(inc, inv, response, n);
    else {
        snprintf(response, n, "I don't understand, could you try again?");
        return 0;
    }
}

int chatbot_is_exit(const char *intent) {
    return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
}

int chatbot_do_exit(int inc, char *inv[], char *response, int n) {
    snprintf(response, n, "Goodbye!");
    return 1;
}

int chatbot_is_load(const char *intent) {
    return compare_token(intent, "load") == 0;
}

int chatbot_do_load(int inc, char *inv[], char *response, int n) {
    if (inc < 3) {
        snprintf(response, n, "Please specify a file to load from.");
        return 0;
    }
    snprintf(response, n, "Loaded knowledge from file \"%s\".", inv[2]);
    return 0;
}

int chatbot_is_question(const char *intent) {
    return compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "who") == 0;
}

int chatbot_do_question(int inc, char* inv[], char* response, int n)
{
    if (inc < 1) {
        snprintf(response, n, "");
        return 0;
    }

    char entity[MAX_ENTITY] = "";           
    char intent[MAX_INTENT] = "";           
    char reply[MAX_RESPONSE] = "";          
    char* overflow = "It is too overwhelming...";
    char userinput[MAX_RESPONSE] = "";
    int putvalid = 0;
    int getvalid = 0;
    size_t input = MAX_ENTITY + MAX_RESPONSE + MAX_INTENT + 3;

    if (inc == 1)
    {
        snprintf(response, n, "Sorry, can you please type in a complete sentence? :(");
        return 0;
    }
    else if (inc == 2)
    {
        snprintf(response, n, "The sentence is still incomplete :(");
        return 0;
    }

    strcpy_s(intent, sizeof(intent), inv[0]);

    // Second token should be the connector (e.g., "is", "are")
    if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0)
    {
        // Start concatenating the remaining tokens to form the entity
        for (int i = 2; i < inc; i++)
        {
            if (i < inc - 1)
            {
                strcat_s(entity, sizeof(entity), inv[i]);
                strcat_s(entity, sizeof(entity), " ");
            }
            else
            {
                strcat_s(entity, sizeof(entity), inv[i]);
            }
        }
    }
    else
    {
        // If no "is" or "are" is found, handle it accordingly (could be an invalid format)
        snprintf(response, n, "Sorry, I don't understand the structure of the question.");
        return 0;
    }

    // Now check the knowledge base for a response
    getvalid = knowledge_get(intent, entity, reply, n);
    if (getvalid == KB_OK)
    {
        snprintf(response, n, "%s", reply);
    }
    else if (getvalid == KB_NOTFOUND)
    {
        // Ask user for more information if the knowledge is not found
        prompt_user(userinput, input, "I don't know. %s %s %s?\n%s: Tell me more!", intent, inv[1], entity, chatbot_botname());
        if (strcmp(userinput, "") == 0)
        {
            snprintf(response, n, "Type something :( Why don't you want to tell me...");
        }
        else
        {
            strcat_s(userinput, sizeof(userinput),"");
            putvalid = knowledge_put(intent, entity, userinput);

            if (putvalid == KB_OK)
            {
                snprintf(response, n, "Thanks for telling me!"); 
            }
            else if (putvalid == KB_NOMEM)
            {
                snprintf(response, n, "%s", overflow);
            }
            else if (putvalid == KB_INVALID)
            {
                snprintf(response, n, "Sorry, I cannot understand your explanation");
            }
        }
    }
    else if (getvalid == KB_INVALID)
    {
        snprintf(response, n, "Sorry, I cannot understand '%s'.", intent);
    }
    return 0;
}

int chatbot_is_reset(const char *intent) {
    return compare_token(intent, "reset") == 0;
}

int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
    knowledge_reset();
    snprintf(response, n, "Chatbot has been reset.");
    return 0;
}

int chatbot_is_save(const char *intent) {
    return compare_token(intent, "save") == 0;
}

int chatbot_do_save(int inc, char* inv[], char* response, int n)
{

    // Error messages
    char* overflow = "It is too overwhelming...";

    // Open file for writing (create or overwrite)
    FILE* openfile = fopen("ICT1503C_Project_Sample.ini", "w");

    // Write knowledge to file
    knowledge_sort();
    knowledge_write(openfile);
    fflush(openfile);
    snprintf(response, n, "My knowledge has been saved");
    fclose(openfile); 
    
    return 0;
}
 
