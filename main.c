#include <stdio.h>
#include <stdlib.h>


typedef struct node node;
typedef node* stack;


#define MAX_LIMIT 1024

char equations_infix[MAX_LIMIT][MAX_LIMIT];
char equations_prefix[MAX_LIMIT][MAX_LIMIT];
int valid_equations[MAX_LIMIT];

FILE *in, *out;
int file_size, math_error;

struct node {
    double value;
    node* next;
    int is_char;
};

/* functions prototypes */

int is_empty(stack);
stack create_stack();
void make_empty(stack);
void pop(stack);
node* top(stack);
void push(double, int, stack);
void check_valid_equations();
int is_open_parentheses(char);
int is_close_parentheses(char);
int is_match_open_close_parentheses(char, char);
int is_operator(char);
int is_operator(char);
char flip_parentheses(char);
int is_valid_consecutive_operations(char, char);
int valid(char*, int);
int precedence(char);
int is_digit(char);
stack convert_equations_from_infix_to_prefix(char*);
stack reverse(stack);
double apply_operation(double, double, char);
double evaluate_prefix(stack);
void print_invalid_equations();
char* print_stack(stack, int);
void display_menu();
void read_file(char*);
void print_all_to_file();
int driver();



/*
 * Checks if a stack is empty.
 *
 * Parameters:
 *  - s: The stack we want to preform the check on it.
 *
 * Returns:
 *  - integer value 1 or 0 indicates if the stack is empty or not, respectively.
 */


int is_empty(stack s){
    return s->next == NULL;
}

/*
 * Creates a stack.
 *
 * Returns:
 *  - A node pointer to the top of the new empty stack.
 */

stack create_stack(){
    stack new_stack = (stack)malloc(sizeof(node));

    if (new_stack == NULL)
        printf("Out of space!\n");

    new_stack->next = NULL;
    make_empty(new_stack);

    return new_stack;
}

/*
 * Makes a stack empty.
 *
 * Parameters:
 *  - s: The stack we want to make it empty.
 */

void make_empty(stack s){
    if (s == NULL)
        printf("Out of space!\n");

    else {
        while(!is_empty(s)){
            pop(s);
        }
    }
}

/*
 * Pops the element at the top of a stack.
 *
 * Parameters:
 *  - s: The stack we want to preform the pop operation on it.
 *
 */


void pop(stack s){

    node* top;

    if (is_empty(s)){
        printf("Empty stack!\n");
    }
    else {
        top = s->next;
        s->next = s->next->next;
        free(top);
    }

}

/*
 * Gets the top element at a stack.
 *
 * Parameters:
 *  - s: The stack we want to get the top element in it.
 *
 * Returns:
 *  - A node pointer to the top element of the stack.
 */

node* top(stack s){

    if (is_empty(s)){
        printf("Empty stack!\n");
        return 0;
    }

    return s->next;

}

/*
 * Pushs a new element to the stack.
 *
 * Parameters:
 *  - value: The value we want to push to the stack.
 *  - is_char: boolean flag indicates if the value represents a number or an operation.
 *  - s: The stack we want to preform the push operation on it.
 *
 */

void push(double value, int is_char, stack s){
    node* temp = (node*)malloc(sizeof(node));

    if (temp == NULL)
        printf("Out of space!\n");
    else {
        temp->value = value;
        temp->next = s->next;
        temp->is_char = is_char;
        s->next = temp;
    }

}

/*
 * Iterates over all the equations to find the valid ones.
 */

void check_valid_equations(){

    for (int i = 0 ; i < file_size ; i++){
        printf("Equation No.%d ->", i + 1);
        if (valid(equations_infix[i], 1)){
            valid_equations[i] = 1;
        }
        else valid_equations[i] = 0;
    }

}


int is_open_parentheses(char c){
    return (c == '(' || c == '[' || c == '{');
}

int is_close_parentheses(char c){
    return (c == ')' || c == ']' || c == '}');
}

int is_match_open_close_parentheses(char open, char close){
    return (open == '(' && close == ')') || (open == '[' && close == ']') || (open == '{' && close == '}');
}

int is_operator(char oper) {
    return oper == '+' || oper == '-' || oper == '*' || oper == '/' || oper == '^';
}

char flip_parentheses(char p){
    char open[3] = {'(', '[', '{'};
    char close[3] = {')', ']', '}'};


    for (int i = 0 ; i < 3 ; i++){
        if (p == open[i])
            return close[i];
        else if (p == close[i])
            return open[i];
    }

    return '\0';

}

