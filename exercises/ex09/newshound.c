/* Example from Head First C.
Downloaded from https://github.com/twcamper/head-first-c
Modified by Allen Downey.
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <wait.h>


void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int main(int argc, char *argv[])
{
    int status;
    pid_t pid;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <search phrase>\n", argv[0]);
        return 1;
    }
    const char *PYTHON = "/usr/bin/python2";
    const char *SCRIPT = "rssgossip.py";
    char *feeds[] = {
        "http://www.nytimes.com/services/xml/rss/nyt/Africa.xml",
        "http://www.nytimes.com/services/xml/rss/nyt/Americas.xml",
        "http://www.nytimes.com/services/xml/rss/nyt/MiddleEast.xml",
        "http://www.nytimes.com/services/xml/rss/nyt/Europe.xml",
        "http://www.nytimes.com/services/xml/rss/nyt/AsiaPacific.xml"
    };
    int num_feeds = 5;
    char *search_phrase = argv[1];
    char var[255];

    for (i=0; i<num_feeds; i++) {
        printf("Creating forked feed %d............\n", i);
        pid = fork(); // Create a new fork

        if (pid == -1) {
            fprintf(stderr, "Fork failed: %s\n", strerror(errno));
            perror(argv[0]);
            exit(1);
        }

        if (pid == 0) {
            sleep(i);
            sprintf(var, "RSS_FEED=%s", feeds[i]);
            char *vars[] = {var, NULL};
            int res = execle(PYTHON, PYTHON, SCRIPT, search_phrase, NULL, vars);
            if (res == -1) {
                error("Can't run script.");
            }
            exit(i);
        }        
    }

    for (i=0; i<5; i++) {
        pid = wait(&status); // Wait for the child

        if (pid == -1) {
            fprintf(stderr, "wait failed: %s\n", strerror(errno));
            perror(argv[0]);
            exit(1);
        }

        // check the exit status of the child
        status = WEXITSTATUS(status);
        printf("Feed %d exited with error code %d.\n", pid, status);
    }

    // Parent thread is done waiting for child processes
    printf("Done searching feeds!\n");

    exit(0);
}
