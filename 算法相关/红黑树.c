#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>

typedef enum NodeColor {
  RED = 0,
  BLACK
}NodeColor;

typedef struct rbNode {
  int data;
  struct rbNode * left;
  struct rbNode * right;
  struct rbNode * parent;
  NodeColor color;
}rbNode;

rbNode * createNode(const int data) {
  rbNode * node = (rbNode *)malloc(sizeof(rbNode));
  node->data = data;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  node->color = RED;
  return node;
}

rbNode * findNode(rbNode * const p_root,const int data){
  if(!p_root) {
    return NULL;
  }
  else if(data == p_root->data) {
    return p_root;
  }else if(data < p_root->data) {
    return findNode(p_root->left, data);
  }else {
    return findNode(p_root->right, data);
  }
}

void insertNodeImpl(rbNode *, rbNode *);

rbNode * insertNode(rbNode ** pp_root, const int data) {
  rbNode * p_cur_node = createNode(data);
  if(p_cur_node == NULL) {
    return (rbNode *)((intptr_t)-1);
  }

  if (*pp_root == NULL) {
    p_cur_node->color = BLACK;
    *pp_root = p_cur_node;
    return *pp_root;
  }
  else {
    insertNodeImpl(*pp_root, p_cur_node);
  }
  while((*pp_root)->parent != NULL) {
    *pp_root = (*pp_root)->parent;
  }
  (*pp_root)->color = BLACK;
  return p_cur_node;
}

void L_Rot(rbNode * rot_center) {
  printf("L_rot\n");
  assert(rot_center != NULL);
  assert(rot_center->right != NULL);
  rot_center->right->parent = rot_center->parent;
  if(rot_center->parent) {
    if(rot_center->parent->left == rot_center) {
      rot_center->parent->left = rot_center->right;
    }else {
      rot_center->parent->right = rot_center->right;
    }
  }
  rot_center->parent = rot_center->right;
  rot_center->right = rot_center->right->left;
  rot_center->parent->left = rot_center;
  if(rot_center->right) {
    rot_center->right->parent = rot_center;
  }
}

void R_Rot(rbNode * rot_center) {
  printf("R_rot\n");
  assert(rot_center != NULL);
  assert(rot_center->left != NULL);
  rot_center->left->parent = rot_center->parent;
  if(rot_center->parent) {
    if(rot_center->parent->left == rot_center) {
      rot_center->parent->left = rot_center->left;
    }else {
      rot_center->parent->right = rot_center->left;
    }
  }
  rot_center->parent = rot_center->left;
  rot_center->left = rot_center->left->right;
  rot_center->parent->right = rot_center;
  if(rot_center->left) {
    rot_center->left->parent = rot_center;
  }
}

void insertAdjustNode(rbNode * p_father, rbNode * p_cur) {
  // case 0 : father is black
  if (p_father->color == BLACK) {
    return;
  }
  if(p_cur == p_father->left) {
    // case 1 : cur is father's left child
    rbNode * p_grandfather = p_father->parent;
    assert(p_grandfather); // root must be black

    rbNode * p_uncle = NULL;
    if (p_father == p_grandfather->left){
      p_uncle = p_grandfather->right;
    }else{
      p_uncle = p_grandfather->left;
    }

    if (p_uncle != NULL && p_uncle->color == RED) {
      // case 1-1 uncle is red :
      p_father->color = BLACK;
      p_uncle->color = BLACK;
      p_grandfather->color = RED;
      if(p_grandfather->parent) {
        insertAdjustNode(p_grandfather->parent,p_grandfather);
      }
      else {
        p_grandfather->color = BLACK;
      }
    }
    else {
      // case 1-2 uncle is black or nullptr
      if (p_grandfather->left == p_father) {
        // case 1-2-1 father is left_child
        p_grandfather->color = RED;
        p_father->color = BLACK;
        R_Rot(p_grandfather);
      }
      else {
        // case 1-2-2 father is right_child
        p_grandfather->color = RED;
        p_cur->color = BLACK;
        R_Rot(p_father);
        L_Rot(p_grandfather);
      }
    }
  }
  else {
    // case 2 : cur is father's right child
    rbNode * p_grandfather = p_father->parent;
    assert(p_grandfather); // root must be black

    rbNode * p_uncle = NULL;
    if (p_father == p_grandfather->left){
      p_uncle = p_grandfather->right;
    }else{
      p_uncle = p_grandfather->left;
    }

    if (p_uncle != NULL && p_uncle->color == RED) {
      // case 2-1 uncle is red :
      p_father->color = BLACK;
      p_uncle->color = BLACK;
      p_grandfather->color = RED;
      if(p_grandfather->parent) {
        insertAdjustNode(p_grandfather->parent,p_grandfather);
      }
      else {
        p_grandfather->color = BLACK;
      }
    }
    else {
      // case 2-2 uncle is black or nullptr
      if (p_grandfather->left == p_father) {
        // case 2-2-1 father is left_child
        p_grandfather->color = RED;
        p_cur->color = BLACK;
        L_Rot(p_father);
        R_Rot(p_grandfather);
      }
      else {
        // case 2-2-2 father is right_child
        p_grandfather->color = RED;
        p_father->color = BLACK;
        L_Rot(p_grandfather);
      }
    }
  }
}

