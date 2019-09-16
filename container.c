#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int INITIAL_VECTOR_CAPACITY = 2;

NodeReferenceVector *new_node_vec()
{
    NodeReferenceVector *vector = (NodeReferenceVector *)calloc(1, sizeof(NodeReferenceVector));
    vector->size = 0;
    vector->capacity = INITIAL_VECTOR_CAPACITY;
    vector->elements = (Node **)calloc(INITIAL_VECTOR_CAPACITY, sizeof(Node *));
    vector->element_size = sizeof(Node *);
    return vector;
}

void push_node(NodeReferenceVector *vector, Node *new_element)
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

Node *get_node(NodeReferenceVector *vector, int index)
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


TemporaryStringVector *new_string_vec()
{
    TemporaryStringVector *vector = (TemporaryStringVector *)calloc(1, sizeof(TemporaryStringVector));
    vector->size = 0;
    vector->capacity = INITIAL_VECTOR_CAPACITY;
    vector->elements = (String **)calloc(INITIAL_VECTOR_CAPACITY, sizeof(String *));
    // vector->element_size = sizeof(String *);
    return vector;
}

void push_string(TemporaryStringVector *vector, String *new_element)
{
    String *element_copy = (String *)calloc(1, sizeof(String));
    memcpy(element_copy, new_element, sizeof(String));

    if (vector->size < vector->capacity)
    {
    }
    else
    {
        String **new_vec_location = realloc(vector->elements, sizeof(String *) * (vector->capacity + 10));
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

String *get_string(TemporaryStringVector *vector, int index)
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
