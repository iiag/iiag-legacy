typedef struct {
	void(*command)(int, char**);
	int keyval;
} command_t;

void init_commands();
void execute();
