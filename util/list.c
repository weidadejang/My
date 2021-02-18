#include "../include/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void zfree(void *ptr) {
  if (ptr == NULL) return;

  free(ptr);
}

void *zmalloc(size_t size) {
  void *ptr = malloc(size);

  if (!ptr) return NULL;

  return ptr;
}

list *listCreate(void) {
  struct list *list;

  if ((list = zmalloc(sizeof(*list))) == NULL) return NULL;
  list->head = list->tail = NULL;
  list->len = 0;
  list->dup = NULL;
  list->free = NULL;
  list->match = NULL;

  return list;
}

void listEmpty(list *list) {
  unsigned long len;
  listNode *current, *next;

  current = list->head;
  len = list->len;
  while (len--) {
    next = current->next;
    if (list->free) list->free(current->value);
    zfree(current);
    current = next;
  }
  list->head = list->tail = NULL;
  list->len = 0;
}

void listRelease(list *list) {
  listEmpty(list);
  zfree(list);
}

list *listAddNodeHead(list *list, void *value) {
  listNode *node;

  if ((node = zmalloc(sizeof(*node))) == NULL) return NULL;
  node->value = value;
  if (list->len == 0) {
    list->head = list->tail = node;
    node->prev = node->next = NULL;
  } else {
    node->prev = NULL;
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
  }
  list->len++;
  return list;
}

list *listAddNodeTail(list *list, void *value) {
  listNode *node;

  if ((node = zmalloc(sizeof(*node))) == NULL) return NULL;
  node->value = value;
  if (list->len == 0) {
    list->head = list->tail = node;
    node->prev = node->next = NULL;
  } else {
    node->prev = list->tail;
    node->next = NULL;
    list->tail->next = node;
    list->tail = node;
  }
  list->len++;
  return list;
}

list *listInsertNode(list *list, listNode *old_node, void *value, int after) {
  listNode *node;

  if ((node = zmalloc(sizeof(*node))) == NULL) return NULL;
  node->value = value;
  if (after) {
    node->prev = old_node;
    node->next = old_node->next;
    if (list->tail == old_node) {
      list->tail = node;
    }
  } else {
    node->next = old_node;
    node->prev = old_node->prev;
    if (list->head == old_node) {
      list->head = node;
    }
  }
  if (node->prev != NULL) {
    node->prev->next = node;
  }
  if (node->next != NULL) {
    node->next->prev = node;
  }
  list->len++;
  return list;
}

void listDelNode(list *list, listNode *node) {
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;
  if (list->free) list->free(node->value);
  zfree(node);
  list->len--;
}

void listRemoveNode(list *list, listNode *node) {
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;
  zfree(node);
  list->len--;
}

listIter *listGetIterator(list *list, int direction) {
  listIter *iter;

  if ((iter = zmalloc(sizeof(*iter))) == NULL) return NULL;
  if (direction == AL_START_HEAD)
    iter->next = list->head;
  else
    iter->next = list->tail;
  iter->direction = direction;
  return iter;
}

void listReleaseIterator(listIter *iter) { zfree(iter); }

void listRewind(list *list, listIter *li) {
  li->next = list->head;
  li->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *li) {
  li->next = list->tail;
  li->direction = AL_START_TAIL;
}

listNode *listNext(listIter *iter) {
  listNode *current = iter->next;

  if (current != NULL) {
    if (iter->direction == AL_START_HEAD)
      iter->next = current->next;
    else
      iter->next = current->prev;
  }
  return current;
}

list *listDup(list *orig) {
  list *copy;
  listIter iter;
  listNode *node;

  if ((copy = listCreate()) == NULL) return NULL;
  copy->dup = orig->dup;
  copy->free = orig->free;
  copy->match = orig->match;
  listRewind(orig, &iter);
  while ((node = listNext(&iter)) != NULL) {
    void *value;

    if (copy->dup) {
      value = copy->dup(node->value);
      if (value == NULL) {
        listRelease(copy);
        return NULL;
      }
    } else
      value = node->value;
    if (listAddNodeTail(copy, value) == NULL) {
      listRelease(copy);
      return NULL;
    }
  }
  return copy;
}

listNode *listSearchKey(list *list, void *key) {
  listIter iter;
  listNode *node;

  listRewind(list, &iter);
  while ((node = listNext(&iter)) != NULL) {
    if (list->match) {
      if (list->match(node->value, key)) {
        return node;
      }
    } else {
      if (key == node->value) {
        return node;
      }
    }
  }
  return NULL;
}

listNode *listIndex(list *list, long index) {
  listNode *n;

  if (index < 0) {
    index = (-index) - 1;
    n = list->tail;
    while (index-- && n) n = n->prev;
  } else {
    n = list->head;
    while (index-- && n) n = n->next;
  }
  return n;
}

void listRotate(list *list) {
  listNode *tail = list->tail;

  if (listLength(list) <= 1) return;

  /* Detach current tail */
  list->tail = tail->prev;
  list->tail->next = NULL;
  /* Move it as head */
  list->head->prev = tail;
  tail->prev = NULL;
  tail->next = list->head;
  list->head = tail;
}

void listJoin(list *l, list *o) {
  if (o->head) o->head->prev = l->tail;

  if (l->tail)
    l->tail->next = o->head;
  else
    l->head = o->head;

  if (o->tail) l->tail = o->tail;
  l->len += o->len;

  o->head = o->tail = NULL;
  o->len = 0;
}
