#include "todo-dep.h"

enum {
	E_OUTOFMEMORY,
	E_INCONSISTENTINDENT,
	E_MISSINGMINUS,
	E_MISSINGOPENBRACKET,
	E_MISSINGCLOSEBRACKET,
	E_MISSINGSPACE,
	E_INVALIDFLAGS,
};

static const char *error_messages[] = {
	"out of memory",
	"inconsistent indentation",
	"missing '-'",
	"missing '['",
	"missing ']'",
	"missing space inbetween '[' and ']'",
	"invalid flags",
};

struct todo_list_parser {
	struct todo_env *env;
	FILE *fp;
	int c;
	int indent;
	char *line;
	size_t nLine;
	size_t iLine;
	size_t lines;
};

static int parser_setup(struct todo_env *env,
		struct todo_list_parser *parser, const char *name)
{
	memset(parser, 0, sizeof(*parser));
	parser->env = env;
	snprintf(env->path, sizeof(env->path), "%s/%s",
		env->home, name);
	if ((parser->fp = fopen(env->path, "r")) == NULL) {
		fprintf(stderr, "E: file '%s' doesn't exist\n", env->path);
		return -1;
	}
	return 0;
}

static int parser_error(struct todo_list_parser *parser, int err)
{
	fprintf(stderr, "E%d at line %zu: %s\n",
		err, parser->lines, error_messages[err]);
	fprintf(stderr, "%s\n", parser->line, parser->iLine, "~");
	for (size_t i = 0; i < parser->iLine; i++)
		fputc('~', stderr);
	fputs("^\n", stderr);
	return -1;
}

static int parser_fatal(struct todo_list_parser *parser, int err)
{
	free(parser->line);
	fclose(parser->fp);
	fprintf(stderr, "E%d: %s\n", err, error_messages[err]);
	return -1;

}

static int parser_getline(struct todo_list_parser *parser)
{
	FILE *const fp = parser->fp;
	char *newLine;
	int c;
	/* number of half indent steps */
	int halfIndent = 0;

	parser->iLine = 0;
	parser->nLine = 0;
	do {
		if (feof(fp))
			return 1;
		while ((c = fgetc(fp)) != EOF) {
			if (parser->nLine == 0) {
				if (c == ' ') {
					halfIndent++;
					continue;
				}
				if (c == '\t') {
					halfIndent += 2;
					continue;
				}
			}
			newLine = realloc(parser->line, parser->nLine + 1);
			if (!newLine)
				return parser_fatal(parser, E_OUTOFMEMORY);
			parser->line = newLine;
			if (c == '\n') {
				if(parser->nLine == 0)
					continue;
				parser->line[parser->nLine++] = 0;
				if (halfIndent & 1)
					parser_error(parser,
						E_INCONSISTENTINDENT);
				parser->indent = halfIndent / 2;
				break;
			}
			parser->line[parser->nLine++] = c;
		}
	} while (parser->nLine == 0);
	parser->lines++;
	return 0;
}

char parser_getchar(struct todo_list_parser *parser)
{
	if (parser->line[parser->iLine] == '\0')
		return 0;
	return parser->line[parser->iLine++];
}

int parser_assert(struct todo_list_parser *parser, char ch, int err)
{
	if (parser->line[parser->iLine] != ch)
		return parser_error(parser, err);
	parser->iLine++;
	return 0;
}

int parser_skipspace(struct todo_list_parser *parser)
{
	while (isblank(parser->line[parser->iLine]))
		parser->iLine++;
	return 0;
}

struct todo_list *todolist_read(struct todo_env *env, const char *name)
{
	struct todo_list_parser parser;
	int err;
	struct todo_list *list, *newList;
	size_t szList;
	char ch;
	int flags;
	struct todo_entry *entry;

	if (parser_setup(env, &parser, name) != 0)
		return NULL;
	list = malloc(sizeof(*list));
	szList = sizeof(*list);
	strcpy(list->name, name);
	list->nEntries = 0;
	while ((err = parser_getline(&parser)) == 0) {
		if (parser_assert(&parser, '-', E_MISSINGMINUS) != 0)
			continue;
		parser_skipspace(&parser);
		if (parser_assert(&parser, '[', E_MISSINGOPENBRACKET) != 0)
			continue;
		ch = parser_getchar(&parser);
		switch (ch) {
		case ']':
			parser_error(&parser, E_MISSINGSPACE);
			continue;
		case 'X':
			flags = 1;
			break;
		case ' ':
			flags = 0;
			break;
		default:
			parser_error(&parser, E_INVALIDFLAGS);
			continue;
		}
		parser_skipspace(&parser);
		if (parser_assert(&parser, ']', E_MISSINGOPENBRACKET) != 0)
			continue;
		newList = realloc(list,
				szList + sizeof(struct todo_entry) +
				parser.nLine - parser.iLine + 1);
		if (!newList) {
			parser_fatal(&parser, E_OUTOFMEMORY);
			free(list);
			return NULL;
		}
		list = newList;
		list->nEntries++;
		entry = (struct todo_entry*) (list + szList);
		entry->flags = flags;
		entry->indent = parser.indent;
		entry->nText = parser.nLine - parser.iLine + 1;
		memcpy(entry->text, parser.line, entry->nText);
		entry->text[entry->nText] = 0;
		szList += sizeof(struct todo_entry) + entry->nText;
	}
	return err < 0 ? NULL : list;
}
