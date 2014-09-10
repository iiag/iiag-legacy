//
// io/nogr/input.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "../../controls.h"

const char * nogr_name_from_key(int key)
{
	static char str[9] = {0};
	memcpy(str, &key, sizeof(key));
	return str;
}

int nogr_key_from_name(const char * str)
{
	int k = 0, shf = 0;
	while (*str) {
		k |= *str << shf;
		shf += 8;
		str++;
	}
	return k;
}

int nogr_get_key(void)
{
	printf("? ");
	return getchar();
}

// this doesn't actually prompt for input
int nogr_get_ctrl(void)
{
	return controls[CTRL_COMMAND].key;
}

int nogr_prompt_dir(const char * prompt, int * px, int * py)
{
	//TODO
	*px = 1;
	*py = 0;
	return 0;
}

int nogr_prompt_inv(const char * prompt, inventory * inv, creature * c)
{
	//TODO
	return 0;
}

void nogr_prompt_equipped(const char * prompt, creature * c)
{
	//TODO
}

char * nogr_prompt_string(const char * prompt)
{
	char * tmp = malloc(256);
	fputs(prompt, stdout);
	fgets(tmp, 256, stdin);
	return tmp;
}
