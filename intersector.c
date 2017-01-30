#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct tree {
    char *word;
    struct tree *left;
    struct tree *right;
} tree;

tree *Insert(tree *root, char *word)
{
    if( root == NULL )
    {
        root = (tree *)malloc(sizeof(tree));
        
        if (root == NULL)
        {
            printf("Memory Error");
            return NULL;
        }
        else
        {
            root->word = word;
            root->left = root->right = NULL;
        }
    }
    else
    {
        int cmpValue = 0;
        cmpValue = strncasecmp(word, root->word, strlen(word));
        if (cmpValue == 0)
        {
            return root;
        }
        else if (cmpValue > 0)
        {
            root->right = Insert(root->right, word);
        }
        else
        {
            root->left = Insert(root->left, word);
        }
    }
    return root;
}

void destroy_tree(tree* root)
{
    if (root == NULL)
    {
         return;
    }
       
    destroy_tree(root->left);  
    destroy_tree(root->right);

    free(root->word);
    free(root);
} 

void inOrder(tree *root)
{
    if (root->left != NULL)
    {
        inOrder(root->left);
    }

    printf("%s\n", root->word);

    if (root->right != NULL)
    {
        inOrder(root->right);
    }
}

tree * load_file(FILE *fp, tree *root)
{

        char buf[256];
        size_t wordLen = 0;
        char *word = NULL;

        while (fscanf(fp, " %255s", buf) == 1)
        {
            wordLen = strlen(buf);
            word = malloc(sizeof(char) * wordLen + 1);
            word[wordLen] = '\0';
            word = strncpy(word, buf, wordLen);

            root = Insert(root, word);
        }
        
        return root;
}

int main(int argc, char *argv[])
{
    if ( argc < 3 )
    {
        printf("ERROR, USAGE: [./intersector] [<First file>] [...<One or more files>]\n");
        return 0;     
    }
    
    FILE *fp = fopen(argv[1], "r");

    if(fp == NULL)
    {
        printf("ERROR, USAGE: %s appears to be missing\n", argv[1]);
        return 0;
    }

    tree *root = NULL;
    root = load_file(fp, root);

    fclose(fp);

    inOrder(root);
    

    destroy_tree(root);
}
