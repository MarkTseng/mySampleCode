/* hack to supress config.h error for libbfd */
#define PACKAGE 1
#define PACKAGE_VERSION 1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <bfd.h>
#include <dis-asm.h>
#include <assert.h>
#include <sys/user.h>
#include <string.h>

//#include <distorm.h>


/* Resolve symbol name to addr */
long get_sym_addr(char* path, char* sym){
    long addr = 0;

    /* load symbol table*/
    long size;
    long nsym;
    asymbol **asymtab;
 
    bfd_init();
    bfd *abfd = bfd_openr(path, NULL);
 
    bfd_check_format(abfd, bfd_object);
    size = bfd_get_symtab_upper_bound(abfd);
    asymtab = malloc(size);
    nsym = bfd_canonicalize_symtab(abfd, asymtab); /*reads symtab*/

    /* get symbol addr*/
    long i = 0;
    int found = 0;
    const char* asymbol_name;

    while (i < nsym)
    {
        asymbol_name = bfd_asymbol_name(asymtab[i]);
        if (!strcmp(sym, asymbol_name)){
            addr = bfd_asymbol_value(asymtab[i]);
            found = 1;
            return addr;
        }
        else if (!found){
            i++;
        }
    }

    if (!found){
        printf("Symbol Not found! \n");
        exit(-1);   
    }
    return -1;
}


/* TODO : Get instruction size */
int get_instruction_size(char* path, long addr){
    //_DecodeResult res;
    //_DecodedInst decodedInstructions[1];
    unsigned int decodedInstructionsCount = 0;
    //_DecodeType dt = Decode64Bits;  // default is 64 bits
    _OffsetType offset = 0;

    FILE* f;
    unsigned long filesize = 0, bytesread = 0;
    struct stat st;

    unsigned char *buf;

    f = fopen(path, "rb");
    if (f == NULL) {
        perror(path);
        exit(-1);
    }

    if (fstat(fileno(f), &st) != 0) {
        perror("fstat");
        fclose(f);
        exit(-1);
    }
    filesize = st.st_size;

    buf = malloc(filesize);
    if (buf == NULL) {
        perror("File too large.");
        fclose(f);
        exit(-1);
    }

    // offset calculation from addr
    unsigned long mask = 0x000fff;
    offset = addr & mask;

    // adjust offset, the way we need it
    fseek(f, offset, SEEK_SET);
    filesize -= offset;

    // read file into memory
    bytesread = fread(buf, 1, filesize, f);

    if (bytesread != filesize) {
        perror("Can't read file into memory.");
        free(buf);
        fclose(f);
        exit(-1);
    }

    fclose(f);
#if 0
    // start disassembly
    res = distorm_decode(offset, (const unsigned char*)buf, 20, dt, decodedInstructions, 1, &decodedInstructionsCount);
    if (res == DECRES_INPUTERR) {
        // Null buffer? Decode type not 16/32/64?
        fputs("Input error, halting!\n", stderr);
        free(buf);
        exit(-1);
    }
#endif
    free(buf);
    return decodedInstructions[0].size;

}


int main(int argc, char **argv)
{
    if(argc == 1)
    {
        fprintf(stderr, "usage sym2addr <binary> <foo>");
        exit(-1);
    }

    char *path = argv[1];
    char *sym = argv[2];

    long address = get_sym_addr(path, sym);
    int size = get_instruction_size(path, address);
    printf("Address for symbol %s is 0x%lx\n", sym, address);
    printf("Size of 1st instruction is %d\n", size);
    return 0;
}

