#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // rb트리 선언
  // TODO: initialize struct if needed test
  node_t *nil = (node_t *)calloc(1, sizeof(node_t)); // 닐 노드 선언
  nil->color = RBTREE_BLACK; // 닐 노드의 색은 검정

  p->nil = nil; //
  p->root = nil;

  return p;
}


void delete_all_nodes(rbtree *t, node_t *node) {
  if (node != t->nil) {
    delete_all_nodes(t, node->left);   // 왼쪽 서브트리 해제
    delete_all_nodes(t, node->right);  // 오른쪽 서브트리 해제
    free(node);                        // 현재 노드 해제
  }
}


void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if (t != NULL) {
    delete_all_nodes(t, t->root);  // 모든 노드 해제
    free(t->nil);                  // NIL 노드 해제
    free(t);                       // 트리 구조체 해제
  }
}


node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t *)calloc(1, sizeof(node_t));  // 삽입할 노드 z
  z->key = key;
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;
  z->parent = t->nil;

  node_t *y = t->nil;  // 부모 노드
  node_t *x = t->root;  // 현재 노드

  while (x != t->nil)  // 삽입 위치를 찾을 때까지 반복
  {
    y = x;  // 현재 노드를 부모로 설정
    if (key < x->key) { // 키가 현재 노드의 키보다 작으면
      x = x->left;  // 왼쪽 서브트리로 이동
    } else if (key >= x->key) {  // 키가 현재 노드의 키보다 크거나 같으면
      x = x->right;  // 오른쪽 서브트리로 이동
    }
  }
  
  z->parent = y;  // 삽입할 노드의 부모를 설정
  if (y == t->nil) {  // 부모 노드가 NIL 노드면 루트 노드이므로
    t->root = z;  // 새로운 루트로 설정
  } else if (key < y->key) {  // 키가 부모 노드보다 작으면
    y->left = z;  // 부모의 왼쪽 자식으로 설정
  } else {
    y->right = z; // 부모의 오른쪽 자식으로 설정
  }

  // return t->root;
  rbtree_insert_fixup(t, z);  // 삽입 후 레드-블랙 트리 속성 위반을 수정

  return z;
}


// 왼쪽 회전을 위한 함수
void left_rotate(rbtree *t, node_t *x){
  node_t *y;       // x의 오른쪽 자식 (회전의 중심이 될 노드)
  y = x->right;    // y를 x의 오른쪽 자식으로 설정
  
  x->right = y->left;  // x의 오른쪽 자식(서브 트리)을 y의 왼쪽 자식(서브 트리)으로 변경
  if (y->left != t->nil){  // y의 왼쪽 자식이 NIL이 아닌 경우
    y->left->parent = x;   // y의 왼쪽 자식의 부모를 x로 설정
  }

  y->parent = x->parent;   // y의 부모를 x의 부모로 설정
  
  if (x->parent == t->nil) {  // x가 루트 노드인 경우(루트 노드는 부모가 없음)
    t->root = y;              // y를 새로운 루트로 설정
  } else if (x == x->parent->left) {  // x가 부모의 왼쪽 자식인 경우
    x->parent->left = y;              // 부모의 왼쪽 자식을 y로 변경
  } else {  // x가 부모의 오른쪽 자식인 경우
    x->parent->right = y;             // 부모의 오른쪽 자식을 y로 변경
  }
  
  y->left = x;    // y의 왼쪽 자식을 x로 설정
  x->parent = y;  // x의 부모를 y로 설정
}


void right_rotate(rbtree *t, node_t *y){
  node_t *x;   // y의 왼쪽 자식
  x = y->left; // x를 y의 왼쪽 자식으로 설정
  
  y->left = x->right;  // y의 왼쪽 자식을 x의 오른쪽 자식으로 변경
  if (x->right != t->nil){  // x의 오른쪽 자식이 nil이 아닌 경우
    x->right->parent = y;  // x의 오른쪽 자식의 부모를 y로 설정
  }

  x->parent = y->parent;  // x의 부모를 y의 부모로 설정
  
  if (y->parent == t->nil) { // y가 루트 노드인 경우(루트 노드는 부모가 없음)
    t->root = x;   // x를 새로운 루트로 설정
  } else if (y == y->parent->right) { // y가 부모의 오른쪽 자식인 경우
    y->parent->right = x;  // 부모의 오른쪽 자식을 x로 변경
  } else {  // y가 부모의 왼쪽 자식인 경우
    y->parent->left = x;  // 부모의 왼쪽 자식을 x로 변경
  }
  
  x->right = y;  // x의 오른쪽 자식을 y로 설정
  y->parent = x;  // y의 부모를 x로 설정
}


