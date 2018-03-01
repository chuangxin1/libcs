/**
 * cs_rbtree.h
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_RBTREE_H_INCLUDED_
#define _CS_RBTREE_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cs_rbtree_node_s {
	void *key;
	void *data;

	unsigned char color;	/* if color=0 then the node is black */

	struct cs_rbtree_node_s *left, *right, *parent;
} cs_rbtree_node_t;

typedef struct cs_rbtree_s {
	cs_rbtree_node_t *root, *nil;

	int (*cmp)(void *d, void *s);
} cs_rbtree_t;

#define CS_TREE_RB_RED(n)        ((n)->color = 1)
#define CS_TREE_RB_BLACK(n)      ((n)->color = 0)
#define CS_TREE_RB_ISRED(n)      ((n)->color)
#define CS_TREE_RB_ISBLACK(n)    (!CS_TREE_RB_ISRED(n))
#define CS_TREE_RB_CPYCOLOR(d,s) ((d)->color = (s)->color)

#define CS_TREE_RB_ISNIL(n)      (!CS_TREE_RB_ISRED(n))

#define CS_TREE_RB_LT(t, a, b)   ((t)->cmp((a), (b)) < 0)  // ((a) < (b))
#define CS_TREE_RB_GT(t, a, b)   ((t)->cmp((a), (b)) > 0)  // ((a) > (b))
#define CS_TREE_RB_EQ(t, a, b)   ((t)->cmp((a), (b)) == 0) // ((a) == (b))

CS_API void cs_rbtree_init(
  cs_rbtree_t *t, cs_rbtree_node_t *nil, int (*cmp)(void *, void *));

CS_API cs_rbtree_node_t *cs_rbtree_add(cs_rbtree_t *t, cs_rbtree_node_t *n);
CS_API void cs_rbtree_del(cs_rbtree_t *t, cs_rbtree_node_t *n);

CS_API cs_rbtree_node_t *cs_rbtree_query(cs_rbtree_t *t, void *k);
CS_API cs_rbtree_node_t *cs_rbtree_min(cs_rbtree_t *t);
CS_API cs_rbtree_node_t *cs_rbtree_max(cs_rbtree_t *t);

CS_API void cs_rbtree_order(
  cs_rbtree_t *t, 
  void (*cs_rbtree_callback)(cs_rbtree_node_t *));

#ifdef __cplusplus
}
#endif /* C++ */
#endif
