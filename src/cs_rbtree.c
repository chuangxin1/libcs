/**
 * cs_rbtree.c
 * Copyright (c) 2007-2018 ls
 **/
#include "../inc/cs.h"
#include "../inc/cs_rbtree.h"

CS_API void cs_rbtree_init(
  cs_rbtree_t *t, cs_rbtree_node_t *nil, int (*cmp)(void *, void *)) {
	CS_TREE_RB_BLACK(nil);
	nil->key = 0;
	nil->parent = nil->left = nil->right = nil;
	
	t->nil = nil;
	t->root = nil;

	t->cmp = cmp;
}

static cs_inline void cs_rbtree_left_rotate(
  cs_rbtree_t *t, cs_rbtree_node_t *x) {
	cs_rbtree_node_t *y;
	cs_rbtree_node_t *nil = t->nil;

	y = x->right;
	x->right = y->left;

	if (y->left != nil) {
		y->left->parent = x; 
	}
  
	y->parent = x->parent;   

	if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

static cs_inline void cs_rbtree_right_rotate(
  cs_rbtree_t *t, cs_rbtree_node_t *x) {
	cs_rbtree_node_t *y;
	cs_rbtree_node_t *nil = t->nil;
	
	y = x->left;
	x->left = y->right;
	
	if (nil != y->right) {
		y->right->parent = x; 
	}
	y->parent = x->parent;
	if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->right = x;
	x->parent = y;
}

CS_API cs_rbtree_node_t *cs_rbtree_add(cs_rbtree_t *t, cs_rbtree_node_t *n) {
	cs_rbtree_node_t *x;
	cs_rbtree_node_t *y;
	cs_rbtree_node_t *nil = t->nil;

	n->left = n->right = nil;
	y = t->root;
	x = t->root->left;
	
	while (x != nil) {
		if (CS_TREE_RB_EQ(t, n->key, x->key)) {
			return (x);
		}
		y = x;
		x = CS_TREE_RB_LT(t, n->key, x->key) ? x->left : x->right;
		if (x->key == nil->key) {
			break;
		}
	}
	n->parent = y;
	if ((y == t->root) || CS_TREE_RB_LT(t, n->key, y->key)) {
		y->left = n;
	} else {
		y->right = n;
	}

	CS_TREE_RB_RED(n);
	while (CS_TREE_RB_ISRED(n->parent)) { 
		/* use sentinel instead of checking for root */
		//printf("CS_TREE_RB_ISRED:%08x\n",n->parent->key);
		if (n->parent == n->parent->parent->left) {
			y = n->parent->parent->right;
			if (CS_TREE_RB_ISRED(y)) {
				CS_TREE_RB_BLACK(n->parent);
				CS_TREE_RB_BLACK(y);
				CS_TREE_RB_RED(n->parent->parent);
				n = n->parent->parent;
			} else {
				if (n == n->parent->right) {
					n = n->parent;
					cs_rbtree_left_rotate(t, n);
				}
				CS_TREE_RB_BLACK(n->parent);
				CS_TREE_RB_RED(n->parent->parent);
				cs_rbtree_right_rotate(t, n->parent->parent);
			} 
		} else {
			y = n->parent->parent->left;
			if (CS_TREE_RB_ISRED(y)) {
				CS_TREE_RB_BLACK(n->parent);
				CS_TREE_RB_BLACK(y);
				CS_TREE_RB_RED(n->parent->parent);
				n = n->parent->parent;
			} else {
				if (n == n->parent->left) {
					n = n->parent;
					cs_rbtree_right_rotate(t, n);
				}
				CS_TREE_RB_BLACK(n->parent);
				CS_TREE_RB_RED(n->parent->parent);
				cs_rbtree_left_rotate(t, n->parent->parent);
			} 
		}
	}
 	CS_TREE_RB_BLACK(t->root->left);
	
	return (NULL);
}

static cs_inline void cs_rbtree_deletefixup(
  cs_rbtree_t *t, cs_rbtree_node_t *x) {
	cs_rbtree_node_t *root = t->root->left;
	cs_rbtree_node_t *w;
	
	while (CS_TREE_RB_ISBLACK(x) && (root != x)) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (CS_TREE_RB_ISRED(w)) {
				CS_TREE_RB_BLACK(w);
				CS_TREE_RB_RED(x->parent);
				cs_rbtree_left_rotate(t, x->parent);
				w = x->parent->right;
			}
			if (CS_TREE_RB_ISBLACK(w->right) && CS_TREE_RB_ISBLACK(w->left)) { 
				CS_TREE_RB_RED(w);
				x = x->parent;
			} else {
				if (CS_TREE_RB_ISBLACK(w->right)) {
					CS_TREE_RB_BLACK(w->left);
					CS_TREE_RB_RED(w);
					cs_rbtree_right_rotate(t, w);
					w = x->parent->right;
				}
				CS_TREE_RB_CPYCOLOR(w, x->parent);
				CS_TREE_RB_BLACK(x->parent);
				CS_TREE_RB_BLACK(w->right);
				cs_rbtree_left_rotate(t, x->parent);
				x = root;
			}
		} else {
			w = x->parent->left;
			if(CS_TREE_RB_ISRED(w)) {
				CS_TREE_RB_BLACK(w);
				CS_TREE_RB_RED(x->parent);
				cs_rbtree_right_rotate(t, x->parent);
				w = x->parent->left;
			}
			if (CS_TREE_RB_ISBLACK(w->right) && CS_TREE_RB_ISBLACK(w->left)) { 
	  			CS_TREE_RB_RED(w);
	  			x = x->parent;
			} else {
				if (CS_TREE_RB_ISBLACK(w->left)) {
					CS_TREE_RB_BLACK(w->right);
					CS_TREE_RB_RED(w);
					cs_rbtree_left_rotate(t, w);
					w = x->parent->left;
				}
				CS_TREE_RB_CPYCOLOR(w, x->parent);
				CS_TREE_RB_BLACK(x->parent);
				CS_TREE_RB_BLACK(w->left);
				cs_rbtree_right_rotate(t, x->parent);
				x = root;
			}
		}
	}
	CS_TREE_RB_BLACK(x);
}

