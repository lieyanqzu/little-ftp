#include "conf.h"

int check_login(char *loginstr)
{
    FILE *fp;
    char line[MAXLINE];
    int ret = -1;

    if ((fp = lab_locate("[account]")) == NULL)
        err_quit("file lab_locate error");

    while (1) // keep comparing
    {
        Fgets(line, MAXLINE, fp);
        trimstr(line, strlen(line));

        if (strlen(line) == 0)
        {
            ret = -1;
            break;
        }
        // login success
        if (strcmp(line, loginstr) == 0)
        {
            ret = 0; // gotcha
            break;
        }
    }
    Fclose(fp);
    return ret;
}

int check_workdir(char *dir)
{
    FILE *fp;
    char line[MAXLINE];
    int ret = -1;

    if ((fp = lab_locate("[workdir]")) == NULL)
        err_quit("file lab_locate error");

    while (1) // keep comparing
    {
        Fgets(line, MAXLINE, fp);
        trimstr(line, strlen(line));

        if (strlen(line) == 0)
        {
            ret = -1;
            break;
        }
        // still in workdir
        if (strncmp(line, dir, strlen(line)) == 0)
        {
            ret = 0; // gotcha
            break;
        }
    }
    Fclose(fp);
    return ret;
}

int get_capab()
{
    FILE *fp;
    char line[MAXLINE];
    int ret = -1;

    if ((fp = lab_locate("[capability]")) == NULL)
        err_quit("file lab_locate error");

    //return capability in next line
    Fgets(line, MAXLINE, fp);
    trimstr(line, strlen(line));
    ret = atoi(line);
    Fclose(fp);
    return ret;
}

FILE *lab_locate(char *label)
{
    FILE *fp;
    char line[MAXLINE];

    fp = Fopen(CONF_FILE, "r");
    fseek((FILE *)fp, SEEK_SET, 0);

    // get label location then return fp
    while (1)
    {
        Fgets(line, MAXLINE, fp);
        trimstr(line, strlen(line));

        if (strcmp(line, label) == 0)
        {
            return fp;
        }
    }
    Fclose(fp);
    return NULL;
}
