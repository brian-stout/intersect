#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

typedef struct tree {
    char *word;
    struct tree *left;
    struct tree *right;
} tree;

tree * Insert(tree *root, char *word);
void destroy_tree(tree* root);
tree * load_file(FILE *fp, tree *root);
bool in_tree(tree *root, char *word);
void in_order(tree *root);
tree * tree_intersection(FILE *fp, tree *root);

int main(int argc, char *argv[])
{

    //The program requires at a mininum, two specified files but can accept more
    if ( argc < 3 )
    {
        printf("ERROR, USAGE: [./intersector] [<First file>] [...<One or more files>]\n");
        return 0;     
    }
    
    FILE *fp = fopen(argv[1], "r");

    //Checks to see if the file was opened sucessfull, if not, assumed to not exist
    if(fp == NULL)
    {
        printf("ERROR, USAGE: file %s appears to be missing\n", argv[1]);
        return 0;
    }

    //Creates the root pointer to the tree struct accessed in the main program
    tree *root = NULL;

    //Populates a binary search tree with all the words found at FP (a FILE *)
    root = load_file(fp, root);

    //Done loading, close file
    fclose(fp); 

    //For loop so the intersect function is ran on each additional file specified
    //  at the command line.  Var i starts at 2 
    for(int i = 2; i < argc; ++i)
    {
        fp = fopen(argv[i], "r");

        //If the file can't be opened, error out
        if(fp == NULL)
        {
            printf("ERROR, USAGE: file %s appears to be missing\n", argv[i]);
            return 0;
        }
        //Runs the word document opened through the intersection function
        root = tree_intersection(fp, root);
        fclose(fp);
    }

    //prints out the results in order (alphabetical)
    in_order(root);

    //Function to free all the memory
    destroy_tree(root);
    root = NULL;
}

tree *Insert(tree *root, char *word)
{

    //If the data has finally found an empty node, insert it
    if( root == NULL )
    {
        //Sets aside memory for three pointers
        root = (tree *)malloc(sizeof(tree));
        
        //If still NULL, then Malloc failed
        if (root == NULL)
        {
            printf("Memory Error");
            return NULL;
        }
        else
        {
            //Set the data
            root->word = word;
            root->left = root->right = NULL;
        }
    }
    else
    {
        int cmpValue = 0;

        //Compares the words with the data given and what's currently at the root
        //  uses strncasecmp() to determine if it's greater than or less than
        //  without being case sensitive
        cmpValue = strncasecmp(word, root->word, strlen(word));

        //There should be no duplicates so if the word is equal it should be free'd
        if (cmpValue == 0)
        {
            //Since word is being discarded it needs to be free()'d
            free(word);
            return root;
        }
        //If it's greater send it to the right
        else if (cmpValue > 0)
        {
            root->right = Insert(root->right, word);
        }
        //If it's lesser than send it to the left
        else
        {
            root->left = Insert(root->left, word);
        }
    }
    return root;
}

void destroy_tree(tree* root)
{

    //If there's a word than free it's memory
    if(root->word)
    {
        free(root->word);
    }
    //If there is a left child then step into it's node to free it
    if(root->left)
    {
        destroy_tree(root->left);
    }
    //Same with right child.
    if(root->right)
    {
        destroy_tree(root->right);
    }

    //Clean up current node
    free(root);
}

bool in_tree(tree *root, char *word)
{
        int cmpValue = 0;
        cmpValue = strncasecmp(word, root->word, strlen(word));
        //If strncasecmp() returns a 0 then the words match
        if (cmpValue == 0)
        {
            return true;
        }
        //If it's bigger than ,search the right side of the tree
        else if (cmpValue > 0)
        {
            if(root->right != NULL)
            {
                return in_tree(root->right, word);
            }
            //If there is no right side of the tree, there is no match so
            //  return false
            else
            {
                return false;
            }
        }
        //If it's smaller than, search the left side of the tree
        else
        {
            if(root->left != NULL)
            {
                return in_tree(root->left, word);
            }
            //No match, return false
            else
            {
                return false;
            }
        }    
}

void in_order(tree *root)
{
    //Looks for the smallest value first
    if (root->left != NULL)
    {
        in_order(root->left);
    }

    //Prints out the word if there's no more smaller values
    printf("%s\n", root->word);

    //Steps in to the right child if there is one
    if (root->right != NULL)
    {
        in_order(root->right);
    }
}



tree * load_file(FILE *fp, tree *root)
{

        //Maximum character size
        char buf[256];
        //Variable to avoid redoing calculations
        size_t wordLen = 0;

        //word pointer to copy buf in to
        char *word = NULL;

        //Until EOF, scan every individual word up to 255 characters (+1 for null byte)
        while (fscanf(fp, " %255s", buf) != EOF)
        {
            //Sets char pointer to NULL at start to avoid undefined behavior
            word = NULL;
            wordLen = strlen(buf);

            //Sets aside memory for the word pointer to point to +1 for null byte
            word = malloc(sizeof(char) * wordLen + 1);

            //Ensures nullbyte is there
            word[wordLen] = '\0';

            //Copies whats in buf in to word
            word = strncpy(word, buf, wordLen);

            //Puts it in to a tree
            root = Insert(root, word);
        }
        
        word = NULL;

        //Returns a tree pointer for main to handle
        return root;
}

tree * tree_intersection(FILE *fp, tree *root)
{
    char buf[256];
    size_t wordLen = 0;
    char *word = NULL;
    tree *newTree = NULL;

    while (fscanf(fp, " %255s", buf) != EOF)
    {
        //Only adds the word to the new tree if it's in the original tree
        if(in_tree(root, buf))
        {
            word = NULL;
            wordLen = strlen(buf);
            word = malloc(sizeof(char) * wordLen + 1);
            word[wordLen] = '\0';
            word = strncpy(word, buf, wordLen);

            //Adding word to the new tree
            newTree = Insert(newTree, word);            
        }   
    }

    //Free'ing up the memory in the original tree since program is about to lose
    //  it's handle on it
    destroy_tree(root);

    //Returns a pointer to the new tree which only has words both files contained
    return newTree;
}