int is_valid_consecutive_operations(char first, char second){
    if (second == '*' || second == '/' || second == '^')
        return 0;
    else return 1;
}

/*
 * Checks the validity of an equation and prints the error if not valid.
 *
 * Parameters:
 *  - equation_infix: A char pointer to the equation string.
 *  - print_to_console: boolean indicator to print the error to the console or not.
 *
 * Returns:
 *  - Boolean value indicates if the equation is valid or not.
 *
 */

int valid(char* equation_infix, int print_to_console){

    stack s = create_stack();

    for (int i = 0 ; equation_infix[i] != '\0'; i++){

        // opening parentheses
        if (is_open_parentheses(equation_infix[i])){
            if (i == 0 || is_operator(equation_infix[i - 1]) || is_open_parentheses(equation_infix[i - 1])){ // Open parentheses should be only at the beginning of the equation, after an operation or after another opening parentheses.
                push(equation_infix[i], 1, s);
            }
            else {
                if (print_to_console)
                    printf("invalid: No operator between \"%c%c\" at position %d.\n", equation_infix[i-1],equation_infix[i], i);
                return 0;
            }
        }

        // closing parentheses
        else if (is_close_parentheses(equation_infix[i])){
            // Finding a close parentheses without an opening.
            if (is_empty(s)){
                if (print_to_console)
                    printf("invalid: A closing parentheses \"%c\" is found at position %d without an opening.\n", equation_infix[i], i + 1);
                return 0;
            }

            char top_now = top(s)->value;
            pop(s);

            if (!is_match_open_close_parentheses(top_now, equation_infix[i])){
                if (print_to_console)
                    printf("invalid: Mismatched parentheses found closing parentheses \"%c\" at position %d with last open parentheses equal to \"%c\".\n", equation_infix[i], i + 1 , top_now);
                return 0;
            }

        }


        // operator

        else if (is_operator(equation_infix[i])) {
            if (i == 0){ // Should not be an operator at the beginning of equation unless it's + or - to indicates the sign.
                if (equation_infix[i] != '+' && equation_infix[i] != '-'){
                    if (print_to_console)
                        printf("invalid: Operator at the start of the equation.\n");
                    return 0;
                }
            }
            else if (equation_infix[i + 1] == '\0'){
                if (print_to_console)
                    printf("invalid: Operator at the end of the equation.\n");
                return 0;
            }

            else if (is_close_parentheses(equation_infix[i + 1])){ //operator followed by closing parentheses.
                if (print_to_console)
                    printf("invalid: Operator followed by closing parentheses at position %d.\n", i + 1);
                return 0;
            }

            else if (is_operator(equation_infix[i - 1]) && !is_valid_consecutive_operations(equation_infix[i - 1], equation_infix[i])){
                if (print_to_console)
                    printf("invalid: Two consecutive operators \"%c%c\" at position %d.\n", equation_infix[i - 1], equation_infix[i], i);
                return 0;
            }

        }


    }

    if (is_empty(s)){
        if (print_to_console)
            printf("valid.\n");
        return 1;
    }
    else {
        if (print_to_console)
            printf("invalid: Found opening parentheses without matching closing parentheses:");
        stack to_print = create_stack();
        while(!is_empty(s)){
            push(top(s)->value, 1, to_print);
            pop(s);
        }
        while(!is_empty(to_print)){
            if (print_to_console)
                printf(" \"%c\" ", top(to_print)->value);
            pop(to_print);
        }
        if (print_to_console)
            printf("\n");
        return 0;
    }


}


int precedence(char oper){
    if (oper == '^')
        return 3;
    if (oper == '*' || oper == '/'){
        return 2;
    }
    else if (oper == '+' || oper == '-'){
        return 1;
    }
    return 0;
}


int is_digit(char c){
    return (c >= '0' && c <= '9');
}

/*
 * Converts the equations form infix to prefix.
 *
 * Parameters:
 *  - infix: A char pointer to the equation string.
 *
 * Returns:
 *  - A pointer to the top of the stack containing the prefix representation of the eqaution.
 *
 */

