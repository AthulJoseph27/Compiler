#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "parser.h"

extern Node *create_node(int operand, int type);
extern Node *setValue(int id, int value);
extern int getValue(int id);
extern Node *free_node(Node *p);

static int lbl;

void set_lho_and_rho()
{
    printf("\tpop rax\n");
    printf("\tmov [rho], rax\n");
    printf("\tpop rax\n");
    printf("\tmov [lho], rax\n");
}

void push(int val)
{
    printf("\tpush %d\n", val);
}

void pop()
{
    printf("\tpop rax\n");
}

void print_int()
{
    pop();
    printf("\tcall _printRAX\n");
}

void read_int(int var)
{
    printf("\tcall _scanInt\n");
    printf("\tmov rax, [num]\n");
    printf("\tmov [%c], rax\n", (var + 'a'));
}

void terminate()
{
    printf("\tmov rax, 60\n");
    printf("\tmov rdi, 0\n");
    printf("\tsyscall");
}

int execute(Node *node)
{
    int lbl1, lbl2;
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
                printf("\tpop rax\n");
                printf("\tcmp rax, 1\n");
                printf("\tje\tL%03d\n", lbl1 = lbl++);
                printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                printf("L%03d:\n", lbl1);
                execute(node->children[1]);
                printf("L%03d:\n", lbl2);
                execute(node->children[2]);
            }
            else
            {
                printf("\tpop rax\n");
                printf("\tcmp rax, 1\n");
                printf("\tje\tL%03d\n", lbl1 = lbl++);
                printf("L%03d:\n", lbl1);
                execute(node->children[1]);
            }
        }
        else if (node->oper == WHILE)
        {
            printf("L%03d:\n", lbl1 = lbl++);
            execute(node->children[0]);
            printf("\tjz\tL%03d\n", lbl2 = lbl++);
            execute(node->children[1]);
            printf("\tjmp\tL%03d\n", lbl1);
            printf("L%03d:\n", lbl2);
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
            printf("\tmov [%c], rax\n", (node->children[0]->oper + 'a'));
        }
        else
        {
            execute(node->children[0]);
            execute(node->children[1]);
            int result = 0;
            if (node->oper == '+')
            {
                pop();
                printf("\tmov [temp], rax\n");
                pop();
                printf("\tadd rax, [temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == '-')
            {
                printf("\tsub\n");
            }
            else if (node->oper == '*')
            {
                pop();
                printf("\tmov [temp], rax\n");
                pop();
                printf("\tmov rbx, [temp]\n");
                printf("\tmul rbx\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == '<')
            {
                set_lho_and_rho();
                printf("\tcall _compareLT\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == '>')
            {
                set_lho_and_rho();
                printf("\tcall _compareGT\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == LE)
            {
                set_lho_and_rho();
                printf("\tcall _compareLE\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == GE)
            {
                set_lho_and_rho();
                printf("\tcall _compareGE\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == NE)
            {
                set_lho_and_rho();
                printf("\tcall _compareNE\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
            }
            else if (node->oper == EE)
            {
                set_lho_and_rho();
                printf("\tcall _compareEE\n");
                printf("\tmov rax,[temp]\n");
                printf("\tpush rax\n");
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
        printf("\tmov rax, [%c]\n", (node->oper + 'a'));
        printf("\tpush rax\n");
    }

    return 0;
}

void generate_code(Node *node)
{
    const char *boiler_code =
        "section .bss\n"
        "input resb 8\n"
        "num resb 8\n"
        "temp resb 8\n"
        "lho resb 8\n"
        "rho resb 8\n"
        "a resb 8\n"
        "b resb 8\n"
        "c resb 8\n"
        "d resb 8\n"
        "e resb 8\n"
        "f resb 8\n"
        "g resb 8\n"
        "h resb 8\n"
        "i resb 8\n"
        "j resb 8\n"
        "k resb 8\n"
        "l resb 8\n"
        "m resb 8\n"
        "n resb 8\n"
        "o resb 8\n"
        "p resb 8\n"
        "q resb 8\n"
        "r resb 8\n"
        "s resb 8\n"
        "t resb 8\n"
        "u resb 8\n"
        "v resb 8\n"
        "w resb 8\n"
        "x resb 8\n"
        "y resb 8\n"
        "z resb 8\n"
        "digitSpace resb 100\n"
        "digitSpacePos resb 8\n"

        "section .text\n"
        "global _start\n"

        "_scanInt:\n"
        "\tmov rax, 0\n"
        "\tmov rdi, 0\n"
        "\tmov rsi, input\n"
        "\tmov rdx, 1\n"
        "\tsyscall\n"

        "\tmov rax, [input]\n"
        "\tcmp rax, 10\n"
        "\tjne _parseInt\n"

        "\tret\n"

        "_parseInt:\n"
        "\tmov rax, [num]\n"
        "\tmov rdx, 0\n"
        "\tmov rbx, 10\n"
        "\tmul rbx\n"

        "\tmov rcx, [input]\n"
        "\tsub rcx, 30h\n"
        "\tadd rax, rcx\n"

        "\tmov [num], rax\n"
        "\tjmp _scanInt\n"

        "_exit:\n"
        "\tret\n"

        "_printRAX:\n"
        "\tmov rcx, digitSpace\n"
        "\tmov rbx, 10\n"
        "\tmov [rcx], rbx\n"
        "\tinc rcx\n"
        "\tmov [digitSpacePos], rcx\n"

        "_printRAXLoop:"
        "\tmov rdx, 0\n"
        "\tmov rbx, 10\n"
        "\tdiv rbx\n"
        "\tpush rax\n"
        "\tadd rdx, 48\n"

        "\tmov rcx, [digitSpacePos]\n"
        "\tmov [rcx], dl\n"
        "\tinc rcx\n"
        "\tmov [digitSpacePos], rcx\n"
        "\t\n"
        "\tpop rax\n"
        "\tcmp rax, 0\n"
        "\tjne _printRAXLoop\n"

        "_printRAXLoop2:\n"
        "\tmov rcx, [digitSpacePos]\n"

        "\tmov rax, 1\n"
        "\tmov rdi, 1\n"
        "\tmov rsi, rcx\n"
        "\tmov rdx, 1\n"
        "\tsyscall\n"

        "\tmov rcx, [digitSpacePos]\n"
        "\tdec rcx\n"
        "\tmov [digitSpacePos], rcx\n"

        "\tcmp rcx, digitSpace\n"
        "\tjge _printRAXLoop2\n"

        "\tret\n"

        "_compareLT:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tjl _push_one\n"
        "\tjmp _push_zero\n"

        "_compareGT:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tjg _push_one\n"
        "\tjmp _push_zero\n"

        "_compareLE:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tjle _push_one\n"
        "\tjmp _push_zero\n"

        "_compareGE:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tjge _push_one\n"
        "\tjmp _push_zero\n"

        "_compareEE:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tje _push_one\n"
        "\tjmp _push_zero\n"

        "_compareNE:\n"
        "\tmov rax, [lho]\n"
        "\tcmp rax, [rho]\n"
        "\tjne _push_one\n"
        "\tjmp _push_zero\n"

        "_push_zero:\n"
        "\tmov rax, 0\n"
        "\tmov [temp], rax\n"
        "\tret\n"

        "_push_one:\n"
        "\tmov rax, 1\n"
        "\tmov [temp], rax\n"
        "\tret\n"

        "_start:\n";

    printf(boiler_code);
    execute(node);
    terminate();
}

// yacc -d -v parser.y && lex lexer.l && gcc compiler.c lex.yy.c y.tab.c -o myscanner
