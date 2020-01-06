/*******************************************
* File Name : elf_link_map.c
* Purpose :
* Creation Date : 02-09-2016
* Last Modified : Fri 09 Sep 2016 10:55:40 AM CST
* Created By : Mark Tseng  
**********************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
# define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

#include <link.h>
#include <dlfcn.h>

///////////////////////////////////////////////////////////////////////////////
static const ElfW(Dyn) *
FindTag(const ElfW(Dyn) * dyn, const ElfW(Sxword) tag) {
  for (; dyn->d_tag != DT_NULL; ++dyn) {
    if (dyn->d_tag == tag) {
      return dyn;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
static size_t
FindVal(const ElfW(Dyn) * dyn, const ElfW(Sxword) tag) {
  for (; dyn->d_tag != DT_NULL; ++dyn) {
    if (dyn->d_tag == tag) {
      return dyn->d_un.d_val;
    }
  }
  //assert(false);
}

///////////////////////////////////////////////////////////////////////////////
static const void *
FindPtr(const ElfW(Addr) load_addr,
           const ElfW(Dyn) * dyn, const ElfW(Sxword) tag) {
  for (; dyn->d_tag != DT_NULL; ++dyn) {
    if (dyn->d_tag == tag) {
      return (const void *)(dyn->d_un.d_ptr - load_addr);
    }
  }
  //assert(false);
}

///////////////////////////////////////////////////////////////////////////////
#define Title(...) printf("-------------------------------------------------" \
  "------------------------------\n" __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
int main(const int argc, const char * const * const argv) {
  assert((argc == 2) && (argc == 2));
  const char * const lib = argv[1];
  Title("Loading: %s\n", lib);
  void * const handle = dlopen(lib, RTLD_LAZY);
  assert(handle != 0);
#ifdef _GNU_SOURCE
  // Get the link map
  const struct link_map * link_map = 0;
  const int ret = dlinfo(handle, RTLD_DI_LINKMAP, &link_map);
  const struct link_map * const loaded_link_map = link_map;
  assert(ret == 0);
  assert(link_map != 0);
  Title("Libraries:\n");
  while (link_map->l_prev) {
    link_map = link_map->l_prev;
  }
  while (link_map->l_next) {
    printf(" - %s (0x%016" PRIX64 ")\n", link_map->l_name, link_map->l_addr);
    link_map = link_map->l_next;
  }
  // Process the dynamic sections
  const ElfW(Dyn) * const dyn_start = loaded_link_map->l_ld;
  const ElfW(Addr) load_addr = loaded_link_map->l_addr;
  Title("Dynamic Sections (%s):\n", loaded_link_map->l_name);
  printf("|%-16s|%-10s|%-12s|%-16s|%-16s|\n", "Tag", "Tag", "Value", "Ptr",
    "Offset");
  for (const ElfW(Dyn) * dyn = dyn_start; dyn->d_tag != DT_NULL; ++dyn) {
    switch (dyn->d_tag) {
      #define print(tag) \
        printf("|%-16s|0x%-8" PRIx64 "|%12" PRIu64 "|%-16p|%-16p|\n", \
          tag, dyn->d_tag, dyn->d_un.d_val, (const void *)(dyn->d_un.d_ptr), \
          (const void *)(dyn->d_un.d_ptr - link_map->l_addr)); \
        break
      #define case(tag) case tag: print(#tag)
      #define default(tag) default: print(#tag)
      case (DT_NEEDED);           /* Name of needed library */
      case (DT_PLTRELSZ);         /* Size in bytes of PLT relocs */
      case (DT_PLTGOT);           /* Processor defined value */
      case (DT_HASH);             /* Address of symbol hash table */
      case (DT_STRTAB);           /* Address of string table */
      case (DT_SYMTAB);           /* Address of symbol table */
      case (DT_RELA);             /* Address of Rela relocs */
      case (DT_RELASZ);           /* Total size of Rela relocs */
      case (DT_RELAENT);          /* Size of one Rela reloc */
      case (DT_STRSZ);            /* Size of string table */
      case (DT_SYMENT);           /* Size of one symbol table entry */
      case (DT_INIT);             /* Address of init function */
      case (DT_FINI);             /* Address of termination function */
      case (DT_SONAME);           /* Name of shared object */
      case (DT_RPATH);            /* Library search path (deprecated) */
      case (DT_SYMBOLIC);         /* Start symbol search here */
      case (DT_REL);              /* Address of Rel relocs */
      case (DT_RELSZ);            /* Total size of Rel relocs */
      case (DT_RELENT);           /* Size of one Rel reloc */
      case (DT_PLTREL);           /* Type of reloc in PLT */
      case (DT_DEBUG);            /* For debugging; unspecified */
      case (DT_TEXTREL);          /* Reloc might modify .text */
      case (DT_JMPREL);           /* Address of PLT relocs */
      case (DT_BIND_NOW);         /* Process relocations of object */
      case (DT_INIT_ARRAY);       /* Array with addresses of init fct */
      case (DT_FINI_ARRAY);       /* Array with addresses of fini fct */
      case (DT_INIT_ARRAYSZ);     /* Size in bytes of DT_INIT_ARRAY */
      case (DT_FINI_ARRAYSZ);     /* Size in bytes of DT_FINI_ARRAY */
      case (DT_RUNPATH);          /* Library search path */
      case (DT_FLAGS);            /* Flags for the object being loaded */
      case (DT_ENCODING);         /* Start of encoded range */
        /* This is a duplicate value Have submitted this as a possible bug:
         * http://sourceware.org/bugzilla/show_bug.cgi?id=15733
         */
