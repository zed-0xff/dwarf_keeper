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

// .text:0855C582 A3 60 58 E4 08                          mov     scr_target_center_x, eax
// .text:0855C587 0F BF 44 24 7C                          movsx   eax, [esp+9Ch+py]
// .text:0855C58C B9 FF FF FF FF                          mov     ecx, -1
// .text:0855C591 31 D2                                   xor     edx, edx
// .text:0855C593 BD D0 8A FF FF                          mov     ebp, -30000
// .text:0855C598 BF D0 8A FF FF                          mov     edi, -30000
// .text:0855C59D BE 17 00 00 00                          mov     esi, 17h
// .text:0855C5A2 89 0D D0 80 5F 09                       mov     ds:dword_95F80D0, ecx
// .text:0855C5A8 A3 64 58 E4 08                          mov     scr_target_center_y, eax
// .text:0855C5AD 0F BF 44 24 7A                          movsx   eax, [esp+9Ch+pUnit]
// .text:0855C5B2 89 15 DC 80 5F 09                       mov     ds:dword_95F80DC, edx
// .text:0855C5B8 89 2D 70 58 E4 08                       mov     dword_8E45870, ebp
// .text:0855C5BE 89 3D 7C 58 E4 08                       mov     dword_8E4587C, edi
// .text:0855C5C4 A3 68 58 E4 08                          mov     scr_target_center_z, eax
// .text:0855C5C9 66 89 35 1C EF 5F 09                    mov     word ptr ds:dword_95FEF1C, si
// .text:0855C5D0 C6 05 F4 2F 79 09 01                    mov     ds:byte_9792FF4, 1
// .text:0855C5D7 C6 05 F5 2F 79 09 01                    mov     ds:byte_9792FF5, 1
// .text:0855C5DE C7 04 24 01 00 00 00                    mov     dword ptr [esp+0], 1
// .text:0855C5E5 E8 96 67 D2 FF                          call    set_screen_center
// .text:0855C5EA 8B 4C 24 40                             mov     ecx, [esp+9Ch+var_5C]
// .text:0855C5EE 8B 81 A8 00 00 00                       mov     eax, [ecx+0A8h]
// .text:0855C5F4 89 04 24                                mov     [esp+0], eax    ; unit id
// .text:0855C5F7 E8 64 24 D3 FF                          call    unit_info_panel?
// .text:0855C5FC C6 43 0C 03                             mov     byte ptr [ebx+0Ch], 3

#define NUM_X86_REGS 92 // from libdisasm's ia32_reg.c

void find_screen_info(char*region_start, char*region_end){
    const char tpl[] = 
//        "a3 !! !! !! !! "           // scr_target_center_X
//        "0f bf 44 24 ?? "
        "b9 ff ff ff ff "
        "31 d2 "
        "bd d0 8a ff ff "
        "bf d0 8a ff ff "
        "?? 17 00 00 00 "           // mov (reg), 17h
        "89 ?? !! !! !! !! ";

    BinaryTemplate bt(tpl, 1);
    if( char*p = bt.find(region_start, region_end) ){
        int pos = 0, buf_size = bt.size();

        x86_emu emu(p, 0x100);
        emu.stop_on(insn_jmp, insn_jcc, insn_return, 0);
        emu.start();

        if(emu.calls.size() == 2){
            GAME.set_screen_center_func     = (void*)emu.calls[0];
            GAME.unit_info_right_panel_func = (void*)emu.calls[1];
        } else {
            printf("[!] screen_info: must have exactly 2 calls, got %d !\n", emu.calls.size());
            return;
        }

        int nUnk = 0;
        uint32_t unks[2];
        for(int i=0; i<emu.mem_writes.size(); i++){
            if( emu.mem_writes[i].known ){
                GAME.unit_info_right_panel_mem_writes.push_back(emu.mem_writes[i]);
            } else {
                if(nUnk == 2){
                    // already have 2 unknown values
                    printf("[!] screen_info: must have exactly 2 unknown values, got 3rd!\n");
                    emu.report();
                    return;
                }
                unks[nUnk] = emu.mem_writes[i].addr;
                nUnk++;
            }
        }
        if(nUnk != 2){
            // already have 2 unknown values
            printf("[!] screen_info: must have exactly 2 unknown values, got %d!\n", nUnk);
            emu.report();
            return;
        }
        if( unks[1] - unks[0] != 4 ){
            printf("[!] screen_info: unks diff must = 4, got %d!\n", unks[1]-unks[0]);
            emu.report();
            return;
        }

        GAME.scr_target_center_px = (int*)(unks[0]-4);
        GAME.scr_target_center_py = (int*)(unks[0]);
        GAME.scr_target_center_pz = (int*)(unks[1]);
    } else {
        printf("[!] screen_info not found!\n");
    }
}

