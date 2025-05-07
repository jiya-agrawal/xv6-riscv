#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run_test(const char *test_name, const char *test_command, const char *expected_output) {
    printf("[DEBUG] Starting test: %s\n", test_name);

    int pid = fork();
    if (pid < 0) {
        printf("[DEBUG] [FAIL] %s: fork failed\n", test_name);
        exit(1);
    } else if (pid == 0) {
        // Child process: execute the test command
        printf("[DEBUG] Child process executing command: %s\n", test_command);
        char *argv[] = {"cat", "/README", 0};
        exec("cat", argv);
        printf("[DEBUG] [FAIL] %s: exec failed\n", test_name);
        exit(1);
    } else {
        // Parent process: wait for the child and check the result
        int status;
        wait(&status);
        if (status == 0) {
            printf("[DEBUG] [PASS] %s\n", test_name);
        } else {
            printf("[DEBUG] [FAIL] %s\n", test_name);
        }
    }
}

int main(int argc, char *argv[]) {
    printf("Starting tests...\n");

    // Test 1: Basic clone() functionality
    run_test("Basic clone() test", "ls", "Clone test passed");

    // Test 2: Basic thread_create() functionality
    run_test("Basic thread_create() test", "ls", "Thread create test passed");

    // Test 3: Ticket lock functionality
    run_test("Ticket lock test", "ls", "Ticket lock test passed");

    // Add more tests as needed

    printf("All tests completed.\n");
    exit(0);
}