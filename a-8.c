#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MIN_ARRAY_CAPACITY 16

////////////////////////////////////////////////////////////////
/////////////// STRUCT DUNAMIC STRING AREA//////////////////////
struct dynamic_string {  // struct of string 
    char * data;    // ptr to stroing
    size_t current_lenght;     // current lenght of string
    size_t capacity;       // data size - 1
};
typedef struct dynamic_string dynamic_string;

dynamic_string * new_dynamic_string() {
    dynamic_string * new_array = malloc(sizeof(dynamic_string));
    if (new_array == NULL) {
        return NULL;
    }

    new_array->current_lenght = 0;
    new_array->capacity = MIN_ARRAY_CAPACITY - 1; //reserve one byte to '\0'
    new_array->data = calloc(MIN_ARRAY_CAPACITY, sizeof(char));

    if (new_array->data == NULL) {
        free(new_array);
        return NULL;
    }
    return new_array;
}

void delete_dynamic_string(dynamic_string * array) {
    free(array->data);
    free(array);
}

bool push_back(dynamic_string * array,const char symbol) { 
    if (array->current_lenght < array->capacity) {
        array->data[array->current_lenght] = symbol;
        array->data[array->current_lenght + 1] = '\0';
        array->current_lenght++;
        return true;

    } else {                              // if array is ful - we must resize
        char * new_data = calloc((array->capacity + 1) * 2, sizeof(char));
        if (new_data == NULL) {
            return false;
        }
        memcpy(new_data, array->data, (array->capacity) * sizeof(char));
        new_data[array->current_lenght + 1] = '\0';

        free(array->data);
        array->data = new_data;
        array->capacity = (array->capacity + 1) * 2 - 1;
        return push_back(array, symbol); // when we resize, we can add symbol 
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////// STRUCT STRINGS DYNAMIC ARRAY  AREA /////////////////////////

struct strings_dynamic_array {
    dynamic_string ** data;
    size_t current_lenght;
    size_t capacity;
};
typedef struct strings_dynamic_array strings_dynamic_array;

strings_dynamic_array * new_strings_dynamic_array() {
    strings_dynamic_array * new_array = malloc(sizeof(strings_dynamic_array));
    if (new_array == NULL) {
        return NULL;
    }

    new_array->current_lenght = 0;
    new_array->capacity = MIN_ARRAY_CAPACITY;
    new_array->data = calloc(MIN_ARRAY_CAPACITY, sizeof(dynamic_string *));
    
    if (new_array->data == NULL) {
        free(new_array);
        return NULL;
    }
    return new_array;
}

void delete_strings_dynamic_array(strings_dynamic_array * array) {
    size_t i = 0;
    for (i = 0; i < array->current_lenght; i++) {
        delete_dynamic_string(array->data[i]);
    }
    free(array->data);
    free(array);
}

bool push_back(strings_dynamic_array * array, dynamic_string * ptr) {
    if (array->current_lenght < array->capacity) {
        array->data[array->current_lenght] = ptr;
        array->current_lenght++;
        return true;
    } else {                         // if array is full - we must resize
        dynamic_string ** new_data = calloc(array->capacity * 2, sizeof(dynamic_string *));
        if (new_data == NULL) {
            return false;
        }
        memcpy(new_data, array->data, (array->capacity) * sizeof(dynamic_string *));
        free(array->data);
        array->data = new_data;
        array->capacity *= 2;
        return push_back(array, ptr);  // when we resize a array we can add ptr to array
    }
    return true;
}
////////////////////////////////////////////////////////////////////
///////////// OTHER FUNCTIONS  /////////////////////////////////////



strings_dynamic_array * read_strings() {
    char c;
    dynamic_string * new_string = new_dynamic_string();
    strings_dynamic_array * new_array = new_strings_dynamic_array();
    while((c = getchar()) != EOF) {
        if (push_back(new_string, c) == false) {
            delete_strings_dynamic_array(new_array);
            delete_dynamic_string(new_string);
            return NULL;
        }
        
        if (c == '\n') {        //  end of string, add in array
            if (push_back(new_array, new_string) == false) {
                delete_strings_dynamic_array(new_array);
                delete_dynamic_string(new_string);
                return NULL;
            }
            new_string = new_dynamic_string();
        }
    }
    if (push_back(new_array, new_string) == false) {
        delete_strings_dynamic_array(new_array);
        delete_dynamic_string(new_string);
        return NULL;
    }
    return new_array;
}

dynamic_string * remove_spases_from_string(const char * old_string) {
    dynamic_string * new_string = new_dynamic_string();
    if (new_string == NULL) {
        return NULL;
    }
    size_t i = 0;
    char prev = '\0';
    while (old_string[i] != '\0') {
        if (old_string[i] != ' ' || prev != ' ') {   // add only one space in new string
                       if (push_back(new_string, old_string[i]) == false) {
                delete_dynamic_string(new_string);
                return NULL;
            }
        }
        prev = old_string[i];
        i++;
    }
    return new_string;
}

strings_dynamic_array * remove_spases(const strings_dynamic_array * input_array) {
    size_t i = 0;
    dynamic_string * new_string;
    strings_dynamic_array * new_array = new_strings_dynamic_array();
    if (new_array == NULL) {
        return NULL;
    }
    for (i = 0; i < input_array->current_lenght; i++) {
        new_string = remove_spases_from_string(input_array->data[i]->data);
        if (new_string == NULL) {
            delete_strings_dynamic_array(new_array);
            return NULL;
        }
        if (push_back(new_array, new_string) == false) {
            delete_strings_dynamic_array(new_array);
            return NULL;
        }
    }
    return new_array;
}

void print_array(const strings_dynamic_array * output_array) {
    size_t i = 0;
    for (i = 0; i < output_array->current_lenght; i++) {
        printf("%s", output_array->data[i]->data);
    }
}

int main() {
    strings_dynamic_array * input_array = read_strings();
    if (input_array == NULL) {
        fprintf(stderr, "error\n");
        return 0;
    }
    strings_dynamic_array * output_array = remove_spases(input_array);
    if (output_array == NULL) {
        fprintf(stderr, "error\n");
        delete_strings_dynamic_array(input_array);
        return 0;
    }
    print_array(output_array);
    delete_strings_dynamic_array(output_array);
    delete_strings_dynamic_array(input_array);
}
