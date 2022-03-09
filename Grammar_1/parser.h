struct Node
{
    int oper;
    int type;
    int child_count;
    struct Node *children[3];
};

typedef struct Node Node;
#define OP_TYPE 0
#define VARIABLE_TYPE 1
#define NUM_TYPE 2