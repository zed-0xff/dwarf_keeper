#define ITEM_VALUE_FUNC         0x866d3c0

typedef int(*unit_name_func_t)(void*, string*, int) __attribute__((fastcall));

unit_name_func_t getUnitFullName = NULL;

//.text:08091A80 55                                      push    ebp
//.text:08091A81 57                                      push    edi
//.text:08091A82 89 C7                                   mov     edi, eax
//.text:08091A84 56                                      push    esi
//.text:08091A85 31 C0                                   xor     eax, eax
//.text:08091A87 83 FF FF                                cmp     edi, 0FFFFFFFFh
//.text:08091A8A 53                                      push    ebx
//.text:08091A8B 74 5A                                   jz      short loc_8091AE7
//.text:08091A8D 8B 2D 8C 2B 61 09                       mov     ebp, ds:units_vector
//.text:08091A93 8B 1D 90 2B 61 09                       mov     ebx, ds:units_vector_end
//.text:08091A99 29 EB                                   sub     ebx, ebp
//.text:08091A9B C1 FB 02                                sar     ebx, 2
//.text:08091A9E 4B                                      dec     ebx
//.text:08091A9F 78 46                                   js      short loc_8091AE7

void find_units_vector(char*region_start, char*region_end){
    const char tpl[] =
        "55 57 89 c7 56 31 c0 83 ff ff 53 74 5a "
        "8b 2d !! !! !! !! "                        // arg0 - units_vector
        "8b 1d !! !! !! !! "                        // arg1 - units_vector_end
        "29 eb c1 fb 02 4b 78 46";

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        uint32_t v0 = bt.getResult(0);
        uint32_t v1 = bt.getResult(1);
        if( v1 - v0 == 4 ){
            GAME.units_vector = (void*)v0;
        } else {
            printf("[!] invalid units_vector values: %x, %x\n", v0, v1);
        }
        // allow multiple
//        if( bt.find(p+1, region_end) ){
//            printf("[?] more than one occurency of tpl_units_vector!\n");
//        }
    } else {
            printf("[!] tpl_units_vector not found!\n");
    }
}

//.text:084ECFD0 57                                      push    edi
//.text:084ECFD1 56                                      push    esi
//.text:084ECFD2 53                                      push    ebx
//.text:084ECFD3 83 EC 10                                sub     esp, 10h
//.text:084ECFD6 A1 08 2C 61 09                          mov     eax, ds:items_vector
//.text:084ECFDB 8B 1D 0C 2C 61 09                       mov     ebx, ds:items_vector_end
//.text:084ECFE1 29 C3                                   sub     ebx, eax

void find_items_vector(char*region_start, char*region_end){
    const char tpl[] = "57 56 53 83 ec 10 a1 !! !! !! !! 8b 1d !! !! !! !! 29 c3";

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        uint32_t v0 = bt.getResult(0);
        uint32_t v1 = bt.getResult(1);
        if( v1 - v0 == 4 ){
            GAME.items_vector = (void*)v0;
        } else {
            printf("[!] invalid items_vector values: %x, %x\n", v0, v1);
        }
        if( bt.find(p+1, region_end) ){
            printf("[?] more than one occurency of tpl_items_vector!\n");
        }
    } else {
            printf("[!] tpl_items_vector not found!\n");
    }
}

//.text:0866C2B0 55                                      push    ebp
// ...
//.text:0866C2F8 83 C7 04                                add     edi, 4
//.text:0866C2FB 39 FD                                   cmp     ebp, edi
//.text:0866C2FD 76 26                                   jbe     short loc_866C325
//.text:0866C2FF
//.text:0866C2FF                         loc_866C2FF:                            ; CODE XREF: item_full_name+3Dj
//.text:0866C2FF 8B 07                                   mov     eax, [edi]
//.text:0866C301 8B 10                                   mov     edx, [eax]
//.text:0866C303 89 04 24                                mov     [esp+6Ch+var_6C], eax
//.text:0866C306 FF 52 08                                call    dword ptr [edx+8]
//.text:0866C309 48                                      dec     eax
//.text:0866C30A 75 EC                                   jnz     short loc_866C2F8
//.text:0866C30C 8B 07                                   mov     eax, [edi]
//.text:0866C30E 8B 10                                   mov     edx, [eax]
//.text:0866C310 89 04 24                                mov     [esp+6Ch+var_6C], eax
//.text:0866C313 FF 52 20                                call    dword ptr [edx+20h]
//.text:0866C316 85 C0                                   test    eax, eax

