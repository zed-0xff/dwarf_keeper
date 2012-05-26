#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <string>

using namespace std;

#include "game.h"

int verbose = 0;

#define error(msg) \
    { perror(msg); exit(EXIT_FAILURE); }

struct elf_section_info {
    off_t    file_offset;
    uint32_t va;
    uint32_t file_size;
    uint32_t mem_size;
    void*    mmap_addr;
};

vector <elf_section_info> sections;

// count number of hex numbers in string
int count_hexnums(char*s){
    int r = 0;
    if(!s || !*s) return 0;
    while(s = strstr(s+1, "0x")) r++;
    return r;
}

void add_section(char*s){
    elf_section_info section_info = {0};

    // Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
    // PHDR           0x000034 0x08048034 0x08048034 0x00100 0x00100 R E 0x4

    s = strstr(s+1, "0x"); if(!s) return;
    section_info.file_offset = strtoul(s, NULL, 0);

    s = strstr(s+1, "0x"); if(!s) return;
    section_info.va = strtoul(s, NULL, 0);
    if(section_info.va == 0) return;

    s = strstr(s+1, "0x"); if(!s) return;
    // skip physaddr

    s = strstr(s+1, "0x"); if(!s) return;
    section_info.file_size = strtoul(s, NULL, 0);
    //if(section_info.file_size == 0) return;

    s = strstr(s+1, "0x"); if(!s) return;
    section_info.mem_size = strtoul(s, NULL, 0);
    if(section_info.mem_size == 0) return;

    sections.push_back(section_info);
}

void read_sections(const char*fname){
    char buf[0x1000];

    sections.clear();

    sprintf(buf, "readelf -l %s", fname);
    FILE*fp = popen(buf, "r");
    if(!fp) error("popen: ");

    memset(buf,0,sizeof(buf));
    while(!feof(fp)){
        int len;

        *buf = 0;
        fgets(buf, sizeof(buf), fp);
        len = strlen(buf);
        if( len < 70 || len > 90 || count_hexnums(buf) < 5) continue;
        if(verbose) printf("[d] %s", buf);
        add_section(buf);
    }
    pclose(fp);
}

void reduce_sections(){
    if( sections.size() < 2) return; // nothing to reduce

    bool deleted;
    do {
        deleted = false;
        for( auto it1 = sections.begin(); it1 < sections.end(); ++it1){
            for( auto it2 = it1+1; it2 < sections.end(); ++it2){
                if(
                    it2->va       >= it1->va        && 
                    it2->mem_size <= it1->mem_size
                ){
                    sections.erase(it2);
                    deleted = true;
                    break;
                }
            }
            if( deleted ) break;

            for( auto it2 = it1+1; it2 < sections.end(); ++it2){
                if(
                    it1->va       >= it2->va        && 
                    it1->mem_size <= it2->mem_size
                ){
                    sections.erase(it1);
                    deleted = true;
                    break;
                }
            }
            if( deleted ) break;
        }
    } while( deleted );
}

void dump_sections(){
    for(int i=0; i<sections.size(); i++){
        printf("[.] section %2d: va=%8x mem_size=%8x\n", i, sections[i].va, sections[i].mem_size);
    }
}

int mmap_file(const char*fname){
    int fd;

    read_sections(fname);

    if( sections.size() == 0 ){
        printf("[?] no sections. cannot continue\n");
        exit(1);
    }

    if( verbose ) printf("[.] %d sections\n", sections.size());
    if( verbose > 1 ) dump_sections();

    reduce_sections();
    if( verbose ) printf("[.] %d sections after reduce\n", sections.size());
    if( verbose > 1 ) dump_sections();

    fd = open(fname, O_RDONLY);
    if (fd == -1) error("open");

    for( int i=0; i<sections.size(); i++){
        //printf("[.] %d %8x\n", i, sections[i].file_offset);
        void* addr = mmap(
                (void*)sections[i].va, 
                sections[i].mem_size,
                PROT_READ, 
                MAP_PRIVATE, 
                fd, 
                sections[i].file_offset);

        if (addr == MAP_FAILED) error("mmap");

        sections[i].mmap_addr = addr;

        if( verbose ) printf("[.] mmapped %6x bytes at %p\n", sections[i].file_size, addr);
    }

    return fd;
}

void munmap_file(int fd){
    for( int i=0; i<sections.size(); i++){
        if( !sections[i].mmap_addr) continue;
        munmap(sections[i].mmap_addr, sections[i].mem_size);
        sections[i].mmap_addr = NULL;
    }

    close(fd);
}

void process_file(const char*fname){
    int fd = mmap_file(fname);

    if( sections.size() == 0 ){
        printf("[?] no sections. cannot continue\n");
        exit(1);
    }

    char *region_start = NULL, *region_end = NULL;

    for( int i=0; i<sections.size(); i++){
        if( !sections[i].mmap_addr) continue;

        char *r_start = (char*)sections[i].mmap_addr;
        char *r_end   = (char*)sections[i].mmap_addr + sections[0].mem_size;

        if( !region_start || r_start < region_start ) region_start = r_start;
        if( !region_end   || r_end   > region_end   ) region_end   = r_end;
    }

    if( !region_start || !region_end ){
        printf("[!] region_start(%p) and region_end(%p) must not be NULL!\n", 
                region_start, region_end);
        exit(1);
    }

//    int nCalls = 0;
//    for(char*p = region_start; p<(region_end-0x10); p++){
//        if( *(unsigned char*)p == 0xe8){
//            char* addr = p+*(int*)(p+1);
//            if(addr >= region_start && addr <= region_end){
//                nCalls++;
//            }
//        }
//    }
//    printf("[d] total %d calls\n", nCalls);

    memset(&GAME, 0, (char*)GAME_INFO_LAST_PTR - (char*)&GAME + 4 );
    os_init(region_start, region_end);

    if(verbose){
        int i = 0;
        for(uint32_t*p = (uint32_t*)&GAME; p <= (uint32_t*)GAME_INFO_LAST_PTR; p++, i++){
            printf("[d] %2d: %8x\n", i, *p);
        }
    }

    munmap_file(fd);
}

int main(int argc, char *argv[]) {
    const char*fname;

    switch( argc ){
        case 0:
        case 1:
            puts("I need a Dwarf Fortress binary filename(s) !");
            exit(1);
            break;

        case 2:
            verbose = 1;
            process_file(argv[1]);
            break;

        default:
            verbose = 0;
            for(int i=1; i<argc; i++){
                if( i>1) puts("");
                printf("[*] %s\n", argv[i]);
                process_file(argv[i]);
            }
            break;
    }

    return 0;
}

