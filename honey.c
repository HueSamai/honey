/*
* HONEY
*
* by Jayden van Zuydam
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void setflag(char*, int);
int getflag(char c);

void printhelp();

void handle_run();
void handle_test();
void handle_batch();
void handle_gen();
void handle_exec();

void gen_dump(char*, int);

int parse_arg1(char*);
int parse_arg2(char*);

/* returns 0 if succeeded, otherwise failed */
int run_test(char* inpath, char* outpath);

void log_test(int result, char* inpath, char* outpath);

#define ERR(str) do {\
    fprintf(stderr, "\n");\
    while (ch != '\n') ch = fgetc(infile);\
} while (0)

int flags = 0;

int cmd_idx = 0;
int IDX_TO_ARGV_IDX[256];

#define DUMP (!getflag('s'))
#define SHOWDIFF (!getflag('s'))
#define SHOWHELP getflag('h')
#define CLI getflag('c')

char CURRENT_COMMAND[256];

int TESTSRUN = 0;
int TESTSSUCCEEDED = 0;

int failed_idx = 0;
char FAILED_STACK[1000][256];
char FAILED_STACK_OUT[1000][256];

char ch;
FILE *infile;
int main(int argc, char* argv[]) {
    int i = 0;
    char cmd[2048];

    if (argc == 1) {
        printhelp();
        return 0;
    }

    for (i = 1; i < argc; ++i)
        setflag(argv[i], i);


    if (CLI) {
        if (cmd_idx > 0) {
            printhelp();
            return 0;
        }
        infile = stdin;
    }
    else {
        char fullpath[256];
        if (cmd_idx != 1 || SHOWHELP) {
            printhelp();
            return 0;
        }
        sprintf(fullpath, "./honey/tests/%s.honey", argv[IDX_TO_ARGV_IDX[0]]);
        infile = fopen(fullpath, "r");
    }

    ch = fgetc(infile);
    while (ch != EOF) {
        char cmd[4];

        /* ignore whitespace */
        while (isspace(ch)) ch = fgetc(infile);
        if (ch == EOF) break;

        cmd[0] = ch;
        cmd[1] = fgetc(infile);
        cmd[2] = fgetc(infile);
        cmd[3] = 0;

        ch = fgetc(infile);
        if (ch != ' ') {
            ERR("Error with command syntax.");
            continue;
        }

        while (isspace(ch)) ch = fgetc(infile);

        if (!strcmp(cmd, "run")) {
            handle_run();
        } else if (!strcmp(cmd, "tst")) {
            handle_test();
        } else if (!strcmp(cmd, "bat")) {
            handle_batch();
        } else if (!strcmp(cmd, "gen")) {
            handle_gen();
        } else if (!strcmp(cmd, "exe")) {
            handle_exec();
        } else {
            ERR("Invalid command!");
            continue;
        }
    }

    printf("RESULT:\t\t\t%d/%d\n", TESTSSUCCEEDED, TESTSRUN);
    printf("PERCENTAGE:\t\t%.2f\n", (TESTSSUCCEEDED / (float)TESTSRUN) * 100.0);

    if (failed_idx > 0)
        printf("\nFAILED TEST CASES:\n\n");
    for (i = 0; i < failed_idx; ++i) {
        printf(" - %s\n", FAILED_STACK[i]);
        if (SHOWDIFF) {
            sprintf(cmd, "diff -y --suppress-common-lines ./honey/tests/%s ./honey/dump/%s", FAILED_STACK_OUT[i], FAILED_STACK_OUT[i]);
            system(cmd);
        }

        if (SHOWDIFF)
            printf("\n\n\n");
    }

    if (!CLI) fclose(infile);

    return 0;
}

int parse_arg1(char* buffer) {
    int i = 0;
    while (ch != ' ') {
        buffer[i++] = ch;
        ch = fgetc(infile);
    }
    buffer[i] = 0;
    return i;
}

int parse_arg2(char* buffer) {
    int i = 0;
    while (ch != '\n') {
        buffer[i++] = ch;
        ch = fgetc(infile);
    }
    buffer[i] = 0;
    return i;
}

