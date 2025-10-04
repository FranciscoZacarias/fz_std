function void
main_thread_base_entry_point(int argc, char **argv)
{
  Scratch scratch = scratch_begin(0, 0);
  os_time_init();

  // Setup log system
  {
    String8 log_path = os_executable_path(scratch.arena);
    log_path = os_directory_pop(log_path); // Pop the applicatin.exe file
    log_path = os_directory_pop(log_path); // Pop from build directory
    log_init(log_path);
    emit_info(S(">>>> Entry point <<<<"));
  }

  Command_Line cmd_line = command_line_parse_from_argc_argv(argc, argv);
  entry_point(&cmd_line);
  scratch_end(&scratch);
}