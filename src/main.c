#include "todo-dep.h"

int todo_add(struct todo_env *env) {}
int todo_check(struct todo_env *env) {}
int todo_create(struct todo_env *env) {}
int todo_dependency(struct todo_env *env) {}
int todo_select(struct todo_env *env) {}
int todo_status(struct todo_env *env) {}
int todo_toggle(struct todo_env *env) {}
int todo_trigger(struct todo_env *env) {}

int todo_help(struct todo_env *env)
{
	return 0;
}

int main(int argc, char **argv)
{
	static const struct {
		const char *name;
		int (*func)(struct todo_env *env);
	} commands[] = {
		{ "add", todo_add },
		{ "check", todo_check },
		{ "create", todo_create },
		{ "dependency", todo_dependency },
		{ "select", todo_select },
		{ "status", todo_status },
		{ "toggle", todo_toggle },
		{ "trigger", todo_trigger },
	};
	const char *home;
	struct todo_env env;
	struct todo_list *list;

	home = getenv("HOME");
	if (!home) {
		fprintf(stderr, "environment variable HOME is not set\n");
		return -1;
	}
	env.home = malloc(strlen(home) + 1 + 4 + 1);
	if (!env.home) {
		fprintf(stderr, "initalization failed: out of memory\n");
		return -1;
	}
	sprintf(env.home, "%s/.todo", home);
	list = todolist_read(&env, "todo.txt");
	if (list == NULL)
		return -1;
	if (argc == 1) {
		todo_help(&env);
	} else {
		for(int i = 0; i < (int) ARRLEN(commands); i++)
			if (!strcmp(commands[i].name, argv[1])) {
				env.argc = argc - 2;
				env.argv = argv + 2;
				return commands[i].func(&env);
			}
		fprintf(stderr,
			"the commmand '%s' doesn't exist\n"
				"type `%s help` for more information\n",
			argv[1], argv[0]);
		return -1;	
	}
	return 0;
}

