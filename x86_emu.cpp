#include "common.h"
#include <libdis.h>

#define NUM_X86_REGS 92 // from libdisasm's ia32_reg.c

struct mem_write_t {
    uint32_t addr;
    char size;
    char known;     // known value
    uint32_t value;

    mem_write_t(uint32_t a, char s, char k, uint32_t v){ addr=a; size=s; known=k; value=v; }
};

static bool libdis_inited = false;
class x86_emu {
    vector <x86_insn_type> stop_ons;
    uint32_t regs[NUM_X86_REGS];
    char*pc;
    uint32_t size;
    x86_insn_t insn;

    public:
    int debug;
    static const uint32_t UNKNOWN_VALUE = 0xdeadbabe;

    vector <uint32_t> calls;
    vector <mem_write_t> mem_writes;

    x86_emu(char*pc, uint32_t size){
        this->pc   = pc;
        this->size = size;
        debug = 0;

        stop_ons.push_back(insn_invalid);
        stop_ons.push_back(insn_return);

        for( int i=0; i<NUM_X86_REGS; i++) regs[i] = UNKNOWN_VALUE;

        if(!libdis_inited){
            libdis_inited = true;
            x86_init(opt_none, NULL, NULL);
        }
    }

    void start(){
        int pos = 0, ins_size;
        bool stop = false;

        while( pos < size ){
            ins_size = x86_disasm((unsigned char*) pc, size, (uint32_t)pc, pos, &insn );
            if( ins_size <= 0 ) break; // invalid instruction
            pos += ins_size;

            if( debug ){
                char buf[0x100];
                x86_format_insn(&insn, buf, sizeof(buf), intel_syntax);
                printf("[d] %s\n", buf);
            }

            for(int i=0; i<stop_ons.size(); i++){
                if(stop_ons[i] == insn.type){
                    stop = true;
                    break;
                }
            }
            if(stop) break;

            switch(insn.type){
                case insn_xor:  do_xor(); break;
                case insn_mov:  do_mov(); break;
                case insn_call: do_call(); break;
            }
        }
    }

    void report(){
        printf("=== x86_emu report ===\n");
        printf("[.] %d calls", calls.size());
        if( !calls.empty() ){
            printf(": ");
            for(int i=0; i<calls.size(); i++){
                printf("%8x ", calls[i]);
            }
        }
        puts("");

        printf("[.] %d mem_writes", mem_writes.size());
        if( mem_writes.empty() ){
            puts("");
        } else {
            printf(":\n");
            for(int i=0; i<mem_writes.size(); i++){
                printf("\t[.] [%8x,%d] = ", mem_writes[i].addr, mem_writes[i].size, mem_writes[i].value);
                if( mem_writes[i].value == UNKNOWN_VALUE ){
                    puts("UNKNOWN_VALUE");
                } else {
                    printf("%x\n", mem_writes[i].value);
                }
            }
        }

        puts("");
    }

    // insn_invalid automatically added each time
    void stop_on(x86_insn_type t, ...){
        stop_ons.clear();
        va_list vl;
        for( va_start(vl,t); t; t = (x86_insn_type)va_arg(vl, int)){
            stop_ons.push_back(t);
        }
        stop_ons.push_back(insn_invalid);
    }

    ~x86_emu(){
        //x86_cleanup();
    }

    private:

    void do_call(){
        uint32_t addr = 0;
        x86_op_t *op = x86_get_dest_operand( &insn );

        if ( x86_optype_is_address(op->type) ) {
                addr = op->data.sdword;
        } else if ( op->type == op_relative_near ) {
                addr = insn.addr + insn.size + op->data.relative_near;
        } else if ( op->type == op_relative_far ) {
                addr = insn.addr + insn.size + op->data.relative_far;
        }

        if(debug) printf("[d] call %x\n", addr);
        calls.push_back(addr);
    }

    void do_xor(){
        x86_op_t *dst = x86_get_dest_operand(&insn);
        x86_op_t *src = x86_get_src_operand(&insn);
        if( src->type == op_register && dst->type == op_register && src->data.reg.id == dst->data.reg.id ){
            regs[dst->data.reg.id] = 0;
        }
    }

    void do_mov(){
        x86_op_t *dst = x86_get_dest_operand(&insn);
        x86_op_t *src = x86_get_src_operand(&insn);

        // mov esi, 0x17
        if( dst->type == op_register && src->type == op_immediate ){
            regs[dst->data.reg.id] = src->data.dword;
            if(dst->data.reg.alias){
                // mov si, 0x17
                memcpy((char*)&regs[dst->data.reg.alias] + dst->data.reg.shift,
                        &regs[dst->data.reg.id],
                        dst->data.reg.size
                      );
            }
        }

        // mov [8E4587C], edi
        if( x86_optype_is_memory(dst->type) && src->type == op_register ){
            uint32_t reg_value;
            char is_known;
            if(src->data.reg.alias){
                // mov [...], si
                is_known = regs[src->data.reg.alias] != UNKNOWN_VALUE;
                reg_value = 0;
                memcpy(&reg_value,
                        (char*)&regs[src->data.reg.alias] + src->data.reg.shift,
                        src->data.reg.size
                      );
            } else {
                // mov [...], esi
                reg_value = regs[src->data.reg.id];
                is_known = reg_value != UNKNOWN_VALUE;
            }
            char buf[0x100];
            x86_format_operand(dst, buf, sizeof(buf), intel_syntax);
            if(is_valid_offset(buf)){
                mem_writes.push_back(mem_write_t(strtoul(buf+1,0,0x10), src->data.reg.size, is_known, reg_value));
            }
        }

        // mov [8E4587C], 0x17
        if( x86_optype_is_memory(dst->type) && src->type == op_immediate ){
            char buf[0x100];
            x86_format_operand(dst, buf, sizeof(buf), intel_syntax);
            if(is_valid_offset(buf)){
                switch( src->datatype ){
                    case op_byte:
                        mem_writes.push_back(mem_write_t(strtoul(buf+1,0,0x10), 1, 1, src->data.byte));
                        break;
                    case op_word:
                        mem_writes.push_back(mem_write_t(strtoul(buf+1,0,0x10), 2, 1, src->data.word));
                        break;
                    case op_dword:
                        mem_writes.push_back(mem_write_t(strtoul(buf+1,0,0x10), 4, 1, src->data.dword));
                        break;
                }
            }
        }
    }

    bool is_valid_offset(char*buf){
        for(char *p=buf; *p; p++){
            if( *p == '[' || *p == ']' || *p == 'x' || 
                    (*p >= '0' && *p<='9') || 
                    (*p >= 'a' && *p <= 'f') ||
                    (*p >= 'A' && *p <= 'F') 
            ){
                // valid offset char
            } else {
                return false;
            }
        }
        return true;
    }
};
