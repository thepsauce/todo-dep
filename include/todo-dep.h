#ifndef INCLUDED_TODO_DEP_H
#define INCLUDED_TODO_DEP_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ARRLEN(a) (sizeof(a)/sizeof*(a))
#define TAB_WIDTH 2

struct todo_env {
	char *home;
	char path[4096];
	int argc;
	char **argv;
};

int todo_add(struct todo_env *env);
int todo_check(struct todo_env *env);
int todo_create(struct todo_env *env);
int todo_dependency(struct todo_env *env);
int todo_select(struct todo_env *env);
int todo_status(struct todo_env *env);
int todo_toggle(struct todo_env *env);
int todo_trigger(struct todo_env *env);

struct todo_entry {
	int flags;
	int indent;
	unsigned nText;
	char text[0];
};

struct todo_list {
	char name[255];
	unsigned nEntries;
	char entries[0];
};

struct todo_list *todolist_read(struct todo_env *env, const char *name);

#endif
