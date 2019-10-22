//
// Created by admin on 2019/10/18.
//

#include "ReVM.h"

void REx::ReVM::start_vm() {
    for (size_t i = 0; i < matched_data.length();) {
        i = do_match(i);
    }
}

int REx::ReVM::do_match(int sp) {
    append_thread(new Thread(0, sp, sp));
    while (!running_thread_list.empty()) {
        if (run_thread(running_thread_list.front())) {
            destroy_queue();

            return success_thread_list.back().end;
        }
        delete running_thread_list.front();
        running_thread_list.pop();
    }

    return sp + 1;
}

bool REx::ReVM::run_thread(REx::Thread *thread) {
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
                return true;
            default:
                return false;
        }
    }

    return false;
}

void REx::ReVM::ins_character(REx::Thread *thread) {
    if (thread->SP < matched_data.length() && program[thread->PC + 1] == matched_data[thread->SP]) {
        thread->SP += 1;
        thread->PC += 2;
    } else {
        terminal_thread(thread);
    }
}

void REx::ReVM::ins_split(REx::Thread *thread) {
    append_thread(new Thread(get_PC(thread->PC + 3), thread->SP, thread->sp_start_point));
    thread->PC = get_PC(thread->PC + 1);
}

void REx::ReVM::ins_jmp(REx::Thread *thread) {
    thread->PC = get_PC(thread->PC + 1);
}

void REx::ReVM::ins_match(REx::Thread *thread) {
    record_success(thread->sp_start_point, thread->SP);
    terminal_thread(thread);
}

void REx::ReVM::ins_loopch(REx::Thread *thread) {
    int times = get_PC(thread->PC + 2);

    while (times != 0) {
        if (thread->SP < matched_data.length() && program[thread->PC + 1] == matched_data[thread->SP]) {
            thread->SP += 1;
            times--;
        } else {
            terminal_thread(thread);
            return;
        }
    }

    thread->PC += 4;
}

void REx::ReVM::ins_oneof(REx::Thread *thread) {
    
}

void REx::ReVM::append_thread(REx::Thread *thread) {
    running_thread_list.push(thread);
}

void REx::ReVM::terminal_thread(REx::Thread *thread) {
    thread->alive = false;
}

void REx::ReVM::record_success(size_t start, size_t end) {
    Matched_range matchedRange;
    matchedRange.start = start;
    matchedRange.end = end;
    success_thread_list.push_back(matchedRange);
}

void REx::ReVM::destroy_queue() {
    while (!running_thread_list.empty()) {
        delete running_thread_list.front();
        running_thread_list.pop();
    }
}

int REx::ReVM::get_PC(int pc) const {
    return program[pc] * 256 + program[pc + 1];
}
