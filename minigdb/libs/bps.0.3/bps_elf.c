/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_elf.c - elf functions for bps
**
** Copyright (C) 2015  Kevin Sheldrake
**
** This file is part of bps.
**
** Bps is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
*/

#include "bps_elf.h"

/*
** elf functions
*/

/* readsyms reads the function names from the elf executable */
int readsyms(struct symbol **symbols, char *filename, int display)
{
	int fd;
	struct stat *stats;
	Elf *elf = NULL;				/* Our Elf pointer for libelf */
	Elf_Scn *scn = NULL;		    /* Section Descriptor */
	Elf_Data *edata = NULL; 		/* Data Descriptor */
	GElf_Sym sym;					/* Symbol */
	GElf_Shdr shdr;					/* Section Header */
	int num_symbols = 0;
	int symbol_count = 0;
	int sym_index = 0;
	int i;

	unsigned char *base_ptr = NULL;

	if (!symbols || !filename) {
		myprintf(0, "readsyms: invalid parameters\n");
		exit(E_ARGS);
	}
	
	stats = malloc(sizeof(struct stat));
	if (stats == NULL) {
		myprintf(0, "readsyms malloc error\n");
		exit(E_MALLOC);
	}

	if ((fd = open(filename, O_RDONLY)) == ERR)
	{
	        myprintf(0, "couldn't open %s\n", filename);
	        exit(E_ARGS);
	}

	if ((fstat(fd, stats)))
	{
	        myprintf(0, "could not fstat %s\n", filename);
	        close(fd);
	        exit(E_ARGS);
	}

	if ((base_ptr = (unsigned char *) malloc(stats->st_size)) == NULL)
	{
		myprintf(0, "readsyms malloc error\n");
		exit(E_MALLOC);
	}

	if ((read(fd, base_ptr, stats->st_size)) < stats->st_size)
	{
		myprintf(0, "could not read file");
		free(base_ptr);
		exit(E_ARGS);
	}

	free(base_ptr);

	/* Check libelf version first */
	if (elf_version(EV_CURRENT) == EV_NONE)
	{
		myprintf(0, "WARNING Elf Library is out of date!\n");
	}

	elf = elf_begin(fd, ELF_C_READ, NULL);  /* Initialize 'elf' pointer to our file descriptor */

	/* empty the symbol array */
	if (display) {
		*symbols = NULL;
		num_symbols = 0;
		sym_index = 0;
	}

	/* Iterate through section headers and stop when we find symbols */
	while((scn = elf_nextscn(elf, scn)) != NULL)
	{
		gelf_getshdr(scn, &shdr);

		/* When we find a section header marked SHT_SYMTAB stop and get symbols */
		if(shdr.sh_type == SHT_SYMTAB)
		{
			/* edata points to our symbol table */
			edata = elf_getdata(scn, edata);

			/* how many symbols are there? this number comes from the size of
			   the section divided by the entry size */
			symbol_count = shdr.sh_size / shdr.sh_entsize;

			/* create or extend the array of symbols */
			if (!display) {
				if (*symbols == NULL) {
					/* create array */
					*symbols = (struct symbol *)malloc(symbol_count * sizeof(struct symbol));
					if (*symbols == NULL) {
						myprintf(0, "readsyms malloc error\n");
						exit(E_MALLOC);
					}
				} else {
					/* extend array */
					*symbols = (struct symbol *)realloc(*symbols, (symbol_count + num_symbols) * sizeof(struct symbol));
					if (*symbols == NULL) {
						myprintf(0, "readsyms realloc error\n");
						exit(E_MALLOC);
					}
					sym_index = num_symbols;
				}
			}

			/* loop through to grab all symbols */
			for(i = 0; i < symbol_count; i++)
			{					  
				/* libelf grabs the symbol data using gelf_getsym() */
				gelf_getsym(edata, i, &sym);

				/* only care about functions */
				if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC)
				{

					/* display == 1 -> print out */
					if (display) {

						/* print out the address */
						myprintf(0, "0x%x ", (sym.st_value));
		
						/* type of symbol binding */
						switch(ELF32_ST_BIND(sym.st_info))
						{
							case STB_LOCAL: myprintf(0, "LOCAL"); break;
							case STB_GLOBAL: myprintf(0, "GLOBAL"); break;
							case STB_WEAK: myprintf(0, "WEAK"); break;
							case STB_NUM: myprintf(0, "NUM"); break;
							case STB_LOOS: myprintf(0, "LOOS"); break;
							case STB_HIOS: myprintf(0, "HIOS"); break;
							case STB_LOPROC: myprintf(0, "LOPROC"); break;
							case STB_HIPROC: myprintf(0, "HIPROC"); break;
							default: myprintf(0, "UNKNOWN"); break;
						}

						myprintf(0, "\t");

						/* the name of the symbol is somewhere in a string table
						   we know which one using the shdr.sh_link member
						   libelf grabs the string using elf_strptr() */
						myprintf(0, "%s\n", elf_strptr(elf, shdr.sh_link, sym.st_name));

					} else {

						/* store the symbol */
						strncpy((*symbols)[sym_index].name, elf_strptr(elf, shdr.sh_link, sym.st_name), S_SYMNAME);
						(*symbols)[sym_index].name[S_SYMNAME - 1] = 0x00;
						(*symbols)[sym_index].address = sym.st_value;
						sym_index++;

					}
				}
			}
			/* update symbol count */
			if (!display) {
				num_symbols = sym_index;
				*symbols = (struct symbol *)realloc(*symbols, sym_index * sizeof(struct symbol));
			}

		}
	}

	return num_symbols;

}


/* display symbols */
void display_symbols(struct symbol *symbols, int total)
{
	int i;

	if (!symbols) {
		myprintf(0, "display_symbols: no symbols\n");
		return;
	}
	
	for (i=0; i<total; i++) {
		myprintf(0, "%p %s\n", (void *)(symbols[i].address), symbols[i].name);
	}

	myprintf(0, "\n");
}


/* symaddr returns the symbol address from the symbol table */
unsigned long symaddr(struct symbol *symbols, int total, char *name)
{
	int i;

	if (!symbols || !name) {
		myprintf(0, "symaddr: invalid parameters\n");
		return 0;
	}
	
	for (i=0; i<total; i++) {
		if (strcmp(symbols[i].name, name) == 0) {
			return symbols[i].address;
		}
	}

	return 0;
}