stack convert_equations_from_infix_to_prefix(char* infix){

    stack reverse_stack = create_stack();
    int pointer_to_input = 0;
    int is_last_oper = 1;


    while(1){
        double number = -1;
        char oper;
        int move_pointer = 0;

        if (sscanf(infix + pointer_to_input, "%c%n", &oper, &move_pointer) == -1)
            break;

        if (is_digit(oper)){
            sscanf(infix + pointer_to_input, "%lf%n", &number, &move_pointer);
            push(number, 0, reverse_stack);
            is_last_oper = 0;
        }
        else if (oper == '-' && is_last_oper){
            push(-1 , 0, reverse_stack);
            push('*', 1, reverse_stack);
            is_last_oper = 1;
        }
        else if (oper == '+' && is_last_oper){
            push(1 , 0 , reverse_stack);
            push('*', 1 , reverse_stack);
            is_last_oper = 1;
        }
        else {
            if (is_open_parentheses(oper) || is_close_parentheses(oper))
                oper = flip_parentheses(oper);
            if (is_operator(oper))
                is_last_oper = 1;

            push(oper, 1, reverse_stack);
        }
        pointer_to_input+= move_pointer;
    }




    stack experssion = create_stack();
    stack chars_stack = create_stack();


    while(!is_empty(reverse_stack)){
        node* top_now = top(reverse_stack);
        double value = top_now->value;
        int is_char = top_now->is_char;
        pop(reverse_stack);



        if (is_char == 0){
            push(value, is_char, experssion);
        }

        else {
            char symbol = value;
            if (is_empty(chars_stack)){
                push(symbol, 1, chars_stack);
            }


            else if (is_open_parentheses(symbol)){
                push(symbol, 1, chars_stack);
            }

            else if (is_operator(symbol)){
                while(!is_empty(chars_stack) && precedence(top(chars_stack)->value) > precedence(symbol)){
                    node* symbol_at_top = top(chars_stack);
                    push(symbol_at_top->value, symbol_at_top->is_char, experssion);
                    pop(chars_stack);
                }
                push(symbol, 1, chars_stack);
            }

            else if (is_close_parentheses(symbol)){
                while(!is_empty(chars_stack) && top(chars_stack)->value != flip_parentheses(symbol)){
                    node* symbol_at_top = top(chars_stack);
                    push(symbol_at_top->value, symbol_at_top->is_char, experssion);
                    pop(chars_stack);
                }
                pop(chars_stack);
            }

        }

    }


    while(!is_empty(chars_stack)){
        node* top_now = top(chars_stack);
        push(top_now->value , top_now->is_char, experssion);
        pop(chars_stack);
    }

    return experssion;
}

/*
 * Reverses a stack.
 *
 * Parameters:
 *  - s: A node pointer to top of the stack we want to reverse.
 *
 * Returns:
 *  - A pointer to the top of the reversed stack.
 *
 */

stack reverse(stack s){
    stack out = create_stack();

    while(!is_empty(s)){
        node* top_s = top(s);
        push(top_s->value, top_s->is_char, out);
        pop(s);
    }

    return out;
}

/*
 * Applies the mathematical operations.
 *
 * Parameters:
 *  - first_number: double value represents the first number of the operation.
 *  - second_number: double value represents the second number of the operation.
 *  - operation: operation we want to apply.
 *
 * Returns:
 *  - A pointer to the top of the reversed stack.
 *
 */

double apply_operation(double first_number, double second_number, char operation){
    if (operation == '+')
        return first_number + second_number;
    else if (operation == '-')
        return first_number - second_number;
    else if (operation == '*')
        return first_number * second_number;
    else if (operation == '/'){
        if (second_number == 0){
            math_error = 1;
            return 0;
        }
        return first_number / second_number;
    }
    else if (operation == '^')
        return pow(first_number, second_number);
}

/*
 * Evaluates the prefix representation of the equation.
 *
 * Parameters:
 *  - prefix: A node pointer to top of the stack of the prefix representation of the equation.
 *
 * Returns:
 *  - double value the result of the equation.
 *
 */

double evaluate_prefix(stack prefix){


    stack reverse_prefix = reverse(prefix);
    stack calc = create_stack();


    while(!is_empty(reverse_prefix)){
        node* top_reverse_prefix = top(reverse_prefix);

        if (top_reverse_prefix->is_char == 1){
            double first_number = top(calc)->value;
            pop(calc);
            double second_number = top(calc)->value;
            pop(calc);

            push(apply_operation(first_number,second_number, top_reverse_prefix->value), 0, calc);

            if (math_error)
                return 0;
        }
        else {
            push(top_reverse_prefix->value, 0, calc);
        }

        pop(reverse_prefix);
    }

    return top(calc)->value;

}


/*
 * Iterates over the invalid equations.
 */

void print_invalid_equations(){
    for (int i = 0 ; i < file_size; i++){
        if (!valid_equations[i]){
            printf("%s\n", equations_infix[i]);
        }
    }
}

/*
 * Prints a stack.
 *
 * Parameters:
 *  - s: A node pointer to top of the stack we want to print.
 *  - print_to_console: boolean value to indicate if we want to print the stack to the console.
 *
 * Returns:
 *  - Char pointer to a string represents the stack contents.
 *
 */


