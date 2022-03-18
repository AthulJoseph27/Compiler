#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "parser.h"

/*
4096    : input
4097    : num
4098    : temp
4099    : argc
4100    : return val
4101 - 4194 : user variables
RAX -> R0
*/

#define REG_INPUT 4096
#define REG_NUM 4097
#define REG_TEMP 4098
#define REG_RESULT 4100
#define REG_ARGC 4101
#define REG_VAR_BASE 4102

extern Node *create_node(int operand, int type);
extern Node *setValue(int id, int value);
extern Node *free_node(Node *p);
extern int getValue(int id);
extern int get_variable_count();

FILE *file;

static int lbl;

const char FUN_LABEL[] = "FUN";
const char RETURN_LABEL[] = "RETURN_LABEL";

int variable_count = 0;

void push(int val)
{
    fprintf(file, "MOV R0, %d\n", val);
    fprintf(file, "PUSH R0\n");
}

void pop()
{
    fprintf(file, "POP R0\n");
}

void terminate()
{
    fprintf(file, "MOV R2, 10\n");
    fprintf(file, "PUSH R2\n");
    fprintf(file, "INT 10\n");
}

void call_function()
{
    // argument will be in top of the stack
    // push all variables to stack
    // copy argument to be passed to argc
    // call func
    // pop back all variables back
    fprintf(file, "POP R0\n");
    fprintf(file, "MOV [%d], R0\n", REG_ARGC);
    for (int i = 0; i < variable_count; i++)
    {
        fprintf(file, "MOV R0, [%d]\n", (REG_VAR_BASE + i));
        fprintf(file, "PUSH R0\n");
    }
    fprintf(file, "CALL FUN\n");
    for (int i = variable_count - 1; i >= 0; i--)
    {
        fprintf(file, "POP R0\n");
        fprintf(file, "MOV [%d], R0\n", (REG_VAR_BASE + i));
    }
    fprintf(file, "MOV R0, [%d]\n", REG_RESULT);
    fprintf(file, "PUSH R0\n");
}

void read_argc()
{
    fprintf(file, "MOV SP, 4195\n");
    fprintf(file, "MOV R1, \"ScanInt\"\n");
    fprintf(file, "PUSH R1\n");
    fprintf(file, "MOV R1, -2\n");
    fprintf(file, "PUSH R1\n");
    fprintf(file, "PUSH R0\n");
    fprintf(file, "PUSH R1\n");
    fprintf(file, "PUSH R1\n");
    fprintf(file, "CALL 0\n");
    fprintf(file, "POP R0\n");
    fprintf(file, "POP R1\n");
    fprintf(file, "POP R1\n");
    fprintf(file, "POP R1\n");
    fprintf(file, "POP R1\n");
    fprintf(file, "MOV R0,[%d]\n", REG_INPUT);
    fprintf(file, "PUSH R0\n");
    call_function();
    fprintf(file, "JMP _PRINT\n");
}

void write_argc()
{
    fprintf(file, "MOV R0, [%d]\n", REG_RESULT);
    fprintf(file, "MOV [%d],R0\n", REG_NUM);
    fprintf(file, "MOV R1, \"PrintInt\"\n");
    fprintf(file, "PUSH R1\n");
    fprintf(file, "MOV R2, -2\n");
    fprintf(file, "PUSH R2\n");
    fprintf(file, "MOV R2, [%d]\n", REG_NUM);
    fprintf(file, "PUSH R2\n");
    fprintf(file, "PUSH R2\n");
    fprintf(file, "PUSH R2\n");
    fprintf(file, "CALL 0\n");
}

int execute(Node *node)
{
    int lbl1, lbl2, lbl3;
    if (node == NULL)
        return 0;

    if (node->type == OP_TYPE)
    {
        if (node->oper == ';')
        {
            for (int i = 0; i < node->child_count; i++)
            {
                execute(node->children[i]);
            }
        }
        else if (node->oper == IF)
        {
            execute(node->children[0]);
            if (node->child_count > 2)
            {
                // if else
                fprintf(file, "POP R0\n");
                fprintf(file, "JNZ R0, L%03d\n", lbl1 = lbl++);
                fprintf(file, "JMP L%03d\n", lbl2 = lbl++);
                lbl3 = lbl++;
                fprintf(file, "L%03d:\n", lbl1);
                execute(node->children[1]);
                fprintf(file, "JMP L%03d\n", lbl3);
                fprintf(file, "L%03d:\n", lbl2);
                execute(node->children[2]);
                fprintf(file, "JMP L%03d\n", lbl3);
                fprintf(file, "L%03d:\n", lbl3);
            }
            else
            {
                fprintf(file, "POP R0\n");
                fprintf(file, "JNZ R0, L%03d\n", lbl1 = lbl++);
                lbl2 = lbl++;
                fprintf(file, "JMP L%03d\n", lbl2);
                fprintf(file, "L%03d:\n", lbl1);
                execute(node->children[1]);
                fprintf(file, "JMP L%03d\n", lbl2);
                fprintf(file, "L%03d:\n", lbl2);
            }
        }
        else if (node->oper == FUN)
        {

            execute(node->children[0]);
            // R0 has the argument
            call_function();
        }
        else if (node->oper == RETURN)
        {
            execute(node->children[0]);
            fprintf(file, "POP R0\n");
            fprintf(file, "MOV [%d], R0\n", REG_RESULT);
            fprintf(file, "JMP %s\n", RETURN_LABEL);
        }
        else if (node->oper == '=')
        {
            execute(node->children[1]);
            pop();
            fprintf(file, "MOV [%d], R0\n", (node->children[0]->oper + REG_VAR_BASE));
        }
        else
        {
            execute(node->children[0]);
            execute(node->children[1]);
            int result = 0;
            if (node->oper == '+')
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "ADD R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == '-')
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "SUB R0, R1\n", REG_TEMP);
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == '*')
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "MUL R0, R1\n", REG_TEMP);
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == '<')
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "LT R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == '>')
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "GT R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == LE)
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "LE R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == GE)
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "GE R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == NE)
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "NE R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
            else if (node->oper == EE)
            {
                pop();
                fprintf(file, "MOV [%d], R0\n", REG_TEMP);
                pop();
                fprintf(file, "MOV R1, [%d]\n", REG_TEMP);
                fprintf(file, "EQ R0, R1\n");
                fprintf(file, "PUSH R0\n");
            }
        }
    }
    else if (node->type == NUM_TYPE)
    {
        push(node->oper);
    }
    else if (node->type == VARIABLE_TYPE)
    {
        // push value at VAR
        fprintf(file, "MOV R0, [%d]\n", (node->oper + REG_VAR_BASE));
        fprintf(file, "PUSH R0\n");
    }

    return 0;
}

void generate_code(Node *node)
{
    variable_count = get_variable_count();
    file = fopen("prelinker.xsm", "w");

    const char *boiler_code =
        "0\n2056\n0\n0\n0\n0\n0\n0\n";

    fprintf(file, boiler_code);
    read_argc();
    fprintf(file, "FUN:\n");
    fprintf(file, "MOV R0, [%d]\n", REG_ARGC);
    fprintf(file, "MOV [%d], R0\n", REG_VAR_BASE);
    execute(node);
    fprintf(file, "_PRINT:\n");
    write_argc();
    terminate();
    fprintf(file, "%s:\n", RETURN_LABEL);
    fprintf(file, "RET\n");
    fclose(file);
}

// yacc -d -v parser.y && lex lexer.l && gcc compiler.c lex.yy.c y.tab.c -o myscanner
