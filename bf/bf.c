#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 50000

int
main(int argc, char *argv[])
{
	size_t len = 0;
	int closed, opened, pos = 0;
	unsigned short *pc;
	char buf[BUFSIZE], *src;

	while (read(STDIN_FILENO, &buf[len], 1) > 0)
		len++;
	buf[len] = '\0';

	if ((src = malloc(len)) == NULL) {
		perror("malloc");
		exit(1);
	}
	strcpy(src, buf);
	memset(buf, 0, len);

	for (pc = (unsigned short *)buf; pos < len; pos++) {
		switch (src[pos]) {
		case '>':
			pc++;
			break;
		case '<':
			pc--;
			break;
		case '+':
			(*pc)++;
			break;
		case '-':
			(*pc)--;
			break;
		case '.':
			putchar(*pc);
			break;
		case ',':
			*pc = getchar();
			break;
		case '[':
			if (!(*pc)) {
				for (opened = 0, pos++; pos < len; pos++) {
					if (src[pos] == ']' && !opened)
						break;
					else if (src[pos] == '[')
						opened++;
					else if (src[pos] == ']')
						opened--;
				}
			}
			break;
		case ']':
			if (*pc) {
				for (closed = 0, pos--; pos >= 0; pos--) {
					if (src[pos] == '[' && !closed)
						break;
					else if (src[pos] == ']')
						closed++;
					else if (src[pos] == '[')
						closed--;
				}
			}
			break;
		}
	}
	free(src);

	return (0);
}
