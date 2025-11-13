#pragma once

struct interrupt_ctx;

void init_interrupts();
void unihandler(struct interrupt_ctx*);
