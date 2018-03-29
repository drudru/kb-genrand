

#include "common.h"

void validate_args(int argc, char * const argv[]);

bool write_str(int fd, const char * msg);

int PWBase = 0;
int PWLen  = 0;

char table[] = "0123456789" "ABCDEF";

int main(int argc, char * const argv[])
{
    validate_args(argc, argv);

    // Open hardware random number generator
    //
    int rngfd = open("/dev/hwrng", O_RDONLY);
    if (rngfd == -1)
    {
        write_str(2, "Could not open /dev/hwrng\n");
        exit(1);
    }

    U8 inbuff[2];
    U8 outbuff[82];

    int i = 0;
    while (i != PWLen)
    {
        if (read(rngfd, inbuff, 1) != 1)
        {
            write_str(2, "Could not read /dev/hwrng\n");
            exit(1);
        }

        U8 byte = inbuff[0];

        U8 first = byte & 0xf;

        if (first < PWBase)
        {
            outbuff[i] = table[first];
            i++;
        }

        if (i == PWLen)
            break;

        U8 second = byte >> 4;

        if (second < PWBase)
        {
            outbuff[i] = table[second];
            i++;
        }

    }

    outbuff[i] = '\n';
    i++;
    outbuff[i] = 0;

    if (write(1, outbuff, i) != i)
    {
        write_str(2, "Could not write result.\n");
        exit(1);
    }
}

void arg_help()
{
    write_str(2, "usage: kb-genrand <base> <length>\n");
    write_str(2, "       base must be either 10 or 16\n");
    write_str(2, "       length must be between 8 and 80\n");
    write_str(2, "\n");
    write_str(2, "   ex: kb-genrand 16 64\n");
    exit(1);
}

void validate_args(int argc, char * const argv[])
{
    if (argc != 3)
        arg_help();

    PWBase = atoi(argv[1]);

    if ((PWBase != 10) && (PWBase != 16)) 
        arg_help();


    PWLen = atoi(argv[2]);

    if ((PWLen < 8) ||(PWLen > 80)) 
        arg_help();
}

bool write_str(int fd, const char * msg)
{
    int len = strlen(msg);
    if (write(fd, msg, len) != len)
        return false;

    return true;
}
