//
// Created by admin on 2019/10/18.
//

#include "ReVM.h"

void REx::ReVM::run_thread(REx::Thread *thread) {
    while (thread->alive) {
        switch (program[thread->PC]) {
            case character:
                ins_character(thread);
                break;
            case jmp:
                ins_jmp(thread);
                break;
            case split:
                ins_split(thread);
                break;
            case match:
                ins_match(thread);
                break;
        }
    }
}

void REx::ReVM::start_vm() {
    for (size_t i = 0; i < matched_data.length(); ++i) {
        append_thread(new Thread(0, i, i));
        for (auto thread : running_thread_list) {
            run_thread(thread);
        }
    }
}

void REx::ReVM::ins_character(REx::Thread *thread) {
    if (program[thread->PC + 1] == matched_data[thread->SP]) {
        thread->SP += 2;
        thread->PC++;
    } else {
        destroy_thread(thread);
    }
}

void REx::ReVM::ins_split(REx::Thread *thread) {
    thread->PC = program[thread->PC + 1];
    append_thread(new Thread(program[thread->PC + 2], thread->SP, thread->sp_start_point));
}

void REx::ReVM::ins_jmp(REx::Thread *thread) {
    thread->PC = program[thread->PC + 1];
}

void REx::ReVM::ins_match(REx::Thread *thread) {
    record_success(thread->sp_start_point, thread->SP);
}

void REx::ReVM::append_thread(REx::Thread *thread) {
    running_thread_list.push_back(thread);
}

void REx::ReVM::destroy_thread(REx::Thread *thread) {
    thread->alive = false;
    delete thread;
}

void REx::ReVM::record_success(size_t start, size_t end) {
    Matched_range matchedRange;
    matchedRange.start = start;
    matchedRange.end = end;
    success_thread_list.push_back(matchedRange);
}
