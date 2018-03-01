/**
 * cs_list.h
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_LIST_H_INCLUDED_
#define _CS_LIST_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cs_list_node_s {
	void *data;
	struct cs_list_node_s *prev, *next;
} cs_list_node_t;
 
typedef struct cs_list_s {
	cs_list_node_t *head, *tail;
} cs_list_t;

#define cs_list_head(list)                    (list)->head
#define cs_list_for_each(list, pos)           for (\
  pos = (list)->head; (pos) != NULL; pos = (pos)->next)
#define cs_list_for_each_safe(list, pos, tmp)	for ( \
  pos = (list)->head, tmp = pos; (pos) != NULL; pos = (tmp)->next, tmp = pos)

#define list_for_each(pos, head)              for (pos = (head)->next; \
	pos != (head); pos = pos->next)
#define list_entry(ptr, T, member)            cs_container_of(ptr, T, member)
#define list_for_each_safe(pos, n, head)      for (pos = (head)->next, \
	n = pos->next; pos != (head); pos = n, n = pos->next)

CS_API void cs_list_init(cs_list_t *list); 
CS_API void cs_list_add_head(cs_list_t *list, cs_list_node_t *node);
CS_API void cs_list_add_tail(cs_list_t *list, cs_list_node_t *node);
CS_API void cs_list_add_before(
  cs_list_t *list, cs_list_node_t *old, cs_list_node_t *node);
CS_API void cs_list_add_after(
  cs_list_t *list, cs_list_node_t *old, cs_list_node_t *node);
CS_API void cs_list_del(cs_list_t *list, cs_list_node_t *node);

CS_API cs_list_node_t *cs_list_find_u32(cs_list_t *list, cs_uint_t key);
CS_API cs_list_node_t *cs_list_find_u64(cs_list_t *list, cs_uint64_t key);

#ifdef __cplusplus
}
#endif /* C++ support */
#endif