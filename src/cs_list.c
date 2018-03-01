/**
 * cs_str.c
 * Copyright (c) 2007-2018 ls
 **/
#include "../inc/cs.h"
#include "../inc/cs_list.h"

CS_API void cs_list_init(cs_list_t *list) {
	list->head = list->tail = NULL;
}

CS_API void cs_list_add_head(cs_list_t *list, cs_list_node_t *node) {
	if (list->head == NULL) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
	} else {
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
}

CS_API void cs_list_add_tail(cs_list_t *list, cs_list_node_t *node) {
	if (list->head == NULL) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
	} else {
		node->prev = list->tail;
		node->next = NULL;
		list->tail->next = node;
		list->tail = node;
	}
}

CS_API void cs_list_add_before(
  cs_list_t *list, cs_list_node_t *old, cs_list_node_t *node) {
	node->next = old;
	node->prev = old->prev;
	if (list->head == old) {
		list->head = node;
	}
	if (node->prev != NULL) {
		node->prev->next = node;
	}
	if (node->next != NULL) {
		node->next->prev = node;
	}
}

CS_API void cs_list_add_after(
  cs_list_t *list, cs_list_node_t *old, cs_list_node_t *node) {
	node->prev = old;
	node->next = old->next;
	if (list->tail == old) {
		list->tail = node;
	}

	if (node->prev != NULL) {
		node->prev->next = node;
	}

	if (node->next != NULL) {
		node->next->prev = node;
	}	
}

CS_API void cs_list_del(cs_list_t *list, cs_list_node_t *node) {
	if (node->prev == NULL) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if (node->next == NULL) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}
}

CS_API cs_list_node_t *cs_list_find_u32(cs_list_t *list, cs_uint_t key) {
	cs_list_node_t *node = NULL;
	cs_uint_t *u;

	cs_list_for_each(list, node) {
		u = (cs_uint_t *)node->data;
		if (*u == key) {
			return (node);
		}
	}

	return (NULL);
}

CS_API cs_list_node_t *cs_list_find_u64(cs_list_t *list, cs_uint64_t key) {
	cs_list_node_t *node = NULL;
	cs_uint64_t *u;

	cs_list_for_each(list, node) {
		u = (cs_uint64_t *)node->data;
		if (*u == key) {
			return (node);
		}
	}
	
	return (NULL);
}
