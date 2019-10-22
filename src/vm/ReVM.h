//
// Created by admin on 2019/10/18.
//

#ifndef FASTREGEXCPP_REVM_H
#define FASTREGEXCPP_REVM_H

#include <vector>
#include <string>
#include <queue>
#include <bitset>

namespace REx{
    typedef uint8_t BYTE;

    enum INSTRUCTIONS {
        character   = 0x01,         //character char<1>         :whether sp == char
        split       = 0x02,         //split L1<2>, L2<2>        :new thread  pc = L1, pc = L2
        jmp         = 0x03,         //jmp L<2>                  :jump to L

        oneof       = 0x04,         //oneof set<32>             :whether sp in set
        loopch      = 0x05,         //loop char<1> times<2>     :loop char n times

        match       = 0x00,         //match                     :end sign
    };

    typedef struct THREAD{
        int PC;
        int SP;

        int sp_start_point;

        bool alive = true;
        bool matched = false;

        THREAD (int pc, int sp, int sp_start_point): PC(pc), SP(sp), sp_start_point(sp_start_point) {};
    } Thread;

    typedef struct {
        size_t start;
        size_t end;
    } Matched_range;

    class ReVM {
    public:
        ReVM(const std::string &matched_data_, const BYTE *program_): matched_data(matched_data_), program(program_) {}

        void start_vm();
    private:
        int do_match(int sp);

        bool run_thread(Thread *thread);
        void append_thread(Thread *thread);
        void terminal_thread(Thread *thread);

        void destroy_queue();

        void record_success(size_t start, size_t end);

        void ins_character(Thread *thread);
        void ins_split(Thread *thread);
        void ins_jmp(Thread *thread);
        void ins_match(Thread *thread);
        void ins_loopch(Thread *thread);
        void ins_oneof(Thread *thread);

        int get_PC(int pc) const;
        const BYTE *program;
        const std::string &matched_data;
        std::queue<Thread *> running_thread_list;
        std::vector<Matched_range> success_thread_list;

        std::bitset<8> error_code;
    };
}

#endif //FASTREGEXCPP_REVM_H
