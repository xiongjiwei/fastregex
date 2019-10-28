//
// Created by admin on 2019/10/18.
//

#include "vm.h"

void REx::vm::start_vm() {
    for (size_t i = 0; i < matched_data.length();) {
        i = do_match(i);
    }
}

int REx::vm::do_match(int sp) {
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

bool REx::vm::run_thread(REx::Thread *thread) {
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
            case oneof:
                ins_oneof(thread);
            case match:
                ins_match(thread);
                return true;
            default:
                return false;
        }
    }

    return false;
}

void REx::vm::ins_character(REx::Thread *thread) {
    if (thread->SP < matched_data.length() && program[thread->PC + 1] == matched_data[thread->SP]) {
        thread->SP += 1;
        thread->PC += 2;
    } else {
        terminal_thread(thread);
    }
}

void REx::vm::ins_split(REx::Thread *thread) {
    append_thread(new Thread(thread->PC + get_PC(thread->PC + 3), thread->SP, thread->sp_start_point));
    thread->PC = get_PC(thread->PC + 1) + thread->PC;
}

void REx::vm::ins_jmp(REx::Thread *thread) {
    thread->PC = get_PC(thread->PC + 1) + thread->PC;
}

void REx::vm::ins_match(REx::Thread *thread) {
    record_success(thread->sp_start_point, thread->SP);
    terminal_thread(thread);
}

void REx::vm::ins_loopch(REx::Thread *thread) {
    int times = program[thread->PC + 2] * 256 + program[thread->PC + 3];

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

void REx::vm::ins_oneof(REx::Thread *thread) {
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

void REx::vm::append_thread(REx::Thread *thread) {
    running_thread_list.push(thread);
}

void REx::vm::terminal_thread(REx::Thread *thread) {
    thread->alive = false;
}

void REx::vm::record_success(size_t start, size_t end) {
    Matched_range matchedRange;
    matchedRange.start = start;
    matchedRange.end = end;
    success_thread_list.push_back(matchedRange);
}

void REx::vm::destroy_queue() {
    while (!running_thread_list.empty()) {
        delete running_thread_list.front();
        running_thread_list.pop();
    }
}

int REx::vm::get_PC(int pc) const {
    int negative = (program[pc] & 0x80u) == 0 ? 1 : -1;      //when highest bit is 1 the number is negative, or positive.
    BYTE h = program[pc] & 0x7Fu;                            //
    return (int) (h * 128u + program[pc + 1]) * negative;
}
