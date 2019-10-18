//
// Created by admin on 2019/10/18.
//

#ifndef FASTREGEXCPP_REVM_H
#define FASTREGEXCPP_REVM_H

#include <vector>
#include <string>

namespace REx{

    enum INSTRUCTIONS {
        character   = 0x01,
        split       = 0x02,
        jmp         = 0x03,
        match       = 0x00,
    };

    typedef struct THREAD{
        int PC;
        int SP;

        bool alive = true;
        bool matched = false;

        THREAD (int pc, int sp): PC(pc), SP(sp) {};
    } Thread;

    class ReVM {
    public:
        ReVM(const std::string &matched_data_, const char *&program_): matched_data(matched_data_), program(program_) {}

        void start_vm();
    private:
        void run_thread(Thread *thread);
        void append_thread(Thread *thread);
        void destroy_thread(Thread *thread);

        void ins_character(Thread *thread);
        void ins_split(Thread *thread);
        void ins_jmp(Thread *thread);
        void ins_match(Thread *thread);

        const char *&program;
        const std::string &matched_data;
        std::vector<Thread *> thread_list;
    };
}

#endif //FASTREGEXCPP_REVM_H