char* print_stack(stack s, int print_to_console){

    char* output = (char*)malloc(MAX_LIMIT * sizeof(char));
    int pointer = 0;

    while(!is_empty(s)){
        node* top_now = top(s);
        if (top_now->is_char == 1){
            if (print_to_console)
                printf("%c ", (int)(top_now->value));
            pointer+= sprintf(output + pointer, "%c ", (int)(top_now->value));
        }
        else {
            if (print_to_console)
                printf("%.2f ", top_now->value);
            pointer+= sprintf(output + pointer, "%.2f ", top_now->value);
        }
        pop(s);
    }
    if (print_to_console)
        printf("\n");
    return output;
}

/*
 * Displays the menu of options.
 */

void display_menu(){
    printf("Please choose one of the operations:\n\n");
    printf("1. Read from the input file.\n");
    printf("2. Check the validity of equations.\n");
    printf("3. Convert valid equations from infix to prefix.\n");
    printf("4. Evaluate prefix.\n");
    printf("5. Print invalid.\n");
    printf("6. Print to output file all results.\n");
    printf("7. Exit.\n");
}

/*
 * Read all the content of a file.
 *
 * Parameters:
 *  - file_name: A char pointer to the string contains the file name we want to read from.
 *
 */

void read_file(char* file_name){

    in = fopen(file_name, "r");

    if (in == NULL){
        printf("Error opening the entered file. Please try again.\n");
        return;
    }

    file_size = 0;

    while(fscanf(in, "%s", equations_infix[file_size]) != -1){
        printf("%d: %s\n", file_size, equations_infix[file_size]);
        file_size++;
    }

    printf("Reading is finished successfully.\n");

}

/*
 * Print all of the results to the output.txt file with a tabular format.
 */


void print_all_to_file(){
    out = fopen("output.txt", "w");


    fprintf(out,"Equation No.i \t \t %-40s %-40s %-100s %-40s \n", "Equation infix", "Validity" ,"Equation prefix", "Result");
    fprintf(out,"\n");

    for (int i = 0 ; i < file_size ; i++){
        fprintf(out ,"Equation No.%d \t \t %-40s", i + 1 , equations_infix[i]);
        if (valid(equations_infix[i], 0)){
            fprintf(out, " %-40s", "valid");
            fprintf(out, " %-100s", print_stack(convert_equations_from_infix_to_prefix(equations_infix[i]), 0));

            double final_result = evaluate_prefix(convert_equations_from_infix_to_prefix(equations_infix[i]));

            if (math_error){
                fprintf(out," %-40s\n", "Math Error, can't divide by zero.");
                math_error = 0;
            }
            else {
                fprintf(out, " %-40.2f\n", final_result);
            }
        }
        else {
            fprintf(out, " %-40s %-100s %-40s\n", "invalid" ,"-", "-");
        }
    }

}


/*
 * Drive the interface of the program.
*/


int driver(){

    char input[MAX_LIMIT];

    scanf("%s", input);


    int option = atoi(input);


    if (option == 1){
        printf("please enter the file name with extension (ex. input.txt):\n");
        char file_name[MAX_LIMIT];
        scanf("%s", file_name);
        read_file(file_name);
    }
    else if (option == 7){
        return -1;
    }
    else if (file_size == 0){
        printf("There is no input data, please provide a valid input file with data in it.\n");
    }
    else if (option == 2){
        check_valid_equations();
    }
    else if (option == 3){
        for (int i = 0 ; i < file_size ; i++){
            if (valid_equations[i]){
                printf("Equation No.%d to prefix: ", i + 1);
                print_stack(convert_equations_from_infix_to_prefix(equations_infix[i]), 1);
            }
        }
    }
    else if (option == 4){
        for (int i = 0 ; i < file_size; i++){
            if (valid_equations[i]){
                printf("Equation No.%d -> ", i + 1);
                math_error = 0;
                double final_result = evaluate_prefix(convert_equations_from_infix_to_prefix(equations_infix[i]));
                if (math_error){
                    printf("Math Error, can't divide by zero.\n");
                    math_error = 0;
                }
                else {
                    printf("%.2f\n", final_result);
                }
            }
        }
    }
    else if (option == 5){
        print_invalid_equations();
    }
    else if (option == 6){
        print_all_to_file();
    }
    else {
        printf("Invalid option, please try again...\n");
    }

    return 0;

}





int main()
{

    do {
        display_menu();
    }while(driver() != -1);

    fclose(in);
    fclose(out);


}