void find_item_name_func(char*region_start, char*region_end){
    const char tpl[] = "83 c7 04 39 fd 76 ?? 8b 07 8b 10 89 04 24 ff 52 08 48 75 ec 8b 07 8b 10 89 04 24 ff 52 20 85 c0";

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        for(char*p1 = p; p1>(p-0x100); p1-=4){
            if( *p1 == 0x55 ){
                GAME.item_name_func = p1;
                if( bt.find(p+1, region_end) ){
                    printf("[?] more than one occurency of getItemName!\n");
                }
                return;
            }
        }
    } else {
        printf("[!] getItemName not found!\n");
    }
}

//.text:0865EFA0 55                                      push    ebp
//.text:0865EFA1 57                                      push    edi
//.text:0865EFA2 31 FF                                   xor     edi, edi
//.text:0865EFA4 56                                      push    esi
//.text:0865EFA5 53                                      push    ebx
//.text:0865EFA6 83 EC 6C                                sub     esp, 6Ch
//.text:0865EFA9 8B B4 24 80 00 00 00                    mov     esi, [esp+7Ch+arg_0]
//.text:0865EFB0 0F B6 9C 24 88 00 00 00                 movzx   ebx, [esp+7Ch+arg_8]
//.text:0865EFB8 8B 46 24                                mov     eax, [esi+24h]
//.text:0865EFBB 8B 6E 28                                mov     ebp, [esi+28h]
//.text:0865EFBE 39 E8                                   cmp     eax, ebp
//.text:0865EFC0 73 32                                   jnb     short loc_865EFF4
//.text:0865EFC2 89 C7                                   mov     edi, eax
//.text:0865EFC4 EB 15                                   jmp     short loc_865EFDB

// XXX: getItemBaseName func may be called via Item vtable
void find_item_base_name_func(char*region_start, char*region_end){
    const char tpl[] = "55 57 31 ff 56 53 83 ec 6c 8b b4 24 80 00 00 00 0f b6 9c 24 88 00 00 00 8b 46 24 8b 6e 28 39 e8 73 32 89 c7 eb";

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        GAME.item_base_name_func = p;
        if( char* p1 = bt.find(p+1, region_end) ){
            printf("[?] more than one occurency of getItemBaseName!: %p, %p\n", p, p1);
        }
    } else {
        printf("[!] getItemBaseName not found!\n");
    }
}

// .text:081ED480 55                                      push    ebp
// .text:081ED481 BA 2D 48 D0 08                          mov     edx, offset aInteractWith ; "Interact with "
// .text:081ED486 57                                      push    edi
// .text:081ED487 B9 0E 00 00 00                          mov     ecx, 0Eh
// .text:081ED48C 56                                      push    esi
// .text:081ED48D 53                                      push    ebx
// .text:081ED48E 83 EC 3C                                sub     esp, 3Ch
// .text:081ED491 8B 44 24 54                             mov     eax, [esp+4Ch+arg_4]
// .text:081ED495 89 54 24 04                             mov     [esp+4Ch+var_48], edx
// .text:081ED499 89 4C 24 08                             mov     [esp+4Ch+var_44], ecx
// .text:081ED49D 89 04 24                                mov     [esp+4Ch+var_4C], eax
// .text:081ED4A0 E8 57 F3 E5 FF                          call    __ZNSs6assignEPKcj ; std::string::assign(char  const*,uint)
// .text:081ED4A5 8B 54 24 50                             mov     edx, [esp+4Ch+arg_0]
// .text:081ED4A9 8B 2D 68 32 61 09                       mov     ebp, ds:buildings_vector
// .text:081ED4AF 8B 7A 10                                mov     edi, [edx+10h]
// .text:081ED4B2 8B 15 6C 32 61 09                       mov     edx, ds:buildings_vector_end

