#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER   "src/"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
 
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    // Get files in SRC_FOLDER
    // Nob_File_Paths all_files = {0};
    // Nob_File_Paths src_files = {0};
    // char *ptr_to_dot;
    // char extension;

    // if (!nob_read_entire_dir(SRC_FOLDER, &all_files)) return 1;
    // printf("[all_files] count = %d\n", all_files.count);
    // printf("[all_files] capacity = %d\n", all_files.capacity);
    // printf("[all_files] count = %d\n", src_files.count);
    // printf("[all_files] capacity = %d\n", src_files.capacity);

    // for (size_t i = 0, j = 0; i < all_files.count; i++) {
    //     // printf("%s\n", all_files.items[i]);
    //     if (ptr_to_dot = strchr(all_files.items[i], '.')) {
    //         extension = *(ptr_to_dot + 1);
    //         if (extension == 'c') {
    //             printf("    %c\n", extension);
    //             // src_files.items[j] = all_files.items[i];
    //             strcpy(src_files.items[j], all_files.items[i]);
    //             printf("    %s\n", src_files.items[j]);
    //             j++;
    //         }
    //     }
    // }
    
    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, "gcc", "-c", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-o", BUILD_FOLDER"main.o", SRC_FOLDER"main.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "gcc", "-c", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-o", BUILD_FOLDER"request.o", SRC_FOLDER"request.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "gcc", "-c", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-o", BUILD_FOLDER"server.o", SRC_FOLDER"server.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "gcc", "-c", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-o", BUILD_FOLDER"socket_comm.o", SRC_FOLDER"socket_comm.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "gcc", "-c", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-o", BUILD_FOLDER"utils.o", SRC_FOLDER"utils.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "gcc", "-MMD", "-Wall", "-Wextra", "-Wno-unused-parameter",\
        "-g", "-std=gnu17", "-fdiagnostics-color=always", "-I"SRC_FOLDER, "-o", "my-http",\
        BUILD_FOLDER"main.o", BUILD_FOLDER"request.o", BUILD_FOLDER"server.o",\
        BUILD_FOLDER"socket_comm.o", BUILD_FOLDER"utils.o");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    // // Let's execute the command synchronously, that is it will be blocked until it's finished.
    // if (!nob_cmd_run_sync(cmd)) return 1;
    // // Reset the cmd array so you can use it again for another command
    // cmd.count = 0;

    // nob_cc(&cmd);
    // nob_cc_flags(&cmd);
    // nob_cc_output(&cmd, BUILD_FOLDER "foo");
    // nob_cc_inputs(&cmd, SRC_FOLDER "foo.c");

    // nob_cmd_run_sync_and_reset() resets the cmd for you automatically
    // if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    return 0;
}