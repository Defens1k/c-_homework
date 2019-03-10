#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MIN_ARRAY_CAPACITY 16
////////////////////////////////////////////////////////////////
/////////////// STRUCT DUNAMIC STRING AREA//////////////////////
struct dynamic_string {
    char * data;
    size_t current_lenght;
    size_t capacity;
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

dynamic_string * copy_dynamic_string(const dynamic_string * old_string) {
    dynamic_string * new_array = malloc(sizeof(dynamic_string));
    if (new_array == NULL) {
        return NULL;
    }

    new_array->current_lenght = strlen(old_string->data);
    new_array->capacity = new_array->current_lenght;
    new_array->data = calloc(new_array->current_lenght + 1, sizeof(char));
    if (new_array->data == NULL) {
        free(new_array);
        return NULL;
    }
    memcpy(new_array->data, old_string->data, new_array->current_lenght + 1);
    return new_array;
}
void delete_dynamic_string(dynamic_string * array) {
    free(array->data);
    free(array);
}

bool add_to_dynamic_string(dynamic_string * array,const char symbol) {
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
        return add_to_dynamic_string(array, symbol); // when we resize, we can add symbol 
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

bool add_to_strings_dynamic_array(strings_dynamic_array * array, dynamic_string * ptr) {
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
        return add_to_strings_dynamic_array(array, ptr);  // when we resize a array we can add ptr to array
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////

struct strings_union {
    strings_dynamic_array * data;  // array of strings
    char left_operand;
    char right_operand;
    int nesting;
    struct strings_union * left;
    struct strings_union * right;
};
typedef struct strings_union strings_union;

void print_union(strings_union *);

strings_union * first_union(strings_union * ptr) {
    if (ptr == NULL) {
        return NULL;
    }
    while (ptr->left != NULL) {
        ptr = ptr->left;
    }
    return ptr;
}

dynamic_string * read_string() {
    char c = '\0';
    dynamic_string * new_string = new_dynamic_string();
    while ((c = getchar()) != '\"') {
        add_to_dynamic_string(new_string, c);
    }
    return new_string;
}

void delete_unions(strings_union * ptr) {
    if (ptr == NULL) {
        return;
    }
    if (ptr->right != NULL) {
        delete_unions(ptr->right);
    }
    if (ptr->data != NULL) {
        delete_strings_dynamic_array(ptr->data);
    }
    free(ptr);
}

strings_dynamic_array * read_union() {
    char c;
    strings_dynamic_array * new_array = new_strings_dynamic_array();
    while(1) {
        c = getchar();
        if (c == ' ') {
            delete_strings_dynamic_array(new_array);
            return NULL;
        }
        if (c == ']') {
            return new_array;
        }
        if (c == '\"') {
            add_to_strings_dynamic_array(new_array, read_string());
        }
    }
}

strings_union * new_union(int nesting) {
    strings_union * new_union = malloc(sizeof(strings_union));
    new_union->nesting = nesting;
    new_union->left = NULL;
    new_union->right = NULL;
    new_union->left_operand = '\0';
    new_union->right_operand = '\0';
    new_union->data = read_union();
    if (new_union->data == NULL) {
        delete_unions(new_union);
        return NULL;
    }
    return new_union;
}

void synchronize(strings_union * left, strings_union * right) {
    if (left != NULL) {
         if (right != NULL) {
            right->left = left;
            left->right = right;
            right->left_operand = left->right_operand;
        }
        else {
            left->right = NULL;   
        }
    }   
    else {
        if (right != NULL) {
            right->left = NULL;
        }
        else {
            return;
        }
    } 
}



strings_union * read_input() {
    char c = ' ';
    strings_union * current_ptr = NULL;
    strings_union * new_ptr = NULL;
    int current_nesting = 0;
    while ((c = getchar()) != '\n' && c != EOF && c != '\0') {
        if (c == ' ') {
            delete_unions(first_union(current_ptr));
            return NULL;
        }
        if (c == '(') {
            current_nesting += 1;
            continue;
        }
        if (c == ')') {
            current_nesting -= 1;
            continue;
        }

        if (c == '[') {
            new_ptr = new_union(current_nesting);
            if (new_ptr == NULL) {
                delete_unions(first_union(current_ptr));
                return NULL;
            }
            synchronize(current_ptr, new_ptr);
            current_ptr = new_ptr;
            continue;
        }
        if (c == 'U') {
            current_ptr->right_operand = 'U';
            continue;
        }
        if (c == '^') {
            current_ptr->right_operand = '^';
            continue;
        }
        if (c == '\\') {
            current_ptr->right_operand = '\\';
            continue;
        }
    }
    if (current_nesting != 0) {
        delete_unions(first_union(current_ptr));
        return NULL;
    }
    return first_union(current_ptr);
}

void print_union(strings_union * ptr) {
    if (ptr == NULL) {
        return;
    }
    printf("[");
    size_t i = 0;
    for (i = 0; i < ptr->data->current_lenght; i++)
    {
        if (i != 0) {
            printf(",");
        }
        printf("\"%s\"", ptr->data->data[i]->data);
    }
    if (i == 0) {
        printf("]\n");
    }
    else {
        printf("]\n");
    }
}

int max_nesting(strings_union * first) {
    int max_nesting = 0;
    strings_union * iter = first;
    while (iter != NULL) {
        if (iter->nesting > max_nesting) {
            max_nesting = iter->nesting;
        }
        iter = iter->right;
    }
    return max_nesting;
}

bool equal(dynamic_string * left, dynamic_string * right) {
    size_t i = 0;
    while(1) {
        if (left->data[i] == right->data[i]) {
            if (left->data[i] == '\0') {
                return true;
            }
            else {
                i++;
                continue;
            }
        }
        return false;
    }
}

strings_dynamic_array * sum_unions(strings_dynamic_array * left, strings_dynamic_array * right) {
    strings_dynamic_array * result = new_strings_dynamic_array();
    for (size_t i = 0; i < left->current_lenght; i++) {
            add_to_strings_dynamic_array(result, copy_dynamic_string(left->data[i]));
    }
    for (size_t i = 0; i < right->current_lenght; i++) {
            add_to_strings_dynamic_array(result, copy_dynamic_string(right->data[i]));
    }
    return result;
}

strings_dynamic_array * decriment_unions(strings_dynamic_array * left, strings_dynamic_array * right) {
    strings_dynamic_array * result = new_strings_dynamic_array();
    for (size_t i = 0; i < left->current_lenght; i++) {
        bool are_equal = false;
        for (size_t j = 0; j < right->current_lenght; j++) {
            if (equal(left->data[i], right->data[j])) {
                are_equal = true;
                break;
            }
        }
        if (!are_equal) {
            add_to_strings_dynamic_array(result, copy_dynamic_string(left->data[i]));
        }
    }
    return result;
}

strings_dynamic_array * cross_unions(strings_dynamic_array * left, strings_dynamic_array * right) {
    strings_dynamic_array * result = new_strings_dynamic_array();
    for (size_t i = 0; i < left->current_lenght; i++) {
        for (size_t j = 0; j < right->current_lenght; j++) {
            if (equal(left->data[i], right->data[j])) {
                add_to_strings_dynamic_array(result, copy_dynamic_string(left->data[i]));
            }
        }
    }
    return result;
}

strings_dynamic_array * operation(strings_union * left, strings_union * right) {
    if (left->right_operand == '^') {
        return cross_unions(left->data, right->data);
    }
    if (left->right_operand == '\\') {
        return decriment_unions(left->data, right->data);
    }
    if (left->right_operand == 'U') {
        strings_dynamic_array * dec = decriment_unions(right->data, left->data);
        strings_dynamic_array * result = sum_unions(dec, left->data);
        delete_strings_dynamic_array(dec);
        return result;
    }
    printf("error");
    return NULL;
}

void all_operations(strings_union * first, int nesting, char operand) {
    strings_union * ptr = first;
    strings_union * swap = NULL;
    while (ptr->right != NULL) {
        if (ptr->nesting == nesting && ptr->right->nesting == nesting && ptr->right_operand == operand) {
            strings_dynamic_array * result = operation(ptr, ptr->right);
            delete_strings_dynamic_array(ptr->data);
            delete_strings_dynamic_array(ptr->right->data);
            ptr->data = result;
            swap = ptr->right->right;                  ///// write result in this union and delete right union
            ptr->right_operand = ptr->right->right_operand;
            free(ptr->right);
            ptr->right = swap;
        } else {
            ptr = ptr->right;
        }
    }
}

void decriment_nesting(strings_union * first, int nesting) {
    strings_union * ptr = first;
    while (ptr != NULL) {
        if (ptr->nesting == nesting) {
            ptr->nesting--;
        }
        ptr = ptr->right;
    }
}

void solve(strings_union * first) {
    if (first->left == NULL && first->right == NULL) {
        return;
    }
    int max = max_nesting(first);
    int i = max;
    for (i = max + 1; i >= 0; i--) {
        all_operations(first, i, '^');
        all_operations(first, i, 'U');
        all_operations(first, i, '\\');
        decriment_nesting(first, i);
    }
}

int compare(const void * a_a, const void * b_b) {
    const dynamic_string ** a = (const dynamic_string **)a_a;
    const dynamic_string ** b = (const dynamic_string **)b_b;
    int i = 0;
    while ((*a)->current_lenght >= i && (*b)->current_lenght >= i) {
        if ((*a)->data[i] != (*b)->data[i]) {
            return (*a)->data[i] > (*b)->data[i];
        }
        i++;
    }
    return (*a)->current_lenght < (*b)->current_lenght;
}

void sort(strings_union * a) {
    strings_dynamic_array * first = a->data;
    qsort(first->data, first->current_lenght, sizeof(dynamic_string(*)), (int(*)(const void * a_a, const void * b_b))compare);
}

int main() {
    strings_union * first = read_input();
    if (first == NULL) {
        printf("[error]\n");
        return 0;
    }
    solve(first);
    sort(first);
    print_union(first);
    delete_strings_dynamic_array(first->data);
    free(first);
}