// .text:08571C20 8B 87 04 06 00 00                    mov     eax, [edi+604h]
// .text:08571C26 85 C0                                test    eax, eax
// .text:08571C28 0F 84 E2 F8 FF FF                    jz      loc_8571510
// .text:08571C2E 8B A8 D4 01 00 00                    mov     ebp, [eax+1D4h]
// .text:08571C34 8B 88 D8 01 00 00                    mov     ecx, [eax+1D8h]
// .text:08571C3A 8B 5C 24 58                          mov     ebx, [esp+0BCh+var_64]
// .text:08571C3E 29 E9                                sub     ecx, ebp
// .text:08571C40 C1 F9 02                             sar     ecx, 2
// .text:08571C43 49                                   dec     ecx
// .text:08571C44 0F BF C9                             movsx   ecx, cx
// .text:08571C47 85 C9                                test    ecx, ecx
// .text:08571C49 0F 88 E1 F8 FF FF                    js      loc_8571530
// .text:08571C4F 89 C8                                mov     eax, ecx
// .text:08571C51 D1 F8                                sar     eax, 1
// .text:08571C53 8B 7C 85 00                          mov     edi, [ebp+eax*4+0]
// .text:08571C57 0F BF 17                             movsx   edx, word ptr [edi]
// .text:08571C5A 83 FA 3C                             cmp     edx, 3Ch

void find_soul_skills(char*region_start, char*region_end){
    const char tpl[] = 
        "8b 87 !! !! 00 00 "    // offset of Soul* in Unit
        "85 c0 "
        "0f 84 ?? ?? ?? ?? "
        "8b a8 !! !! 00 00 "    // offset of skills vector in Soul
        "8b 88 !! !! 00 00 "    // offset of skills vector end in soul
        "8b 5c 24 58 "
        "29 e9 "
        "c1 f9 02 "
        "49 "
        "0f bf c9 "
        "85 c9 ";

    BinaryTemplate bt(tpl);
    if(char*p = bt.find(region_start, region_end)){
        GAME.unit_soul_offset = bt.getResult(0);
        uint32_t v0 = bt.getResult(1);
        uint32_t v1 = bt.getResult(2);
        if( v1 - v0 == 4 ){
            GAME.soul_skills_offset = v0;
        } else {
            printf("[!] invalid skills vector values: %x, %x\n", v0, v1);
        }
        if( bt.find(p+1, region_end) ){
            printf("[?] more than one occurency of tpl_soul_skills!\n");
        }
    } else {
            printf("[!] tpl_soul_skills not found!\n");
    }
}

void find_happiness(char*region_start, char*region_end){
    if( !GAME.unit_info_func ){
        printf("[!] cannot find happiness w/o unit_info_func!\n");
        return;
    }
    // .text:08AB333E 8B 86 A4 06 00 00                    mov     eax, [esi+6A4h]
    // .text:08AB3344 3D 95 00 00 00                       cmp     eax, 149
    // .text:08AB3349 0F 8F 8F 01 00 00                    jg      loc_8AB34DE
    // .text:08AB334F 83 F8 7C                             cmp     eax, 124
    const char tpl[] = "8b ?? !! !! 00 00 3d 95 00 00 00 0f 8f ?? ?? ?? ?? 83 f8 7c";

    BinaryTemplate bt(tpl, 1);
    if(char*p = bt.find((char*)GAME.unit_info_func, (char*)GAME.unit_info_func+0x8000)){
        GAME.unit_happiness_offset = bt.getResult(0);
    } else {
        printf("[!] tpl_unit_happiness not found!\n");
    }
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

    find_screen_info(region_start, region_end);
    find_soul_skills(region_start, region_end);
    find_happiness(region_start, region_end);

    BENCH_END("bin find");

    {
        int n0=0, n1=0;
        for(void**p = (void**)&GAME; p <= (void**)GAME_INFO_LAST_PTR; p++){
            if(*p) n1++; else n0++;
        }
        printf("[*] GAME_INFO: %d ptrs", n0+n1);
        if(n0){
            printf(", \x1b[1;31m%d ptrs are NULL!\x1b[0m\n",n0);
        } else {
            // no NULL ptrs! excellent!
            puts("");
        }
    }
}
