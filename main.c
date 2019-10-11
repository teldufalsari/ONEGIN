#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

//!Maximal size of the name of the file. The final 24 symbols are reserved for suffixes like "sorted".
#define MAXNAME 1024
//!Maximal number of attempts to enter name before program terminates.
#define MAXTRIES 5

/*!
 *
 * @param [out]size Size of the opened file in bytes.
 * @param [out]text Pointer to the array where the text is stored.
 * @param [out]name Name of the opened file (or path to it).
 * @return Pointer to the text array.
 *
 * This function is used to to import the text file into the memory.
 * If it fails to open the file, it terminates program with exit code -1.
 */
char* Import (size_t* size, char* text, char* name);
/*!
 *
 * @param buffer Array where the name is stored.
 * This function requests file name and replaces new line symbol with zero symbol, so the name becomes usable.
 */
void GetName (char* buffer);
/*!
 *
 * @param [in]name Name of the file to open.
 * @param [out]size Size of the opened file in bytes.
 * @return Pointer to the array where the text is stored.
 * This function reads text file by given name.
 */
char* Read (char *name, size_t* size);
/*!
 *
 * @param [in]text Pointer to the array with the text to sort.
 * @param [in]size Size of given array in bytes.
 * @param [out]num_of_strings Number of strings in the file.
 * @return Pointer to the array of pointers, indicating beginnings of the lines.
 * This function finds separate lines in given file and writes pinters to their beginnings in created array.
 * It also adds finishing symbols to the last line.
 */
char** Split(char* text, size_t size, size_t *num_of_strings);
/*!
 *
 * @param [in]str1 First string.
 * @param [in]str2 Second string.
 * @return Comparision result.
 * This function compares strings from their endings ignoring ALL non-alpha characters.
 */
int StrCmpEnd (const char** str1, const char** str2);
/*!
 *
 * @param [in]str1 First string.
 * @param [in]str2 Second string.
 * @return Result of comparision.
 * This function returns strcmp() result but non-alpha symbols before words are ignored.
 */
int StrCmpHead (const char** str1, const char** str2);
/*!
 *
 * @param [in]address Name of the file where strings are printed.
 * @param [in]num_of_strings Number of strings to be printed.
 * @param [in]index Array where pointers to strings beginnings are stored.
 * This function prints all strings to the text file, beginnings of which are listed in index array.
 */
void Dump (char* address, size_t num_of_strings, char** index);
/*!
 *
 * @param [in]str String to be printed.
 * @param [in]dst Pointer to the file where the string is printed.
 * This function checks whether string contains words and then prints it.
 */
void SendString (char* str, FILE* dst);
/*!
 *
 * @param [in, out]name String where suffix is added to.
 * @param [in]addition Suffix to be added.
 * @return Pointer to name string.
 * This function removes extension from the file name and adds suffix with extension. It is used to create output files.
 */
char* ReName (char* name, char* addition);
/*!
 *
 * @param c Character to be checked.
 * @return 1 or 0 if given character is letter or not correspondently.
 * This function works as isalpha from <ctype.h> supporting cyrillic letters.
 */
int IsLiteral(int c);

int main ()
{
    setlocale(LC_ALL, "Russian");
    char *text = NULL, name[MAXNAME] = "";
    size_t size = 0, n_str = 0;

    printf("Welcome to ONEGIN file sort\n");
    printf("Version 1.05\n");
    printf("Copyright @teldufalsari\n\n");

    text = Import(&size, text, name);

    char** index = Split(text, size, &n_str);
    char new_name [MAXNAME] = "";


    qsort(index, n_str, sizeof(char *), (int (*)(const void *, const void *)) StrCmpHead);
    strcpy(new_name, name);
    Dump(ReName(new_name, "_sorted.txt"), n_str, index);

    qsort(index, n_str, sizeof(char *), (int (*)(const void *, const void *)) StrCmpEnd);
    strcpy(new_name, name);
    Dump(ReName(new_name, "_rhymes.txt"), n_str, index);

    printf("File has been sorted successfully!\n");
    system("pause");

    free(text);
    free(index);
    return 0;
}