static cs_inline cs_rbtree_node_t *cs_rbtree_successor(
  cs_rbtree_t *t, cs_rbtree_node_t *n) {
	cs_rbtree_node_t *y;
	cs_rbtree_node_t *nil = t->nil;
	cs_rbtree_node_t *root = t->root;

	if (nil != (y = n->right)) {
		while (y->left != nil) {
			y = y->left;
		}
	} else {
		y = n->parent;
		while (n == y->right) {
			n = y;
			y = y->parent;
		}
		if (y == root){
			return (nil);
		}
	}

	return (y);
}

CS_API void cs_rbtree_del(cs_rbtree_t *t, cs_rbtree_node_t *n) {
	cs_rbtree_node_t *y;
	cs_rbtree_node_t *x;
	cs_rbtree_node_t *nil = t->nil;
	cs_rbtree_node_t *root = t->root;
	
	y = ((n->left == nil) || (n->right == nil)) 
    ? n : cs_rbtree_successor(t, n);
	x = (y->left == nil) ? y->right : y->left;
	if (root == (x->parent = y->parent)) {
		root->left = x;
	} else {
		if (y == y->parent->left) {
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	}
	if (y != n) {
		if(CS_TREE_RB_ISBLACK(y)){
			cs_rbtree_deletefixup(t, x);
		}	  
		
		y->left = n->left;
		y->right = n->right;
		y->parent = n->parent;
		CS_TREE_RB_CPYCOLOR(y, n);
		n->left->parent = n->right->parent = y;
		if (n == n->parent->left) {
			n->parent->left = y;
		} else {
			n->parent->right = y;
		}
	} else {
		if (CS_TREE_RB_ISBLACK(y)) {
			cs_rbtree_deletefixup(t, x);
		}
	}
}

static void cs_rbtree_order_internal(
  cs_rbtree_t *t, 
  cs_rbtree_node_t *n, 
  void (*cs_rbtree_callback)(cs_rbtree_node_t *)) {
	if (n == t->nil) {
		return;
	}
	cs_rbtree_order_internal(t, n->left, cs_rbtree_callback);
	cs_rbtree_callback(n);
	cs_rbtree_order_internal(t,n->right, cs_rbtree_callback);
}

CS_API void cs_rbtree_order(
  cs_rbtree_t *t, 
  void (*cs_rbtree_callback)(cs_rbtree_node_t *)) {
	cs_rbtree_order_internal(t, t->root->left, cs_rbtree_callback);
}

CS_API cs_rbtree_node_t *cs_rbtree_query(cs_rbtree_t *t, void *k) {
	cs_rbtree_node_t *x = t->root->left;
	cs_rbtree_node_t *nil = t->nil;

	if (x == nil) {
		return (NULL);
	}
	while (!CS_TREE_RB_EQ(t, k, x->key)) {
		if (CS_TREE_RB_LT(t, k, x->key)) {
			x = x->left;
		} else {
			x = x->right;
		}
		if (x->key == nil->key) {
			return (NULL);
		}
	}
	
  return (x);
}

CS_API cs_rbtree_node_t *cs_rbtree_min(cs_rbtree_t *t) {
	cs_rbtree_node_t *n = t->root;

	while (n->left != t->nil) {
    n = n->left;
		if (n->key == t->nil->key) {
			return (NULL);
		}
  }
	
  return (n);
}

CS_API cs_rbtree_node_t *cs_rbtree_max(cs_rbtree_t *t) {
	cs_rbtree_node_t *n = t->root->left;
	
	while (n->right != t->nil) {
    n = n->right;
		if (n->key == t->nil->key) {
			return (NULL);
		}
  }
	
  return (n);
}