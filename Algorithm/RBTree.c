#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

enum color{
    RED = 0, 
    BLACK};

// 定义红黑树节点结构体
typedef struct _rbtree_node{
    int key;
    void* value;

    struct _rbtree_node* left;
    struct _rbtree_node* right;

    struct _rbtree_node* parent;

    unsigned char color; // 0: red, 1: black
}rbtree_node;

// 叶子节点都是黑色的，可以定义为空节点，为了好判断，不能定义为NULL，否则是非法操作
typedef struct _rbtree{
    struct _rbtree_node* root;
    struct _rbtree_node* nil; // 叶子节点，黑色的空节点
    int size; // 树中节点的数量
}rbtree;

void leftRotate(rbtree* tree, rbtree_node* x);
void rightRotate(rbtree* tree, rbtree_node* y);
void insertFixup(rbtree* tree, rbtree_node* z);
void rbtree_insert(rbtree* tree, rbtree_node* node);

rbtree* rbtree_init();
void print_tree(rbtree* tree);
void free_tree(rbtree* tree, rbtree_node* node);
const char* color_to_str(unsigned char color);

int main()
{
    const int size = 15;
    int array[15];
    srand(time(NULL));
    
    printf("生成 %d 个随机数:\n", size);
    for (int i = 0; i < size; ++i) {
        array[i] = rand() % 100;
        printf("%d ", array[i]);
    }
    printf("\n\n");
    
    // 初始化红黑树
    rbtree* tree = rbtree_init();
    if (!tree) {
        printf("红黑树初始化失败\n");
        return 1;
    }
    
    // 插入节点
    for (int i = 0; i < size; ++i) {
        rbtree_node* node = (rbtree_node*)malloc(sizeof(rbtree_node));
        if (!node) {
            printf("内存分配失败\n");
            free_tree(tree, tree->root);
            free(tree->nil);
            free(tree);
            return 1;
        }
        
        node->key = array[i];
        node->value = NULL;
        node->left = tree->nil;
        node->right = tree->nil;
        node->parent = tree->nil;
        node->color = RED;
        
        rbtree_insert(tree, node);
        tree->size++;
        
        printf("插入节点: %d (颜色: %s)\n", node->key, color_to_str(node->color));
    }
    
    printf("\n最终红黑树结构 (大小: %d):\n", tree->size);
    print_tree(tree);
    
    
    // 释放内存
    free_tree(tree, tree->root);
    free(tree->nil);
    free(tree);
    
    return 0;
}

void leftRotate(rbtree* tree, rbtree_node* x)
{
    rbtree_node* y = x->right; // 右子节点
    
    // 断x与y的父子关系
    x->right = y->left; // 让左子节点成为右子节点的父节点
    if(y->left != tree->nil) // 如果左子节点不是叶子节点，则更新父节点的指向
    {
        y->left->parent = x;
    }

    // 断x与x父节点的父子关系，并建立x父节点与y的父子关系
    y->parent = x->parent; // 让y的父节点指向x的父节点
    if(x->parent == tree->nil) // 如果x是根节点，则更新根节点的指向
    {
        tree->root = y;
    }else if(x == x->parent->left) // 如果x是其父节点的左子节点，则更新其父节点的左子节点
    {
        x->parent->left = y;
    }else // 如果x是其父节点的右子节点，则更新其父节点的右子节点
    {
        x->parent->right = y;
    }

    // 更新x与y的父子关系
    y->left = x; // 让x成为y的左子节点
    x->parent = y; // 更新父节点的指向
}

// 将左旋反过来，x变y，left变right，right变left
void rightRotate(rbtree* tree, rbtree_node* y)
{
    rbtree_node* x = y->left; // 左子节点
    
    // 断y与x的父子关系
    y->left = x->right; // 让右子节点成为左子节点的父节点
    if(x->right != tree->nil) // 如果右子节点不是叶子节点，则更新父节点的指向
    {
        x->right->parent = y;
    }

    // 断y与y父节点的父子关系，并建立y父节点与x的父子关系
    x->parent = y->parent; // 让x的父节点指向y的父节点
    if(y->parent == tree->nil) // 如果y是根节点，则更新根节点的指向
    {
        tree->root = x;
    }else if(y == y->parent->right) // 如果y是其父节点的右子节点，则更新其父节点的右子节点
    {
        y->parent->right = x;
    }else // 如果y是其父节点的左子节点，则更新其父节点的左子节点
    {
        y->parent->left = x;
    }

    // 更新y与x的父子关系
    x->right = y; // 让y成为x的右子节点
    y->parent = x; // 更新父节点的指向
}

