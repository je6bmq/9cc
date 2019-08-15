#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int INITIAL_VECTOR_CAPACITY = 2;

NodeReferenceVector *new_vec()
{
    NodeReferenceVector *vector = (NodeReferenceVector *)calloc(1, sizeof(NodeReferenceVector));
    vector->size = 0;
    vector->capacity = INITIAL_VECTOR_CAPACITY;
    vector->elements = (Node **)calloc(INITIAL_VECTOR_CAPACITY, sizeof(Node *));
    vector->element_size = sizeof(Node *);
    return vector;
}

void push(NodeReferenceVector *vector, Node *new_element)
{
    Node *element_copy = (Node *)calloc(1, sizeof(Node));
    memcpy(element_copy, new_element, sizeof(Node));

    if (vector->size < vector->capacity)
    {
    }
    else
    {
        Node **new_vec_location = realloc(vector->elements, sizeof(Node *) * (vector->capacity + 10));
        if (new_vec_location == NULL)
        {
            fprintf(stderr, "領域再確保に失敗しました\n");
            exit(1);
        }
        vector->capacity = vector->capacity + 10;
        vector->elements = new_vec_location;
    }
    vector->elements[vector->size] = new_element;
    vector->size++;
}

Node *get(NodeReferenceVector *vector, int index)
{
    if (vector->size <= index)
    {
        fprintf(stderr, "領域外参照です．(at %d th)\n", index);
        exit(1);
    }
    else
    {
        return vector->elements[index];
    }
}