void find_buildings_vector(char* region_start, char* region_end){
    const char tpl[] = 
        "55 ba ?? ?? ?? ?? "
        "57 b9 0e 00 00 00 "
        "56 53 83 ec 3c "
        "8b 44 24 54 "
        "89 54 24 04 "
        "89 4c 24 08 "
        "89 04 24 "
        "e8 ?? ?? ?? ?? "
        "8b 54 24 50 "
        "8b 2d !! !! !! !! "        // arg0 - buildings_vector
        "8b 7a 10 "
        "8b 15 !! !! !! !!";        // arg1 - buildings_vector_end

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        uint32_t v0 = bt.getResult(0);
        uint32_t v1 = bt.getResult(1);
        if( v1 - v0 == 4 ){
            GAME.buildings_vector = (void*)v0;
        } else {
            printf("[!] invalid buildings_vector values: %x, %x\n", v0, v1);
        }
        if( bt.find(p+1, region_end) ){
            printf("[?] more than one occurency of tpl_buildings_vector!\n");
        }
    } else {
            printf("[!] tpl_buildings_vector not found!\n");
    }
}

// .text:08A66360 8B 83 08 06 00 00                       mov     eax, [ebx+608h]
// .text:08A66366 8B 04 30                                mov     eax, [eax+esi]
// .text:08A66369 8B 50 10                                mov     edx, [eax+10h]
// .text:08A6636C 42                                      inc     edx
// .text:08A6636D 3B 50 14                                cmp     edx, [eax+14h]
// .text:08A66370 89 50 10                                mov     [eax+10h], edx
// .text:08A66373 0F 8E B6 00 00 00                       jle     loc_8A6642F
// .text:08A66379 B8 FC 59 E4 08                          mov     eax, 8E459FCh
// .text:08A6637E 89 FA                                   mov     edx, edi
// .text:08A66380 89 84 24 90 00 00 00                    mov     [esp+0BCh+var_2C], eax
// .text:08A66387 89 D8                                   mov     eax, ebx
// .text:08A66389 E8 42 10 F8 FF                          call    dwarf_name_with_prof_2_str
// .text:08A6638E B9 A5 94 D6 08                          mov     ecx, offset aHasForgottenAD ; " has forgotten a demand."

void find_unit_name_func(char*region_start, char*region_end){
    const char tpl[] = 
        "8b 83 ?? ?? 00 00 "
        "8b 04 30 8b 50 10 42 3b 50 14 89 50 10 0f 8e b6 00 00 00 "
        "?? ?? ?? ?? ?? "       // mov (eax, edx), 8....FC
        "89 ?? "                // mov eax, ebx / mov edx, edi
        "89 ?? 24 90 00 00 00 " // eax / edx
        "89 ?? "                // mov eax, ebx / mov edx, edi
        "e8 !! !! !! !!";       // call $+X

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        *((void**)&getUnitFullName) = p + bt.getResult(0) + bt.size();
    } else {
        printf("[!] unit_name_func not found!\n");
    }
}

///////////////////////////////////////////////////////////////////////////////

void os_init(){
    int fd = open("/proc/self/maps", O_RDONLY);
    if( -1 == fd ){
        perror("open /proc/self/maps: ");
        return;
    }

    // assume first region is a program CODE
    char buf[0x100], *pc = NULL;
    memset(buf,0,sizeof(buf));
    read(fd, buf, 0x40);
    close(fd);

    char *region_start = NULL, *region_end = NULL;

    region_start = (char*)strtoul(buf,&pc,0x10);
    if( pc ){
        region_end = (char*)strtoul(pc+1,NULL,0x10);
    }

    if( !region_start || !region_end || region_start >= region_end){
        printf("[!] invalid CODE region at %p-%p\n", region_start, region_end);
        return;
    }
    printf("[.] found CODE region at %p-%p\n", region_start, region_end);

    BENCH_START;
    find_units_vector(region_start, region_end);
    find_items_vector(region_start, region_end);
    find_buildings_vector(region_start, region_end);

    find_item_name_func(region_start, region_end);
    find_item_base_name_func(region_start, region_end);
    find_unit_name_func(region_start, region_end);
    BENCH_END("bin find");
}
