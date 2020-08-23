#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int match(char* test, char* pattern, int nvars, char* varsbuff)
{
    int len_test = strlen(test);
    int len_pattern = strlen(pattern);
    if (len_test != len_pattern || len_test == 0)
        return 0;

    //reset vars buff
    for (int i = 0; i < nvars; i++)
    {
        varsbuff[i] = 0x00;
    }
    
    int matches = 1;
    char* curr = &varsbuff[pattern[0] - 0x61]; // a is location 0, b
                                               // is location 1,
                                               // etc... 
    *curr = test[0];
    for (int i = 1; (i < len_test) && matches; i++)
    {
        curr = &varsbuff[pattern[i] - 0x61]; // a -> 0, b -> 1 etc
        
        if (*curr == 0x00)            // check if variable is already set
            *curr = test[i];          // it is not, so we initialize it
        else if (!(*curr == test[i])) // already set, so we test if
                                      // the varbuff variable is the
                                      // same as the one the test word
            matches = 0;
    }
    return matches;
}

int main(int argc, char** argv)
{
    extern char *optarg;
    extern int optind;
    int flag_basic = 0;
    int flag_resultonly = 0;
    char c;
    char* options = "sr";

    opterr = 0;
    
    while ((c = getopt(argc, argv, options)) != -1)
    {
        switch (c)
        {
        case 's':
            flag_basic = 1;
            break;
        case 'r':
            flag_resultonly = 1;
            break;
        case '?':
            printf("usage: %s [options] <pattern> <path-to-dictionary>\n", argv[0]);
            printf("options:\n");
            for (int i = 0, len = strlen(options); i < len; i++)
                printf("\t-%c\n", options[i]);
            break;
        }
    }
    if ((optind + 2) > argc)
    {
        printf("usage: %s [options] <pattern> <path-to-dictionary>\n", argv[0]);
        return 1;
    }

    FILE* dictionary = fopen(argv[optind + 1], "r");
    if (!dictionary)
    {
        printf("could not open dictionary file named '%s'.\n", argv[optind + 1]);
        return 2;
    }
    
    char pattern[128];
    strncpy(pattern, argv[optind], 127);
    pattern[128] = 0x00;
    
    for (char* p = pattern; *p; ++p) *p = tolower(*p); // lower case
                                                       // pattern
    int plen = strlen(pattern);
    char* uniquechars = calloc(plen, sizeof(char));
    if (!uniquechars)
    {
        printf("something went wrong");
        return 3;
    }

    int nvars = 0;
    int unique = 1;
    for (int i = 0; i < plen; i++)
    {
        char* p;
        unique = 1;
        for (p = uniquechars; *p; p++)
        {
            if (pattern[i] == *p)
            {
                unique = 0;
            }
        }
        if (unique)
        {
            *p = pattern[i];
            nvars++;
        }

    }
    free(uniquechars);
    
    char* varbuff = calloc(nvars, sizeof(char));
    if (!varbuff)
    {
        printf("could not allocate memory for chars. Maybe the number of \
chars is too big?\n");
        return 5;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    long long lines = 0;
    long long matches = 0;
    
    while ((read = getline(&line, &len, dictionary)) != -1)
    {
        line[read - 1] = 0x00; // remove trailing newline
        if (match(line, pattern, nvars, varbuff))
        {
            if (!flag_resultonly)
                printf("%s\n", line);
            matches++;
        }
        lines++;
    }
    if (!flag_basic)
        printf("searched %lld words\n%lld matches\n", lines, matches);

    free(varbuff);
    free(line);
    
    return 0;
}