void handle_run() {
    int i = 0;
    while (ch != '\n') {
        CURRENT_COMMAND[i++] = ch;
        ch = fgetc(infile);
    }
    CURRENT_COMMAND[i] = 0;
}

void handle_test() {
    char inpath[256], outpath[256];

    parse_arg1(inpath);
    parse_arg2(outpath);

    if (DUMP) gen_dump(outpath, 0);
    log_test(run_test(inpath, outpath), inpath, outpath);
}

void handle_exec() {
    char cmd[1024];

    parse_arg2(cmd);

    system(cmd);
}

void log_test(int result, char* inpath, char* outpath) {
    if (!result) {
        ++TESTSSUCCEEDED;
    } else {
        strcpy(FAILED_STACK[failed_idx], inpath);
        strcpy(FAILED_STACK_OUT[failed_idx++], outpath);
    }
    ++TESTSRUN;
}

void handle_batch() {
    char dir[100], num[100];
    char inpath[256], outpath[256];
    int max;

    int i = 0;
    i = parse_arg1(dir);
    if (dir[i - 1] == '/') --i;
    dir[i] = 0;
    
    parse_arg2(num);
    max = atoi(num);

    if (DUMP) gen_dump(dir, 1);

    for (i = 1; i <= max; ++i) {
        sprintf(inpath, "%s/%d.in", dir, i);
        sprintf(outpath, "%s/%d.out", dir, i);

        log_test(run_test(inpath, outpath), inpath, outpath);
    }
}

void handle_gen() {
    char dir[100], num[100];
    char cmd[1024];
    int max;

    int i = 0;
    i = parse_arg1(dir);
    if (dir[i - 1] == '/') --i;
    dir[i] = 0;
    
    parse_arg2(num);
    max = atoi(num);

    for (i = 1; i <= max; ++i) {
        sprintf(cmd, "%s ./honey/tests/%s/%d.in > ./honey/tests/%s/%d.out 2>&1", CURRENT_COMMAND, dir, i, dir, i);
        system(cmd);
    }
}

int run_test(char* inpath, char* outpath) {
    char cmd[2048];
    char fullpath[256];
    char dumppath[512];
    FILE *out, *expected_out;
    int difference = 0;

    if (DUMP) {
        sprintf(cmd, "%s ./honey/tests/%s > ./honey/dump/%s 2>&1", CURRENT_COMMAND, inpath, outpath);
    }
    else
        sprintf(cmd, "%s ./honey/tests/%s 2>&1", CURRENT_COMMAND, inpath);

    sprintf(dumppath, "./honey/dump/%s", outpath);
    if (DUMP) {
        system(cmd);
        out = fopen(dumppath, "r");
    }
    else
        out = popen(cmd, "r");

    sprintf(fullpath, "./honey/tests/%s", outpath);

    expected_out = fopen(fullpath, "r");

    for (;;) {
        char c, d;
        c = fgetc(out);
        d = fgetc(expected_out);

        if (c != d) ++difference; 
        if (c == EOF && d == EOF) break;
    }

    fclose(expected_out);
    if (DUMP)
        pclose(out);
    else
        fclose(out);

    return difference;
}

void setflag(char* str, int idx) {
    if (*str++ != '-') {
        IDX_TO_ARGV_IDX[cmd_idx++] = idx;
        return;
    }
    if (*str < 'a' || *str > 'z') return; 
    flags |= 1 << (*str - 'a');
}

int getflag(char c) {
    return flags & (1 << (c - 'a'));
}

void gen_dump(char* outpath, int isdir) {
    char cmd[1024];
    if (isdir)
        sprintf(cmd, "mkdir -p -- ./honey/dump/%s", outpath);
    else
        sprintf(cmd, "mkdir -p -- ./honey/dump/%s.k && rm -rf ./honey/dump/%s.k", outpath, outpath);
    system(cmd);
}

void printhelp() {
    printf("TL;DR: ./honeyc [testfile]\n\nUsage: honeyc [flags] [testfile] (prepends 'honey/tests/' and adds '.honey')\nPossible flags: -c(li) -s(implified output) -h(elp)\n\nNOTE: if you are running honey with '-c' you should NOT have any commands. Just run: ./honeyc -c\n");
}