//      case (DT_PREINIT_ARRAY);    /* Array with addresses of preinit fct*/
      case (DT_PREINIT_ARRAYSZ);  /* size in bytes of DT_PREINIT_ARRAY */
      case (DT_NUM);              /* Number used */
      case (DT_LOOS);             /* Start of OS-specific */
      case (DT_HIOS);             /* End of OS-specific */
      case (DT_LOPROC);           /* Start of processor-specific */
      case (DT_HIPROC);           /* End of processor-specific */
      case (DT_PROCNUM);          /* Most used by any processor */
      case (DT_GNU_HASH);         /* GNU-style hash table.  */
      case (DT_VERDEF);           /* Address of version definition table */
      case (DT_VERDEFNUM);        /* Number of version definitions */
      case (DT_VERNEED);          /* Address of table with needed versions */
      case (DT_VERNEEDNUM);       /* Number of needed versions */
      case (DT_VERSYM);           /* The versioning entry types. */
      case (DT_RELACOUNT);
      case (DT_CHECKSUM);
      case (DT_GNU_PRELINKED);    /* Prelinking timestamp */
      default(UNKNOWN);
      #undef print
      #undef case
    }
  }
  // Some aliases
  #define GetTag(tag) FindTag(dyn_start, tag)
  #define GetVal(tag) FindVal(dyn_start, tag)
  #define GetPtr(tag) FindPtr(load_addr, dyn_start, tag)
  #define IterTag(tag) \
    for (const ElfW(Dyn) * dyn = GetTag(tag); dyn; dyn = FindTag(++dyn, tag))
  // Get the string table
  const size_t strtabsize = GetVal(DT_STRSZ);
  const char * const strtab = (const char * const)GetPtr(DT_STRTAB);
  Title("String Table: %p (%" PRIu64")\n", strtab, strtabsize);
  // Get the so name
  Title("SO Name: %s\n", &strtab[GetVal(DT_SONAME)]);
  // Get the needed libraries
  Title("Needed:\n");
  IterTag(DT_NEEDED) {
    const size_t index = dyn->d_un.d_val;
    assert(index < strtabsize);
    printf(" - %s\n", &strtab[dyn->d_un.d_val]);
  }
  // Get the symbol table
  typedef ElfW(Sym) SymEnt;
  const size_t symentsize = GetVal(DT_SYMENT);
  const SymEnt * const symtab = (const SymEnt*)GetVal(DT_SYMTAB);
  const SymEnt * syment = symtab;
  Title("Symbols:\n");
  printf("|%-16s|%-10s|%-10s|%-8s|%-16s|%-8s|\n", "Name", "Type",
    "Visibility", "Section", "Addr", "Size");
  while (syment->st_shndx != STN_UNDEF) {
    assert(syment->st_name);
    assert(syment->st_name < strtabsize);
    printf("|%-16s|%10u|%10u|%8u|%-16p|%8" PRIu64"|\n",
      &strtab[syment->st_name], syment->st_info, syment->st_other,
      syment->st_shndx, (const void*)(syment->st_value), syment->st_size);
    syment = (const SymEnt*)((const uint8_t*)(syment) + symentsize);
  }
#else
# warning Not using GNU extensions
#endif
  dlclose(handle);
  return 0;
}

