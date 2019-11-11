//
// Created by admin on 2019/10/18.
//

#include <cstring>
#include "vm.h"

void REx::Vm::start_vm() {
    for (size_t i = 0; i < matched_data.length();) {
        i = do_match(i);
    }
}

int REx::Vm::do_match(int sp) {
    append_thread(new Thread(0, sp, sp));
    while (!running_thread_list.empty()) {
        Thread *next_thread = get_next_thread();
        if (run_thread(next_thread)) {
            destroy_running_thread_list();

            return success_recorder.back().end;
        }
        delete next_thread;
    }

    return sp + 1;
}

bool REx::Vm::run_thread(REx::Thread *thread) {
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
            case loopch:
                ins_loopch(thread);
                break;
            case loop:
                ins_loop(thread);
                break;
            case endloop:
                ins_endloop(thread);
                break;
            case oneof:
                ins_oneof(thread);
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

void REx::Vm::ins_character(REx::Thread *thread) {
    if (thread->SP < matched_data.length() && program[thread->PC + 1] == matched_data[thread->SP]) {
        thread->SP += 1;
        thread->PC += 2;
    } else {
        terminal_thread(thread);
    }
}

void REx::Vm::ins_split(REx::Thread *thread) {
    append_thread(new Thread(thread->PC + bit16_to_int16(thread->PC + 3), thread->SP, thread->sp_start_point));
    thread->PC = bit16_to_int16(thread->PC + 1) + thread->PC;
}

void REx::Vm::ins_jmp(REx::Thread *thread) {
    thread->PC = bit16_to_int16(thread->PC + 1) + thread->PC;
}

void REx::Vm::ins_match(REx::Thread *thread) {
    record_success(thread->sp_start_point, thread->SP);
    terminal_thread(thread);
}

void REx::Vm::ins_loopch(REx::Thread *thread) {
//    int times = program[thread->PC + 2] * 256 + program[thread->PC + 3];
    int times = bit16_to_int16(thread->PC + 2);

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

void REx::Vm::ins_loop(REx::Thread *thread) {
//    loop_times = program[thread->PC + 1] * 256 + program[thread->PC + 2];
    loop_times = bit16_to_int16(thread->PC + 1);
    thread->PC += 3;
}

void REx::Vm::ins_endloop(REx::Thread *thread) {
    loop_times--;
    if (loop_times == 0) {
        thread->PC += 3;
    } else {
        thread->PC += bit16_to_int16(thread->PC + 1);
    }
}

void REx::Vm::ins_oneof(REx::Thread *thread) {
    int index = matched_data[thread->SP] / 8;       //index of set<32>
    int bit_index = matched_data[thread->SP] % 8;   //index of bit in BYTE

    BYTE mask = 0x80u >> bit_index;                 //1000 0000

    BYTE i = program[thread->PC + 1 + index];
    if ((i & mask) != 0) {
        thread->SP += 1;
        thread->PC += 33;
    } else {
        terminal_thread(thread);
    }
}

void REx::Vm::append_thread(REx::Thread *thread) {
    running_thread_list.push(thread);
}

void REx::Vm::terminal_thread(REx::Thread *thread) {
    thread->alive = false;
}

void REx::Vm::record_success(size_t start, size_t end) {
    Matched_range matchedRange;
    matchedRange.start = start;
    matchedRange.end = end;
    success_recorder.push_back(matchedRange);
}

void REx::Vm::destroy_running_thread_list() {
    while (!running_thread_list.empty()) {
        delete running_thread_list.top();
        running_thread_list.pop();
    }
}

int16_t REx::Vm::bit16_to_int16(int pc) const {
    int16_t t = 0;
    memcpy(&t, program + pc, 2);
    return t;
}

std::vector<REx::Matched_range> REx::Vm::get_matched_result() {
    return success_recorder;
}

REx::Thread *REx::Vm::get_next_thread() {
    Thread *&pThread = running_thread_list.top();
    running_thread_list.pop();
    return pThread;
}