void rbtree_insert_fixup(rbtree *t, node_t *z){
  node_t *y;
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)  // 케이스 1: 부모가 할아버지의 왼쪽 자식
    {
      y = z->parent->parent->right;  // 삼촌 노드

      if (y->color == RBTREE_RED)  // 케이스 1: 삼촌이 빨간색
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } 
      else  // 케이스 2: 삼촌이 검은색
      {
        if (z == z->parent->right)  // 케이스 2: z가 부모의 오른쪽 자식
        {
          z = z->parent;
          left_rotate(t, z);
        }
        // 케이스 3: z가 부모의 왼쪽 자식
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    } 
    else  // 케이스 1의 대칭: 부모가 할아버지의 오른쪽 자식
    {
      y = z->parent->parent->left;  // 삼촌 노드

      if (y->color == RBTREE_RED)  // 케이스 1: 삼촌이 빨간색
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } 
      else  // 케이스 2: 삼촌이 검은색
      {
        if (z == z->parent->left)  // 케이스 2: z가 부모의 왼쪽 자식
        {
          z = z->parent;
          right_rotate(t, z);
        }
        // 케이스 3: z가 부모의 오른쪽 자식
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }

    t->root->color = RBTREE_BLACK;  // 루트는 항상 검은색
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *current = t->root;  // 루트 노드부터 검색 시작

  while (current != t->nil) // NIL 노드에 도달할 때까지 검색
  {
    if (key < current->key) // 찾는 키가 현재 노드보다 작으면 왼쪽 서브트리로
    {
      current = current->left; // 현재를 왼쪽으로 이동
    }
    else if (key > current->key) // 찾는 키가 현재 노드보다 크면 오른쪽 서브트리로
    {
      current = current->right;  // 현재를 오른쪽으로 이동
    }
    else                   // 찾는 키와 현재 노드가 같으면 찾은 것
    {
      return current;      // 값이 같으면 값 반환
    }
  }

  return NULL;  // 널 반환 
}


// 서브트리의 최솟값 찾기 
node_t *rbtree_sub_min(node_t *root, node_t *nil) {
  node_t *current = root;
  if (current == nil) {
    return nil;
  }
  while (current->left != nil) {
    current = current->left;
  }
  return current;
}


// 전체 트리의 최솟값 찾기
node_t *rbtree_min(const rbtree *t) {
  return rbtree_sub_min(t->root, t->nil);
}


// 최댓값 찾기
node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *current = t->root;

  if (current == t->nil)
  {
    return t->nil;
  }

  while (current->right != t->nil)
  {
    current = current->right;
  }

  return current;
}


int rbtree_erase(rbtree *t, node_t *z) {
  // x - y의 자리를 차지하는 노드
  // y - 실제로 제거되는 노드
  // z - 삭제할 노드
  // delete_fixup은 실제로 제거된 노드 y의 색상과 그 자리를 차지한 노드 x를 기준으로 수행되기 때문
  node_t *x;
  node_t *y;
  color_t y_original_color;  // 지역 변수로 저장

  y = z;
  y_original_color = y->color;  // 원래 색상 저장

  if (z->left == t->nil)
  {
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else // 자식을 둘 있을 때
  {
    y = rbtree_sub_min(z->right, t->nil);
    y_original_color = y->color;  // 원래 색상 저장
    x = y->right;
    
    if (y != z->right)
    {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    else
    {
      x->parent = y;
      // rbtree_transplant(t, z, y);
      // y->left = z->left;
      // y->left->parent = y;
      // y->color = z->color;
    }
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (y_original_color == RBTREE_BLACK)  // 지역 변수 사용
  {
    rbtree_delete_fixup(t, x);
  }
  
  free(z);
  return 0;
}


void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil) {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }

  v->parent = u->parent;
}


void rbtree_delete_fixup(rbtree *t, node_t *x){
  node_t *w;

  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      w = x->parent->right;

      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }

        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      w = x->parent->left;

      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }

      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }

        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  
  return 0;
}
