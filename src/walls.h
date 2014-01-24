chtype wall_chars[0xFF];

void fill_walls(void) {
	int i = 0;
	
	wall_chars[i++] = ACS_BLOCK;
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = '?';
	wall_chars[i++] = '?';
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_PLUS;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_TTEE;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_URCORNER;
	wall_chars[i++] = ACS_RTEE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_LTEE;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_ULCORNER;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_VLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_BTEE;
	wall_chars[i++] = ACS_LLCORNER;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_HLINE;
	wall_chars[i++] = ACS_LRCORNER;
	wall_chars[i++] = ' ';
}
