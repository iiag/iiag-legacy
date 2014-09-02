//
// introspection.c
//

#include <string.h>
#include <stdio.h>
#include "introspection.h"
#include "log.h"

#ifndef WITH_INTROSPECTION

void init_introspection(const char *prg_file)
{
	info("Introspection initialization with program %s", prg_file);
	info("Introspection not enabled; no symbol information will be shown.");
	debug("Compile with WITH_INTROSPECTION defined to enable.");
}

void get_location(void *loc, char *s, int sz)
{
	snprintf(s, sz, "<unknown>");
}

void *get_frame(int lev) { return (void *) 0; }

const char *resolve_string(int idx) { return ""; }

#else

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <stdlib.h>
#include <errno.h>

//Change this between 32/64-bit headers
//TODO: Is there a way to do this at compile-time?

#if defined(_32BIT)

#define ElfN(tp) Elf32_##tp
#define GET_BASEPOINTER() asm("mov %ebp, __ebp")

#elif defined(_64BIT)

#define ElfN(tp) Elf64_##tp
#define GET_BASEPOINTER() asm("mov %rbp, __ebp")

#else
#error WITH_INTROSPECTION: Must define _32BIT or _64BIT
#endif

static void *exec_map=NULL;
static ElfN(Sym) *symtab=NULL;
static ElfN(Shdr) *symsec=NULL;
static ElfN(Shdr) *strsec=NULL;
static char *strtab=NULL;
static unsigned long *__ebp;

void init_introspection(const char *prg_file)
{
	int exec_fd, i;
	struct stat stat_buf;
	ElfN(Ehdr) *header;
	ElfN(Shdr) *sections;
	ElfN(Shdr) *secstrsec;
	char *secstrtbl;
	
	info("Introspection initialization with program %s", prg_file);
	
	//Create a memory map of our own executable (argv[0])
	if(stat(prg_file, &stat_buf)) {
		warning("Failed to stat program file; no symbol information will be available.");
		debug("Error: %s", strerror(errno));
		return;
	}
	exec_fd = open(prg_file, O_RDONLY);
	if(exec_fd < 0) {
		warning("Failed to open program file for debugging; no symbol information will be available.");
		debug("Error: %s", strerror(errno));
		return;
	}
	exec_map = mmap(NULL, stat_buf.st_size, PROT_READ, MAP_SHARED, exec_fd, 0);
	close(exec_fd);
	if(exec_map == MAP_FAILED) {
		warning("Failed to map program file; no symbol information will be available.");
		debug("Error: %s", strerror(errno));
		return;
	}
	
	//Initialize the ELF headers
	header = ((ElfN(Ehdr) *) exec_map);
	sections = ((ElfN(Shdr) *) ((char *) exec_map + header->e_shoff));
	
	
	//Search for a symbol string table--first find the section string table
	secstrsec = (sections + header->e_shstrndx);
	secstrtbl = ((char *) exec_map + secstrsec->sh_offset);
	
	//Search for a the sections section
	symtab=NULL;
	for(i = 0; i < header->e_shnum; i++) {
		if(sections[i].sh_type == SHT_SYMTAB) {
			symsec = (sections + i);
			symtab = ((ElfN(Sym) *) ((char *) exec_map + symsec->sh_offset));
		}
		if(sections[i].sh_type == SHT_STRTAB
			&& !strncmp((secstrtbl + sections[i].sh_name), ".strtab", 7)) {
			strsec = (sections + i);
			strtab = ((char *) exec_map + strsec->sh_offset);
		}
	}
	if(!symtab) {
		warning("Could not locate a symbol table; no symbol information will be available.");
		munmap(exec_map, stat_buf.st_size);
		return;
	}
	
	info("Successfully loaded debugging symbol information.");
}

void get_location(void *loc, char *s, int sz) {
	const char *symname="<unknown symbol>";
	int i, offset=-1;
	int entries;
	ElfN(Sym) *entry;
	
	if(!symtab) {
		snprintf(s, sz, "<pre-init; location %p>", loc);
		return;
	}
	
	entries = (symsec->sh_size / symsec->sh_entsize);
	
	for(i = 0; i < entries; i++) {
		entry = (symtab + i);
		if(loc >= entry->st_value && loc < (entry->st_value + entry->st_size)) {
			if(entry->st_name != 0) {
				symname = resolve_string(entry->st_name);
			} else {
				symname = "<unnamed symbol>";
			}
			offset = ((int) loc - entry->st_value);
			break;
		}
	}
	
	if(offset < 0) {
		snprintf(s, sz, "<unknown location %p>", loc);
	} else {
		snprintf(s, sz, "<%p> %s+%x", loc, symname, offset);
	}
}

void *get_frame(int lev) {
	int i;
	
	GET_BASEPOINTER();
	
	//Starting at level -1 because we're inside a function call already
	//...or not
	for(i = 0; i < lev; i++) {
		__ebp = *((unsigned int **)__ebp);
	}
	
	return ((void *) __ebp[1]);
}

const char *resolve_string(int idx) {
	return strtab + idx;
}

#endif