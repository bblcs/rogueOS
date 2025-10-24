#pragma once

struct interrupt_ctx;

void interrupts_setup();
void unihandler(struct interrupt_ctx*);
