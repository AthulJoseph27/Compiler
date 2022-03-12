#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LABEL_SIZE 1024
#define MAXN 1024

struct LT
{
    int size;
    char *labels[MAX_LABEL_SIZE];
    int address[MAX_LABEL_SIZE];
};

struct Code
{
    int capacity;
    int size;
    char **code;
};

typedef struct LT LT;
typedef struct Code Code;

Code *allocate_code_space()
{
    FILE *file = fopen("prelinker.xsm", "r");
    int count = 0;

    Code *xsm_code = (Code *)malloc(sizeof(Code));

    char buffer[MAXN];

    while (fgets(buffer, MAXN, file))
    {
        count++;
    }

    fclose(file);

    int N = (count * 3) / 2;
    xsm_code->capacity = N;
    xsm_code->size = 0;
    xsm_code->code = (char **)malloc(sizeof(char *) * N);

    for (int i = 0; i < N; i++)
    {
        xsm_code->code[i] = (char *)malloc(sizeof(char) * MAXN);
    }

    return xsm_code;
}

void split(char *str, char *first, char *last)
{
    int n = strlen(str);

    int index = -1;
    for (int i = 0; i < n; i++)
    {
        if ((str[i] == ' ') || (str[i] == '\n'))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;

    strncpy(first, str, index);
    strcpy(last, str + index + 1);
}

bool is_jmp_statment(char *first)
{
    char *jmp = "JMP";

    if (strcmp(first, jmp) == 0)
        return true;

    jmp = "JZ";

    if (strcmp(first, jmp) == 0)
        return true;

    jmp = "JNZ";

    if (strcmp(first, jmp) == 0)
        return true;

    return false;
}

void add_label(LT *lt, char *fst, char *label)
{
    char first[MAXN], last[MAXN];

    char jmp[] = "JMP";

    if (strcmp(fst, jmp) != 0)
    {
        split(label, first, last);
        strcpy(label, last);
    }

    int n = strlen(label);
    label[--n] = '\0';

    for (int i = 0; i < lt->size; i++)
    {
        if (strcmp(lt->labels[i], label) == 0)
        {
            return;
        }
    }

    if (lt->size == MAX_LABEL_SIZE)
    {
        printf("Max label count exceeded!\n");
        exit(1);
    }

    lt->labels[lt->size++] = strdup(label);
}

bool is_label(LT *lt, char *label, int line)
{
    int n = strlen(label);

    if (label[n - 2] != ':')
    {
        return false;
    }

    label[n - 1] = '\0';
    label[n - 2] = '\0';

    for (int i = 0; i < lt->size; i++)
    {
        if (strcmp(lt->labels[i], label) == 0)
        {
            return true;
        }
    }

    lt->labels[lt->size] = (char *)malloc(sizeof(char) * (n + 5));
    lt->address[lt->size] = line + 2;
    strcpy(lt->labels[lt->size++], label);
    return true;
}

LT *get_labels(Code *xsm_code)
{
    FILE *file = fopen("prelinker.xsm", "r");
    LT *lt = (LT *)malloc(sizeof(LT));
    lt->size = 0;

    for (int i = 0; i < MAX_LABEL_SIZE; i++)
    {
        lt->labels[i] = NULL;
        lt->address[i] = 0;
    }

    char buffer[MAXN];
    memset(buffer, 0, sizeof(buffer));

    int line = 2048;

    for (int i = 0; i < 8; i++)
    {
        fgets(buffer, sizeof(buffer), file);
        strcpy(xsm_code->code[xsm_code->size++], buffer);
        memset(buffer, 0, sizeof(buffer));
        line++;
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (is_label(lt, buffer, line - 2))
        {
            memset(buffer, 0, sizeof(buffer));
            continue;
        }

        if (strlen(buffer) < 2)
            continue;

        line += 2;
        strcpy(xsm_code->code[xsm_code->size++], buffer);
        // printf("[%d] %s", line - 2, xsm_code->code[xsm_code->size - 1]);
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(file);

    return lt;
}

int get_address(LT *lt, char *label)
{
    int n = strlen(label);
    label[--n] = '\0';
    for (int i = 0; i < lt->size; i++)
    {
        if (strcmp(lt->labels[i], label) == 0)
            return lt->address[i];
    }

    printf("[WARNING]: UnRecognized label! Label - '%s'\n", label);
    return -1;
}

void replace_with_address(char *statement, LT *lt)
{

    char first[MAXN], second[MAXN], last[MAXN];
    memset(first, 0, sizeof(first));
    memset(last, 0, sizeof(last));
    split(statement, first, last);

    if (!is_jmp_statment(first))
        return;

    char jmp[] = "JMP";

    bool have_second = false;

    if (strcmp(first, jmp) != 0)
    {
        char tmp[MAXN];
        memset(tmp, 0, sizeof(tmp));
        have_second = true;
        memset(second, 0, sizeof(second));
        split(last, second, tmp);
        strcpy(last, tmp);
    }

    int address = get_address(lt, last);

    char new_statement[MAXN];

    if (have_second)
        sprintf(new_statement, "%s %s %d\n", first, second, address);
    else
        sprintf(new_statement, "%s %d\n", first, address);

    strcpy(statement, new_statement);
}

void replace_labels(Code *xsm_code, LT *lt)
{
    for (int i = 0; i < xsm_code->size; i++)
    {
        replace_with_address(xsm_code->code[i], lt);
    }
}

void output_code(Code *ocode)
{
    FILE *file = fopen("output.xsm", "w");
    for (int i = 0; i < ocode->size; i++)
    {
        fprintf(file, "%s", ocode->code[i]);
    }
    fclose(file);
}

int main()
{
    Code *xsm_code = allocate_code_space();

    LT *lt = get_labels(xsm_code);

    replace_labels(xsm_code, lt);

    output_code(xsm_code);

    return 0;
}