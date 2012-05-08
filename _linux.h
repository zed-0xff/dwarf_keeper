typedef int(*unit_name_func_t)(void*, string*, int) __attribute__((fastcall));

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

void find_unit_info_func(char*region_start, char*region_end){
    const char very_unhappy[] = "very unhappy";
    char* p_very_unhappy = (char*)memmem(region_start, region_end-region_start, very_unhappy, strlen(very_unhappy)+1);
    if( !p_very_unhappy ){
        printf("[!] cannot find '%s' string\n", very_unhappy);
        return;
    }
    char buf[0x40];
    memcpy(buf, "\xc7\x44\x24\x04", 4);
    memcpy(buf+4, &p_very_unhappy, 4);

    for(char* p = region_start; p = (char*)memmem(p, region_end-p, buf, 8); p++){
        // .text:08AB337C 
        // C7 44 24 04 C6 A0 D6 08                 
        // mov     [esp+64Ch+var_648], offset aVeryUnhappy ; "very unhappy"
        p = (char*)((uint32_t)p | 3) + 1; // dword-align

        for(char*p1 = p; p1>(p-0x200); p1-=4){
            if( !memcmp(p1, "\x55\x57\x56", 3) ){
                GAME.unit_info_func = p1;
                return;
            }
        }
    }
    printf("[!] unit_info_func not found!\n");
}

///////////////////////////////////////////////////////////////////////////////

#define FIND_SIMPLE(WHAT, TPL) { \
    BinaryTemplate bt(TPL); \
    if(char*p = bt.find(region_start, region_end)){ \
        GAME.WHAT = p; \
        if( char* p1 = bt.find(p+1, region_end) ){ \
            printf("[?] more than one occurency of " #WHAT "!: %p, %p\n", p, p1); \
        } \
    } else { \
        printf("[!] " #WHAT " not found!\n"); \
    } \
}

#define FIND_BY_CALL(WHAT, TPL) { \
    BinaryTemplate bt(TPL); \
    if(char*p = bt.find(region_start, region_end)){ \
        GAME.WHAT = p + bt.getResult(0) + bt.size(); \
        if( char* p1 = bt.find(p+1, region_end) ){ \
            printf("[?] more than one occurency of " #WHAT "!: %p, %p\n", p, p1); \
        } \
    } else { \
        printf("[!] " #WHAT " not found!\n"); \
    } \
}

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

    FIND_SIMPLE(item_value_func, "55 57 56 53 83 EC 5C 8B  5C 24 70 8B 6C 24 78 8B");

    // XXX: getItemBaseName func may be called via Item vtable
    FIND_SIMPLE(item_base_name_func, "55 57 31 ff 56 53 83 ec 6c 8b b4 24 80 00 00 00 0f b6 9c 24 88 00 00 00 8b 46 24 8b 6e 28 39 e8 73 32 89 c7 eb");

    // XXX: unit coords are in pUnit +0x48, +0x4a, +0x4c
    FIND_SIMPLE(unit_coords_func, "83 EC 3C 8B 44 24 40 89 7C 24 34 8B 54 24 48 89 6C 24 38 8B 7C 24 44 89 5C 24 2C 8B 6C 24 4C 89 74 24 30 F6 80 8F 00 00 00 02 89 54 24 1C 66 C7");
    
    FIND_SIMPLE(cmp_item_size_func, "55 57 56 53 BB 58 1B 00 00 0F BF 4C 24 20 0F BF 74 24 14 0F BF 7C 24 18 0F BF 54 24 1C 85 C9 78 14 8b 2d");

    FIND_BY_CALL(unit_name_func,
            "8b 83 ?? ?? 00 00 "
            "8b 04 30 8b 50 10 42 3b 50 14 89 50 10 0f 8e b6 00 00 00 "
            "?? ?? ?? ?? ?? "       // mov (eax, edx), 8....FC
            "89 ?? "                // mov eax, ebx / mov edx, edi
            "89 ?? 24 90 00 00 00 " // eax / edx
            "89 ?? "                // mov eax, ebx / mov edx, edi
            "e8 !! !! !! !!"        // call $+X
            );

    find_unit_info_func(region_start, region_end);

    FIND_BY_CALL(skill_lvl_2_string_func,
            "8b 10 89 04 24 ff 52 1c 39 c5 "
            "0f 84 ?? ?? ?? ?? "    // jz      loc_84DC7A8
            "C6 05 08 ?? ?? ?? 07 " // mov     ds:byte_8E45A08, 7
            "C6 05 09 ?? ?? ?? 00 " // mov     ds:byte_8E45A09, 0
            "C6 05 0A ?? ?? ?? 01 " // mov     ds:byte_8E45A0A, 1
            "a1 ?? ?? ?? ?? "
            "8b 04 a8 85 c0 "
            "0f 8e ?? ?? ?? ?? "
            "89 44 24 04 "
            "89 34 24 "
            "e8 !! !! !! !!"        // call skill_lvl_2_string
            );

    FIND_SIMPLE(skill_id_2_string_func, "83 EC 4C 0F BF 54 24 54 89 6C 24 48 0F BF 44 24 5C 89 5C 24 3C 8B 5C 24 50 89 74 24 40 0F BF 74 24 58 0F B7 EA 83 FD 73  89 7C 24 44 77 28 0F BF");

    BENCH_END("bin find");
}
