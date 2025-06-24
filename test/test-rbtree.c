#include <assert.h>
#include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// new_rbtree should return rbtree struct with null root node
void test_init(void) {
  rbtree *t = new_rbtree();
  assert(t != NULL);
#ifdef SENTINEL
  assert(t->nil != NULL); // NIL 노드가 존재하는지 확인
  assert(t->root == t->nil); // 빈 트리에서 root가 NIL을 가리키는지 확인
#else
  assert(t->root == NULL); // NIL 없이 구현시 root가 NULL인지 확인
#endif
  delete_rbtree(t);
}

// root node should have proper values and pointers
void test_insert_single(const key_t key) {
  rbtree *t = new_rbtree();  // 새로운 레드-블랙 트리 생성
  node_t *p = rbtree_insert(t, key);  // 키를 가진 노드 삽입
  
  assert(p != NULL);  // 삽입된 노드가 NULL이 아닌지 확인
  assert(t->root == p);  // 삽입된 노드가 루트 노드인지 확인
  assert(p->key == key);  // 삽입된 노드의 키가 올바른지 확인
  // assert(p->color == RBTREE_BLACK);  // color of root node should be black

#ifdef SENTINEL
  // NIL 노드 사용 시: 자식들과 부모가 모두 NIL인지 확인
  assert(p->left == t->nil);   // 왼쪽 자식이 NIL인지 확인
  assert(p->right == t->nil);  // 오른쪽 자식이 NIL인지 확인
  assert(p->parent == t->nil); // 부모가 NIL인지 확인
#else
  // NULL 사용 시: 자식들과 부모가 모두 NULL인지 확인
  assert(p->left == NULL);   // 왼쪽 자식이 NULL인지 확인
  assert(p->right == NULL);  // 오른쪽 자식이 NULL인지 확인
  assert(p->parent == NULL); // 부모가 NULL인지 확인
#endif
  
  delete_rbtree(t);  // 트리 메모리 해제
}

// find should return the node with the key or NULL if no such node exists
void test_find_single(const key_t key, const key_t wrong_key) {
  rbtree *t = new_rbtree();  // 새로운 레드-블랙 트리 생성
  node_t *p = rbtree_insert(t, key);  // 올바른 키를 가진 노드 삽입

  node_t *q = rbtree_find(t, key);  // q에 찾은 결과 저장
  assert(q != NULL);  // 존재하는 키는 찾을 수 있어야 함
  assert(q->key == key);  // 찾은 노드의 키가 일치해야 함
  assert(q == p);   // 찾은 노드가 삽입한 노드와 동일해야 함

  q = rbtree_find(t, wrong_key);  // 잘못된 키를 찾는 결과 저장
  assert(q == NULL);  // 존재하지 않는 키는 찾을 수 없어야 함

  delete_rbtree(t); // 트리 메모리 해제
}

// erase should delete root node
void test_erase_root(const key_t key) {
  rbtree *t = new_rbtree();  // 새로운 레드-블랙 트리 생성
  node_t *p = rbtree_insert(t, key); // 키 삽입
  assert(p != NULL);   // 널이 아닌지 확인
  assert(t->root == p);  // 삽입한 키가 루트인지 확인 
  assert(p->key == key);  // p의 키가 삽입한 키인지 확인 

  rbtree_erase(t, p);   // 루트 지우기
#ifdef SENTINEL     
  assert(t->root == t->nil); // 지운 루트가 nil 노드인지 확인
#else
  assert(t->root == NULL);
#endif

  delete_rbtree(t);  // 트리 메모리 해제
}

static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    rbtree_insert(t, arr[i]);
  }
}

static int comp(const void *p1, const void *p2) {
  const key_t *e1 = (const key_t *)p1;
  const key_t *e2 = (const key_t *)p2;
  if (*e1 < *e2) {
    return -1;
  } else if (*e1 > *e2) {
    return 1;
  } else {
    return 0;
  }
};

// min/max should return the min/max value of the tree
void test_minmax(key_t *arr, const size_t n) {
  // null array is not allowed
  assert(n > 0 && arr != NULL);

  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(t->root != NULL);
#ifdef SENTINEL
  assert(t->root != t->nil);
#endif

  qsort((void *)arr, n, sizeof(key_t), comp);
  node_t *p = rbtree_min(t);
  assert(p != NULL);
  assert(p->key == arr[0]);

  node_t *q = rbtree_max(t);
  assert(q != NULL);
  assert(q->key == arr[n - 1]);

  rbtree_erase(t, p);
  p = rbtree_min(t);
  assert(p != NULL);
  assert(p->key == arr[1]);

  if (n >= 2) {
    rbtree_erase(t, q);
    q = rbtree_max(t);
    assert(q != NULL);
    assert(q->key == arr[n - 2]);
  }

  delete_rbtree(t);
}

void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
  assert(t != NULL);

  insert_arr(t, arr, n);
  qsort((void *)arr, n, sizeof(key_t), comp);

  key_t *res = calloc(n, sizeof(key_t));
  rbtree_to_array(t, res, n);
  for (int i = 0; i < n; i++) {
    assert(arr[i] == res[i]);
  }
  free(res);
}

