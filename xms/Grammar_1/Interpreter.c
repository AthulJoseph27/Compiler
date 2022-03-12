#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "parser.h"

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
            Node *result = execute(node->children[0]);
            if (result->oper)
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
            while (1)
            {
                execute(node->children[0]);
                Node *result = execute(node->children[1]);
                if (!(result->oper))
                    break;
            }
            return NULL;
        }
        else if (node->oper == WRITE_VALUE)
        {
            Node *result = execute(node->children[0]);
            printf("%d\n", result->oper);
            return NULL;
        }
        else if (node->oper == READ_VALUE)
        {
            int inp;
            scanf("%d", &inp);
            setValue(node->children[0]->oper, inp);
            return NULL;
        }
        else if (node->oper == '=')
        {
            Node *result = execute(node->children[1]);
            setValue(node->children[0]->oper, result->oper);
            return NULL;
        }
        else
        {
            Node *left = execute(node->children[0]);
            Node *right = execute(node->children[1]);
            int result = 0;
            if (node->oper == '+')
            {
                result = left->oper + right->oper;
            }
            else if (node->oper == '-')
            {
                result = left->oper - right->oper;
            }
            else if (node->oper == '*')
            {
                result = left->oper * right->oper;
            }
            else if (node->oper == '<')
            {
                result = left->oper < right->oper;
            }
            else if (node->oper == '>')
            {
                result = left->oper > right->oper;
            }
            else if (node->oper == LE)
            {
                result = left->oper <= right->oper;
            }
            else if (node->oper == GE)
            {
                result = left->oper >= right->oper;
            }
            else if (node->oper == NE)
            {
                result = left->oper != right->oper;
            }
            else if (node->oper == EE)
            {
                result = left->oper == right->oper;
            }

            return create_node(result, NUM_TYPE);
        }

        return NULL;
    }
    else if (node->type == NUM_TYPE)
    {
        return node;
    }
    else if (node->type == VARIABLE_TYPE)
    {
        return create_node(getValue(node->oper), NUM_TYPE);
    }

    return NULL;
}

// yacc -d -v myscanner.y && lex myscanner.l && gcc compiler.c lex.yy.c y.tab.c -o myscanner