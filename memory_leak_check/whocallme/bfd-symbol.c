/*******************************************
 * File Name : bfd-symbol.c
 * Purpose :
 * Creation Date : 12-09-2016
 * Last Modified : Mon 12 Sep 2016 07:45:09 PM CST
 * Created By : Mark Tseng  
 **********************************************/
/*test2.c only for alert7 test*/
#include <stdio.h>
#include <stdlib.h>
#include <bfd.h>
#include <strings.h>
#include <linux/elf.h>

#define nonfatal(s) {perror(s); return;}
#define fatal(s) {perror(s); exit(-1);}
#define bfd_nonfatal(s) {bfd_perror(s); return;}
#define bfd_fatal(s) {bfd_perror(s); exit(-1);}

main(int argc, char *argv[])
{
	bfd *ibfd;
	char *filename;
	char **matching;

	if (argc<2) exit(-1);
	filename = argv[1];

	bfd_init();

	ibfd = bfd_openr(filename, NULL);
	if (ibfd == NULL)
	{
		bfd_nonfatal("openr");
	}

	if (bfd_check_format_matches(ibfd, bfd_object, &matching))
	{

		long storage_needed;
		asymbol **symbol_table;
		long number_of_symbols;
		long i;
		symbol_info symbolinfo ;

		storage_needed = bfd_get_symtab_upper_bound (ibfd);

		if (storage_needed < 0)
			bfd_nonfatal("bfd_get_symtab_upper_bound");

		if (storage_needed == 0) {
			return ;
		}
		symbol_table = (asymbol **) xmalloc (storage_needed);
		number_of_symbols =
			bfd_canonicalize_symtab (ibfd, symbol_table);

		if (number_of_symbols < 0)
			bfd_nonfatal("bfd_canonicalize_symtab");

		printf("Scanning %i symbols\n", number_of_symbols);
		for(i=0;i<number_of_symbols;i++)
		{
			if (symbol_table[i]->section==NULL) continue;
			printf("Section %s  ",symbol_table[i]->section->name);
			bfd_symbol_info(symbol_table[i],&symbolinfo);
			printf("Symbol \"%s\"  value 0x%x\n",
					symbolinfo.name, symbolinfo.value);
		}


	} else {
		bfd_fatal("format_matches");
	}
	bfd_close(ibfd);
}

