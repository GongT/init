#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>

#include "file-parser.h"
#include "signal-switch.h"

int validSig(char *sig)
{
	for (int c = strlen(sig) - 1; c >= 0; c--)
		sig[c] = toupper(sig[c]);
	int ret = signal_number(sig);
	if (!ret)
		die("Unknown signal: %s", sig);
	// fprintf(stderr, "signal: %s => %d\n", sig, ret);
	return ret;
}

int trim_char(const char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
char *trim(char *str)
{
	for (char *last = str + strlen(str) - 1; trim_char(*last); last--)
		*last = '\0';
	char *start;
	for (start = str; trim_char(*start); start++)
		;
	return start;
}

bool set_value(program_t *p, const char *key, char *value)
{
	if (0 == strcmp(key, "arg[]"))
	{
		p->command[p->__command_size] = value;
		p->command = srealloc(p->command, (++p->__command_size) * sizeof(char **));
		printf("    add argument: (%d)%s\n", p->__command_size, value);
		p->command[p->__command_size] = NULL;
		return true;
	}
	if (0 == strcmp(key, "signal"))
	{
		if (value[0] == 'S' && value[1] == 'I' && value[2] == 'G')
		{
			p->signal = validSig(value + 3);
		}
		else
		{
			p->signal = validSig(value);
		}
		printf("    stop signal: %s(%d)\n", value, p->signal);
		free(value);
		return true;
	}
	return false;
}

char *copy_new(const char *from, size_t len)
{
	if (!len)
		len = strlen(from);

	const char *start;
	for (start = from; trim_char(*start); start++)
		len--;
	for (const char *last = start + len - 1; trim_char(*last); last--)
		len--;
	return strndup(start, len);
}

void fail_parse_line(const char *line)
{
	die("can not parse line: %s\n", line);
}

program_list_t parse_file(const char *filepath)
{
	FILE *fp = fopen(filepath, "r");
	if (!fp)
		die("failed to open file %s", filepath);

	char *raw_line = NULL;
	size_t len = 0;
	ssize_t read = -1;
	program_t **ret = NULL, *curr = NULL;
	uint32_t count = 0;

	while ((read = getline(&raw_line, &len, fp)) != -1)
	{
		if (read == -1)
			break;
		if (read == 0)
			continue;
		char *trimed_line = trim(raw_line);
		uint32_t line_size = strlen(trimed_line);
		// printf(" + %d -> %d: '%s' => '%s'\n", read, line_size, raw_line, trimed_line);

		if (line_size == 0 || trimed_line[0] == '#')
			continue;

		if (trimed_line[0] == '[' && trimed_line[line_size - 1] == ']')
		{
			count++;
			ret = srealloc(ret, count * sizeof(void *));
			curr = ret[count - 1] = smalloc(sizeof(program_t));

			curr->command = smalloc(sizeof(char **));
			curr->__command_size = 0;
			curr->signal = 2;

			curr->title = copy_new(trimed_line + 1, line_size - 2);
			printf("program: %s\n", curr->title);
		}
		else
		{
			char *sSignPos = strchr(trimed_line, ':');
			if (sSignPos == NULL)
				fail_parse_line(trimed_line);
			*sSignPos = '\0';

			const char *key = trim(trimed_line);
			char *value = copy_new(trim(sSignPos + 1), 0);

			if (!set_value(curr, key, value))
			{
				fail_parse_line(trimed_line);
			}
		}
	}

	free(raw_line);
	fclose(fp);

	program_list_t r = {
		.size = count,
		.head = ret,
	};
	return r;
}
