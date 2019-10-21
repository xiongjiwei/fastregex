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
        ReVM(const std::string &matched_data_, const char *&program_): matched_data(matched_data_), program(program_) {}

        void start_vm();
    private:
        void run_thread(Thread *thread);
        void append_thread(Thread *thread);
        void destroy_thread(Thread *thread);

        void record_success(size_t start, size_t end);

        void ins_character(Thread *thread);
        void ins_split(Thread *thread);
        void ins_jmp(Thread *thread);
        void ins_match(Thread *thread);

        const char *&program;
        const std::string &matched_data;
        std::vector<Thread *> running_thread_list;
        std::vector<Matched_range> success_thread_list;
    };
}

#endif //FASTREGEXCPP_REVM_H
