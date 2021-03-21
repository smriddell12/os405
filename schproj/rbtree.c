//README this is the beginning of the tree coding that I started doing but ran out of time
//I'm going to continue working on it because I actually had a lot of fun doing the diagrams, they're like puzzles


#include <stdio.h>
#include <stdlib.h>

//COLOR KEY
//RED = 1
//BLACK = 0


//node -- vruntime is the data
struct rbNode{
    int data;
    int color;
    struct rbNode *parent;
    struct rbNode *LChild;
    struct rbNode *RChild;
};
//no root in an empty tree
struct rbNode *root = NULL;

//start the tree
struct rbNode *createNode(int data){
    struct rbNode *newNode;
    newNode = (struct rbNode *)malloc(sizeof(struct rbNode));
    newNode->data = data;
    newNode->color = 1; 		        	//new nodes are always red
    newNode->parent = NULL;
    newNode->LChild = NULL;
    newNode->RChild = NULL;
    return newNode;
}

//insertion
void insert(int data){
    struct rbNode *curr_Node;
    if(root == NULL){
        root = createNode(data);
        return;
    }
   

        
    
}