void rbtree_insert(rbtree* tree, rbtree_node* node)
{
    rbtree_node* y = tree->nil;  // 记录x的父节点
    rbtree_node* x = tree->root; // 根节点

    // 找到插入的位置
    while(x != tree->nil) // 如果不是叶子节点，则继续查找
    {   
        y = x; // 记录父节点
        if(node->key < x->key) // 如果插入节点的键小于当前节点的键，则向左子节点查找
        {
            x = x->left;
        }else if(node->key > x->key) // 如果插入节点的键大于等于当前节点的键，则向右子节点查找
        {
            x = x->right;
        }else{      // 如果插入节点的键等于当前节点的键，则替换当前节点
            x->value = node->value;
        }
    }

    // 如果是空红黑树，则直接插入
    if(y == tree->nil) // 如果y是叶子节点，则直接插入
    {
        tree->root = node; // 更新根节点
    }else{
        if(y->key > node->key) // 如果插入节点的键小于父节点的键，则将插入节点设置为左子节点
        {
            y->left = node;
        }else // 如果插入节点的键大于父节点的键，则将插入节点设置为右子节点
        {
            y->right = node;
        }
    }

    node->parent = y; // 更新父节点

    node->left = tree->nil;
    node->right = tree->nil;
    node->color = RED ; // 新插入的节点默认为红色，不改变黑高

    // 插入之后，进行旋转和变色
    insertFixup(tree, node);
}
// 插入之后的调整
void insertFixup(rbtree* tree, rbtree_node* node)
{
    rbtree_node* parent = node->parent;
    rbtree_node* grandparent = NULL;
    rbtree_node* uncle = NULL;
    // 如果节点本身是红色
    while(parent->color == RED) // 如果父节点是红色，那么祖父节点必是黑色，叔父节点可能是红色或黑色
    {
        grandparent = parent->parent; // 获取祖父节点
        if(parent == grandparent->left) // 如果父节点是其祖父节点的左子节点
        {
            uncle = grandparent->right; // 叔父节点
            // 情况2和3：叔父节点是红色
            if (uncle->color == RED)
            {
                parent->color = BLACK; // 将父节点变为黑色
                uncle->color = BLACK; // 将叔父节点变为黑色
                grandparent->color = RED; // 将祖父节点变为红色
                node = grandparent; // 继续向上调整
            }else // 叔父节点是黑色
            {
                // 情况4：N是P的左子节点(LL)
                if (node == parent->left)
                {
                    rightRotate(tree, grandparent); // 对祖父节点进行右旋
                    parent = node->parent; // 更新父节点
                }
                // 情况5：N是P的右子节点(LR)
                else
                {
                    leftRotate(tree, parent); // 对父节点进行左旋
                    parent = node->parent; // 更新父节点
                    rightRotate(tree, grandparent); // 对祖父节点进行右旋
                }

                parent->color = BLACK; // 将父节点变为黑色
                grandparent->color = RED; // 将祖父节点变为红色
                node = parent; // 继续向上调整
            }
        }else // 父节点是其祖父节点的右子节点
        {
            uncle = grandparent->left; // 获取叔父节点

            // 情况2和3：叔父节点是红色
            if (uncle->color == RED)
            {
                parent->color = BLACK; // 将父节点变为黑色
                uncle->color = BLACK; // 将叔父节点变为黑色
                grandparent->color = RED; // 将祖父节点变为红色
                node = grandparent; // 继续向上调整
            }
            else // 叔父节点是黑色
            {
                // 情况6：N是P的左子节点(RL)
                if (node == parent->left)
                {
                    rightRotate(tree, parent); // 对父节点进行右旋
                    parent = node->parent; // 更新父节点
                    leftRotate(tree, grandparent); // 对祖父节点进行左旋
                }
                // 情况7：N是P的右子节点(RR)
                else
                {
                    leftRotate(tree, grandparent); // 对祖父节点进行左旋
                    parent = node->parent; // 更新父节点
                }

                parent->color = BLACK; // 将父节点变为黑色
                grandparent->color = RED; // 将祖父节点变为红色
                node = parent; // 继续向上调整
            }
        }
    }
    tree->root->color = BLACK; // 将根节点变为黑色
}

// 初始化红黑树
rbtree* rbtree_init() 
{
    rbtree* tree = (rbtree*)malloc(sizeof(rbtree));
    if (!tree) return NULL;
    
    tree->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
    if (!tree->nil) {
        free(tree);
        return NULL;
    }
    
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = tree->nil;
    tree->root = tree->nil;
    tree->size = 0;
    
    return tree;
}

// 辅助函数：颜色转字符串
const char* color_to_str(unsigned char color) 
{
    return color == RED ? "RED" : "BLACK";
}

// 打印树结构
void print_tree(rbtree* tree) 
{
    printf("红黑树节点信息 (根节点: %d):\n", 
           tree->root != tree->nil ? tree->root->key : -1);
    printf("%-8s %-8s %-8s %-8s %-8s %-8s\n", 
           "节点", "颜色", "左孩子", "右孩子", "父节点", "值");
    
    if (tree->root == tree->nil) {
        printf("树为空\n");
        return;
    }
    
    // 使用队列进行层次遍历
    rbtree_node* queue[1000];
    int front = 0, rear = 0;
    int value = -1;
    queue[rear++] = tree->root;
    
    while (front < rear) {
        rbtree_node* current = queue[front++];
        
        if (current == tree->nil) continue;
        
        // 打印节点信息
        value = current->value ? *(int*)current->value : -1;
        printf("%-8d %-8s %-8d %-8d %-8d %-8d\n", 
               current->key,
               color_to_str(current->color),
               current->left != tree->nil ? current->left->key : -1,
               current->right != tree->nil ? current->right->key : -1,
               current->parent != tree->nil ? current->parent->key : -1,
               value);
        
        // 将子节点加入队列
        if (current->left != tree->nil) {
            queue[rear++] = current->left;
        }
        if (current->right != tree->nil) {
            queue[rear++] = current->right;
        }
    }
}

// 释放树内存
void free_tree(rbtree* tree, rbtree_node* node) 
{
    if (node == tree->nil) return;
    
    free_tree(tree, node->left);
    free_tree(tree, node->right);
    free(node);
}