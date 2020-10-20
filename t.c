
void __sch_save_ctx(){
    __asm__ __volatile__ (
        "movq %%rbx, 32(%0)\n\t"
        "movq %0, %%rbx\n\t"
        "movq %%r12, (%%rbx)\n\t"
        "movq %%r13, 8(%%rbx)\n\t"
        "movq %%r14, 16(%%rbx)\n\t"
        "movq %%r15, 24(%%rbx)\n\t"
        ::"a"(__S_zerqaq.routinue_ctxs[__S_zerqaq.running]->reg)
        :"r12", "r13", "r14", "r15", "rbx"
    );
    __Context *ctx = __S_zerqaq.routinue_ctxs[__S_zerqaq.running];
    u64 *rbp;
    get_reg(rbp, rbp);
    ctx->reg[6] = *rbp;
    ctx->reg[5] = (u64)(rbp + 2);
    ctx->addr = *(rbp + 1);

    u64 stack_size = ctx->stack_base - ctx->reg[5];
    if(stack_size > ctx->stack_size){
        if(ctx->stack_size > 0) free(ctx->stack);
        ctx->stack = malloc(stack_size);
    }
    ctx->stack_size = stack_size;
    u8 *rsp = ctx->reg[5];
    for(int i = 0; i < ctx->stack_size; i++) ctx->stack[i] = rsp[i];

    list_add(__S_zerqaq.ready, ctx->node_ptr);

    printf("stack_base: %llx\n", ctx->stack_base);
    puts("reg:");
    for(int i = 0; i < 7; i++){
        printf("%llx ", ctx->reg[i]);
    } puts("");
    printf("addr: %llx\n", ctx->addr);
    printf("stack(%lld):\n", ctx->stack_size);
    for(int i = 0; i < ctx->stack_size; i++){
        if(i % 4 == 0) putchar(' ');
        printf("%02x", ctx->stack[i]);
    } puts("");

    __jmp_to_sch;
}


//恢复协程上下文 并运行
void ___sch_recover(){
    //set_reg(rbp, __S_zerqaq.sch_s_base);
    __r_ctx.rsp = __r_ctx.ctx->stack_base - __r_ctx.ctx->stack_size;
    printf("sch_recover: recovering routinue %d(%s)\n", __r_ctx.ctx->id, __r_ctx.ctx->func_name);
    printf("sch_recover: rsp:%p ctx->rbp:%p size:%lld", __r_ctx.rsp, __r_ctx.ctx->rbp, __r_ctx.ctx->stack_size); puts("");
    printf("sch_recover: ctx->stack in %p", __r_ctx.ctx->stack); puts("");
    for(__r_ctx.i = 0; __r_ctx.i < __r_ctx.ctx->stack_size; __r_ctx.i++){
        if(__r_ctx.i % 4 == 0) putchar(' ');
        printf("%02x", __r_ctx.ctx->stack[__r_ctx.i]);
        //printf("%d %d", __r_ctx.i, __r_ctx.ctx->stack_size); puts("");
        __r_ctx.rsp[__r_ctx.i] = __r_ctx.ctx->stack[__r_ctx.i];
    }
    //puts("sch_recover: stack recovery fin");
    //memcpy(rsp, ctx->stack, ctx->stack_size);
    //set_reg(rsp, rsp);
    printf("sch_recover: jmp to %p", __r_ctx.ctx->addr); puts("");
    //jmp(ctx->addr);
    __asm__ __volatile__ (
        "movq %0, %%rsp\n\t"
        //"movq %1, %%rbp\n\t"
        "pushq %2\n\t"
        "retq\n\t"
        ::"r"(__r_ctx.rsp), "r"(__r_ctx.ctx->rbp), "r"(__r_ctx.ctx->addr)
    );
}