void test_multi_instance() {
  rbtree *t1 = new_rbtree();
  assert(t1 != NULL);
  rbtree *t2 = new_rbtree();
  assert(t2 != NULL);

  key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
  insert_arr(t1, arr1, n1);
  qsort((void *)arr1, n1, sizeof(key_t), comp);

  key_t arr2[] = {4, 8, 10, 5, 3};
  const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
  insert_arr(t2, arr2, n2);
  qsort((void *)arr2, n2, sizeof(key_t), comp);

  key_t *res1 = calloc(n1, sizeof(key_t));
  rbtree_to_array(t1, res1, n1);
  for (int i = 0; i < n1; i++) {
    assert(arr1[i] == res1[i]);
  }

  key_t *res2 = calloc(n2, sizeof(key_t));
  rbtree_to_array(t2, res2, n2);
  for (int i = 0; i < n2; i++) {
    assert(arr2[i] == res2[i]);
  }

  free(res2);
  free(res1);
  delete_rbtree(t2);
  delete_rbtree(t1);
}

// Search tree constraint
// The values of left subtree should be less than or equal to the current node
// The values of right subtree should be greater than or equal to the current
// node

static bool search_traverse(const node_t *p, key_t *min, key_t *max,
                            node_t *nil) {
  if (p == nil) {
    return true;
  }

  *min = *max = p->key;

  key_t l_min, l_max, r_min, r_max;
  l_min = l_max = r_min = r_max = p->key;

  const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
  if (!lr || l_max > p->key) {
    return false;
  }
  const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
  if (!rr || r_min < p->key) {
    return false;
  }

  *min = l_min;
  *max = r_max;
  return true;
}

void test_search_constraint(const rbtree *t) {
  assert(t != NULL);
  node_t *p = t->root;
  key_t min, max;
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  assert(search_traverse(p, &min, &max, nil));
}

// Color constraint
// 1. Each node is either red or black. (by definition)
// 2. All NIL nodes are considered black.
// 3. A red node does not have a red child.
// 4. Every path from a given node to any of its descendant NIL nodes goes
// through the same number of black nodes.

bool touch_nil = false;
int max_black_depth = 0;

static void init_color_traverse(void) {
  touch_nil = false;
  max_black_depth = 0;
}

static bool color_traverse(const node_t *p, const color_t parent_color,
                           const int black_depth, node_t *nil) {
  if (p == nil) {
    if (!touch_nil) {
      touch_nil = true;
      max_black_depth = black_depth;
    } else if (black_depth != max_black_depth) {
      return false;
    }
    return true;
  }
  if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
    return false;
  }
  int next_depth = ((p->color == RBTREE_BLACK) ? 1 : 0) + black_depth;
  return color_traverse(p->left, p->color, next_depth, nil) &&
         color_traverse(p->right, p->color, next_depth, nil);
}

void test_color_constraint(const rbtree *t) {
  assert(t != NULL);
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  node_t *p = t->root;
  assert(p == nil || p->color == RBTREE_BLACK);

  init_color_traverse();
  assert(color_traverse(p, RBTREE_BLACK, 0, nil));
}

// rbtree should keep search tree and color constraints
void test_rb_constraints(const key_t arr[], const size_t n) {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(t->root != NULL);

  test_color_constraint(t);
  test_search_constraint(t);

  delete_rbtree(t);
}

// rbtree should manage distinct values
void test_distinct_values() {
  const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

// rbtree should manage values with duplicate
void test_duplicate_values() {
  const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

void test_minmax_suite() {
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_minmax(entries, n);
}

void test_to_array_suite() {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_to_array(t, entries, n);

  delete_rbtree(t);
}

void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(p != NULL);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(p != NULL);
    assert(p->key == arr[i]);
    rbtree_erase(t, p);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(p == NULL);
  }

  for (int i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(p != NULL);
    node_t *q = rbtree_find(t, arr[i]);
    assert(q != NULL);
    assert(q->key == arr[i]);
    assert(p == q);
    rbtree_erase(t, p);
    q = rbtree_find(t, arr[i]);
    assert(q == NULL);
  }
}

void test_find_erase_fixed() {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(arr) / sizeof(arr[0]);
  rbtree *t = new_rbtree();
  assert(t != NULL);

  test_find_erase(t, arr, n);

  delete_rbtree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed) {
  srand(seed);
  rbtree *t = new_rbtree();
  key_t *arr = calloc(n, sizeof(key_t));
  for (int i = 0; i < n; i++) {
    arr[i] = rand();
  }

  test_find_erase(t, arr, n);

  free(arr);
  delete_rbtree(t);
}

int main(void) {
  test_init();
  test_insert_single(1024);
  test_find_single(512, 1024);
  test_erase_root(128);
  test_find_erase_fixed();
  test_minmax_suite();
  // test_to_array_suite();
  test_distinct_values();
  test_duplicate_values();
  // test_multi_instance();
  test_find_erase_rand(10000, 17);
  printf("Passed all tests!\n");
}