char* Read (char *name, size_t* size)
{
    FILE* input;

    if ( (input = fopen(name, "rb")) == NULL)
        return NULL;

    struct stat buff;
    fstat (fileno (input), &buff);
    *size = buff.st_size;
    char * text = calloc(*size + 2, sizeof(char));
    for (size_t i = 0; i <= *size; i++)
        *(text + i) = '\r';

    int c = 0, i = 0;
    while ( (c = getc(input)) != EOF)
        *(text + i++) = c;

    fclose(input);
    return text;
}

char** Split(char* text, size_t size, size_t *num_of_strings)
{
    *num_of_strings = 1;
    for (size_t i = 0; i < size; i++)
        if ( *(text + i) == '\n')
            *num_of_strings += 1;

    char **index = calloc(*num_of_strings, sizeof(char*));

    *index = text;
    int j = 1;
    for (size_t i = 0; i < size; i++)
    {
        if ( *(text + i) == '\r')
        {
            *(text + i) = '\n';
            *(text + i + 1) = '\0';
            *(index + j) = text + i + 2;
            j++;
        }
    }

    *(text + size) = '\n';
    *(text + size + 1) = '\0';
    return index;
}

int StrCmpHead (const char** str1, const char** str2)
{
    size_t i1 = 0, i2 = 0;
    while( (!isalpha( *(*str1 + i1) ) ) && ( *(*str1 + i1) != '\n') )
        i1++;
    while( (!isalpha( *(*str2 + i2) ) ) && ( *(*str2 + i2) != '\n') )
        i2++;

    return strcmp(*str1 + i1, *str2 + i2);
}

int StrCmpEnd (const char** str1, const char** str2)
{
    size_t i1 = strlen(*str1) - 1, i2 = strlen(*str2) - 1;

    while( (!IsLiteral(*(*str1 + i1)))  && (i1 > 0 ) )
        i1--;
    while( (!IsLiteral( *(*str2 + i2))) && (i2 > 0) )
        i2--;

    while ( (i1 > 0) && (i2 > 0) )
    {
        int d = *(*str1 + i1--) - *(*str2 + i2--);

        if (d != 0)
            return d;

        while( (!IsLiteral(*(*str1 + i1)) )  && (i1 > 0 ) )
            i1--;
        while( (!IsLiteral( *(*str2 + i2) ) ) && (i2 > 0) )
            i2--;
    }

    return strlen(*str1) - strlen(*str2);
}

void GetName(char *buffer)
{
    int k = 0;
    for (; k < MAXTRIES; k++)
    {
        fgets(buffer, MAXNAME, stdin);
        if ( (buffer[MAXNAME - 24] == '\0') || (buffer[MAXNAME - 24] == '\n') )
            break;
        printf("Too long path to file. Try again\n");
        fflush(stdin);
    }
    if (k == MAXTRIES)
    {
        printf("The path limit is 1000 characters. Move the file into another directory and begin again\n");
        system("pause");
        exit(-2);
    }

    int i = 0;
    while (buffer[i] != '\n')
        i++;
    buffer[i] = '\0';

}

void Dump(char* address, size_t num_of_strings, char** index)
{
    FILE* output = fopen(address, "w");
    for(size_t i = 0; i < num_of_strings; i++)
        SendString(*(index + i), output);
    fclose(output);
}

char* Import (size_t* size, char* text, char* name)
{
    printf("What is going to be sorted?\n");
    printf("(Enter either full path to the file or name of the file in the program's root folder)\n");

    for (int k = 0; k < MAXTRIES; k++)
    {
        GetName(name);
        if ((text = Read(name, size) ) != NULL )
        {
            return text;
        }
        printf("Unable to open the file. Try again\n");
    }

    printf("You've failed too many tries. Find out the exact path to file and begin again.\n");
    system("pause");
    exit(-1);
}

void SendString (char* str, FILE* dst)
{
    for (size_t i = 0; i < strlen(str); i++)
        if   ( (isalpha(str[i])) &&  (str[i] != 'I') && (str[i] != 'V') && (str[i] != 'X') && (str[i] != 'L') )
        {
            fputs(str, dst);
            break;
        }
}

char* ReName (char* name, char* addition)
{
    size_t length = 0 ;
    for(; name[length] != '.' ; length++)
        ;
    *(name + length) = '\0';

    return strcat(name, addition);
}

int IsLiteral(int c)
{
    if ( ((c >= -64) && (c <= -1)) || (c == -88) ||(c == -72) || isalpha(c))
        return 1;
    return 0;
}