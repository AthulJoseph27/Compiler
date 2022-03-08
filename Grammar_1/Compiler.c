#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "myscanner.h"

extern Node *create_node(int operand, int type);
extern Node *setValue(int id, int value);
extern int getValue(int id);
extern Node *free_node(Node *p);

static int lbl;

Node *execute(Node *node)
{
    if (node == NULL)
        return node;

    if (node->type == OP_TYPE)
    {
        if (node->oper == ';')
        {
            for (int i = 0; i < node->child_count; i++)
            {
                execute(node->children[i]);
            }
            return NULL;
        }
        else if (node->oper == IF)
        {
            if (node->children[0]->oper)
            {
                execute(node->children[1]);
            }
            else
            {
                execute(node->children[2]);
            }
        }
        else if (node->oper == WHILE)
        {
            // execute(node->children[1]);
            ;
        }
        else if (node->oper == WRITE_VALUE)
        {

            if (node->children[0]->type == VARIABLE_TYPE)
            {
                printf("%d\n", getValue(node->children[0]->oper));
                return NULL;
            }
            Node *result = execute(node->children[0]);
            printf("%d\n", result->oper);
            return NULL;
        }
        else if (node->oper == READ_VALUE)
        {
            scanf("%d", &node->oper);
        }
        else if (node->oper == '=')
        {
            Node *result = execute(node->children[1]);
            setValue(node->children[0]->oper, result->oper);
            return NULL;
        }
        return NULL;
    }
    else if (node->type == NUM_TYPE)
    {
        return node;
    }

    return NULL;
}

// yacc -d -v myscanner.y && lex myscanner.l && gcc compiler.c lex.yy.c y.tab.c -o myscanner