void insertNodeImpl(rbNode * p_root, rbNode * p_cur) {
  if(p_cur->data < p_root->data) {
    if (p_root->left == NULL) {
      p_root->left = p_cur;
      p_cur->parent = p_root;

      // adjustment
      // case 0 parent is black: needn't adjust

      // case 1 parent is red
      if (p_root->color == RED) {
        insertAdjustNode(p_root, p_cur);
      }
    }
    else {
      insertNodeImpl(p_root->left, p_cur);
    }
  }
  else if (p_cur->data > p_root->data) {
    if (p_root->right == NULL) {
      p_root->right = p_cur;
      p_cur->parent = p_root;
      if (p_root->color == RED) {
        insertAdjustNode(p_root, p_cur);
      }
    }
    else {
      insertNodeImpl(p_root->right, p_cur);
    }
  }
  else {
    //
  }
}

void deleteTree(rbNode * p_root){
  if(p_root == NULL) {
    return;
  }
  deleteTree(p_root->left);
  deleteTree(p_root->right);
  free(p_root);
}

/* love from chatGPT */
// GPT 写的打印一棵树，能凑合用。
// 计算树的高度
int getHeight(rbNode *root) {
    if (!root) return 0;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// 打印适量的空格
void printSpaces(int count) {
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

// 递归打印当前层的节点
void printLevel(rbNode *root, int level, int space) {
    if (level == 0) {
        if (root) {
            printf("%d%s", root->data, root->color == RED ? "R" : "B");
        } else {
            printf(" ");
        }
        return;
    }
    if (root == NULL) {
        printSpaces(space);
        return;
    }
    printLevel(root->left, level - 1, space / 2);
    printSpaces(space);
    printLevel(root->right, level - 1, space / 2);
}

// 层次结构化打印红黑树
void printTree(rbNode *root) {
    int height = getHeight(root);
    int maxWidth = (1 << height) - 1;  // 2^h - 1，表示打印的最大宽度

    for (int i = 0; i < height; i++) {
        int space = maxWidth / (1 << i);  // 控制当前层的间距
        printSpaces(space / 2);
        printLevel(root, i, space);
        printf("\n\n");
    }
}

/* GPT_END */


void removeAdjustNode(rbNode * p_cur);

int removeNode(rbNode ** pp_root, const int data) {
  if (!pp_root) {
    return 1;
  }
  if (!*pp_root) {
    return 1;
  }

  if ((*pp_root)->data == data && !(*pp_root)->parent && !(*pp_root)->left && !(*pp_root)->right) {
    // only one root node
    free(*pp_root);
    *pp_root = NULL;
    return 0;
  }

  rbNode * delNode = findNode(*pp_root, data);
  if (!delNode) {
    return 1;
  }

  if (delNode->left && delNode->right) {
    // have both left_child and right_child , find the next node
    rbNode * replaceNode = delNode->right;
    while(replaceNode->left) {
      replaceNode = replaceNode->left;
    }
    delNode->data = replaceNode->data;
    delNode = replaceNode;
  }
  if(delNode->left && !delNode->right) {
    // only have left child
    delNode->data = delNode->left->data;
    free(delNode->left);
    delNode->left = NULL;
    return 0;
  }
  if(delNode->right && !delNode->left) {
    // only have right child
    delNode->data = delNode->right->data;
    free(delNode->right);
    delNode->right = NULL;
    return 0;
  }
  // delNode have no child
  removeAdjustNode(delNode);
  if(delNode->parent->left == delNode) {
    delNode->parent->left = NULL;
  }else {
    delNode->parent->right = NULL;
  }
  free(delNode);

  while((*pp_root)->parent) {
    *pp_root = (*pp_root)->parent;
  }
  (*pp_root)->color = BLACK;
  return 0;
}

void removeAdjustNode(rbNode * p_cur) {
  if(p_cur->color == RED) {
    return;
  }
  if(p_cur->parent == NULL) {
    return;
  }
  rbNode * p_sibling = NULL;
  bool curIsLeft = false;
  if(p_cur == p_cur->parent->left) {
    p_sibling = p_cur->parent->right;
    curIsLeft = true;
  } else {
    p_sibling = p_cur->parent->left;
    curIsLeft = false;
  }

  if(p_sibling->color == RED) {
    p_sibling->color = BLACK;
    p_cur->parent->color = RED;
    if(curIsLeft) {
      L_Rot(p_cur->parent);
    }else{
      R_Rot(p_cur->parent);
    }
    removeAdjustNode(p_cur);
    return;
  }

  // sibling is BLACK
  if(curIsLeft){
    // sibling is right
    if(p_sibling->right && p_sibling->right->color == RED) {
      // sibling has right red child
      p_sibling->right->color = BLACK;
      p_sibling->color = p_cur->parent->color;
      p_cur->parent->color = BLACK;
      L_Rot(p_cur->parent);
      return;
    }else if(p_sibling->left && p_sibling->left->color == RED){
      // sibling has left red child
      p_sibling->left->color = p_cur->parent->color;
      p_cur->parent->color = BLACK;
      R_Rot(p_sibling);
      L_Rot(p_cur->parent);
      return;
    }else{
      // sibling has no red child
      p_sibling->color = RED;
      if(p_cur->parent->color == RED){
        p_cur->parent->color = BLACK;
      } else {
        removeAdjustNode(p_cur->parent);
      }
      return;
    }
  } else {
    // sibling is left child
    if(p_sibling->left && p_sibling->left->color == RED) {
      p_sibling->left->color = BLACK;
      p_sibling->color = p_cur->parent->color;
      p_cur->parent->color = BLACK;
      R_Rot(p_cur->parent);
      return;
    } else if (p_sibling->right && p_sibling->right->color == RED) {
      p_sibling->right->color = p_cur->parent->color;
      p_cur->parent->color = BLACK;
      L_Rot(p_sibling);
      R_Rot(p_cur->parent);
      return;
    } else {
      p_sibling->color = RED;
      if(p_cur->parent->color == RED) {
        p_cur->parent->color = BLACK;
      }else {
        removeAdjustNode(p_cur->parent);
      }
      return;
    }
  }

};

int main() {
  rbNode * p_testTree = NULL;

//  这些是插入的一个测试集
 insertNode(&p_testTree,17);
 insertNode(&p_testTree,18);
 insertNode(&p_testTree,23);
 insertNode(&p_testTree,34);
 insertNode(&p_testTree,27);
 insertNode(&p_testTree,15);
 insertNode(&p_testTree,9);
 insertNode(&p_testTree,6);
 insertNode(&p_testTree,8);
 insertNode(&p_testTree,5);
 insertNode(&p_testTree,25);

// 下面是删除的测试集
  // int tmp[] = {15,9,18,6,13,17,27,10,23,34,25,37};
  // for(int i = 0;i < sizeof(tmp)/sizeof(int);i++) {
  //   insertNode(&p_testTree,tmp[i]);
  // }

  // int rmTmp[] = {18,25,15,6,13,37,27,17,34,9,10,23};
  // for(int i = 0;i < sizeof(rmTmp)/sizeof(int);i++) {
  //   removeNode(&p_testTree,rmTmp[i]);
  // }

  // 测试通过。

  printTree(p_testTree);

  deleteTree(p_testTree);

  return 0;
}