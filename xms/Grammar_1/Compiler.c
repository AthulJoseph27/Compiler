#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "parser.h"

/*
4096    : input
4097    : num
4098    : temp
4099    : lho
4100    : rho
4101    : digitSpace
4102    : digitSpacePos
4103 - 4194 : user variables
RAX -> R0
*/

#define REG_INPUT 4096
#define REG_NUM 4097
#define REG_TEMP 4098
#define REG_LHO 4099
#define REG_RHO 4100
#define REG_DIGITSPACE 4101
#define REG_DIGITSPACEPOS 4102
#define REG_VAR_BASE 4103

extern Node *create_node(int operand, int type);
extern Node *setValue(int id, int value);
extern int getValue(int id);
extern Node *free_node(Node *p);

FILE *file;

static int lbl;

void push(int val)
{
    fprintf(file, "MOV R0, %d\n", val);
    fprintf(file, "PUSH R0\n");
}

void pop()
{
    fprintf(file, "POP R0\n");
}

void print_int()
{
    fprintf(file, "POP R0\n");
    fprintf(file, "MOV [%d],R0\n", REG_NUM);
    fprintf(file, "MOV SP, 4195\n");
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

void read_int(int var)
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
    fprintf(file, "MOV [%d],R0\n", (REG_VAR_BASE + var));
}

void terminate()
{
    fprintf(file, "MOV R2, 10\n");
    fprintf(file, "PUSH R2\n");
    fprintf(file, "INT 10\n");
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
        else if (node->oper == WHILE)
        {
            fprintf(file, "JMP L%03d\n", lbl1 = lbl++);
            fprintf(file, "L%03d:\n", lbl1);
            execute(node->children[0]);
            execute(node->children[1]); // execute while condition
            fprintf(file, "POP R0\n");
            fprintf(file, "JNZ R0, L%03d\n", lbl1);
            fprintf(file, "JMP L%03d\n", lbl2 = lbl++);
            fprintf(file, "L%03d:\n", lbl2);
        }
        else if (node->oper == WRITE_VALUE)
        {
            execute(node->children[0]);
            print_int();
        }
        else if (node->oper == READ_VALUE)
        {
            read_int(node->children[0]->oper);
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
    file = fopen("prelinker.xsm", "w");

    const char *boiler_code =
        "0\n2056\n0\n0\n0\n0\n0\n0\n";

    fprintf(file, boiler_code);
    execute(node);
    terminate();
    fclose(file);
}

// yacc -d -v parser.y && lex lexer.l && gcc compiler.c lex.yy.c y.tab.c -o myscanner
