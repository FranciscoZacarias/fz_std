internal void
entry_main_thread(void (*entry_point)(u64 argc, char** argv), u64 argument_count, char **arguments) {
	Assert(F_THREAD_CONTEXT_H);

	//~ Setup
	Thread_Context tctx = thread_context_alloc();
	tctx.is_main_thread = true;
	thread_context_set(&tctx);
	thread_context_set_name(StringLiteral("Main Thread"));
	
	//~ Entry point
	entry_point(argument_count, arguments);

	//~ Cleanup	
	thread_context_release(&tctx);
}

//~ Non-main-thread entry point
internal void
entry_non_main_thread(void (*entry_point)(void *p), void *params) {
	Thread_Context tctx = thread_context_alloc();
	thread_context_set(&tctx);
	entry_point(params);
	thread_context_release(&tctx);
}
