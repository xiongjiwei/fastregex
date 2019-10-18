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
    append_thread(new Thread(0, 0));
    for (auto thread : thread_list) {
        run_thread(thread);
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
    append_thread(new Thread(program[thread->PC + 1], thread->SP));
    append_thread(new Thread(program[thread->PC + 2], thread->SP));
    destroy_thread(thread);
}

void REx::ReVM::ins_jmp(REx::Thread *thread) {
    thread->PC = program[thread->PC + 1];
}

void REx::ReVM::ins_match(REx::Thread *thread) {

}

void REx::ReVM::append_thread(REx::Thread *thread) {
    thread_list.push_back(thread);
}

void REx::ReVM::destroy_thread(REx::Thread *thread) {
    thread->alive = false;
    delete thread;
}
