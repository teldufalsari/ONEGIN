#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

//!Maximal length of names/paths to files.
#define MAXNAME 1024
//! Maximal number of tries to enter name before the program terminates.
#define MAXTRIES 5

/*!
 *Import function is used to to import the text file into the memory.
 * If it fails to open the file, it terminates program with exit code -1.
 * @param [out] size Size of the imported file (bytes).
 * @param [out] text Pointer to the imported text string.
 * @return - Pointer if file was opened succesfully;
 * - Exit code -1 if failed to open file.
 * @code
 {
    printf("What is going to be sorted?\n");
    printf("(Enter either full path to the file or name of the file in the program's root folder)\n");

    char *name = NULL;
    for (int k = 0; k < MAXTRIES; k++)
    {
        name = getname();
        if ( (text = f_read(name, size) ) != NULL )
        {
            free(name);
            return text;
        }
        printf("Unable to open the file. Try again\n");
    }

    free(name);
    printf("You've failed too many tries. Find out the exact path to file and begin again.\n");
    exit(-1);
    @endcode
}
 */
char* import (size_t* size, char* text, char* name);
/*!
 *
 * @return Trimmed string - name of the file.
 * @code
 {
    char buffer[MAXNAME] = "";
    int k = 0;
    for (; k < MAXTRIES; k++)
    {
        fgets(buffer, MAXNAME, stdin);
        if ( (buffer[MAXNAME - 2] == '\0') || (buffer[MAXNAME - 2] == '\n') )
            break;
        printf("Too long path to file. Try again\n");
        fflush(stdin);
    }
    if (k == MAXTRIES)
    {
        printf("The path limit is 1022 characters. Move the file into another directory and begin again\n");
        exit(-2);
    }

    int i = 0;
    while (buffer[i] != '\n')
        i++;

    char* name = calloc(i, sizeof(char));

    for (int j = 0; j < i; j++)
        *(name + j) = *(buffer + j);

    return name;
}
 @endcode
 */
void getname (char* buffer);
/*!
 *This function reads a string containing file name and converts it to a string suitable to use in fopen() function.
 * Since fgets() function loads strings with 'new sreing' character and of pre-defined size, we have to create a string of desired size
 * and without 'new string'.
 * @param [in] name Name of the file to open.
 * @param [out] size Size of the file that was read (bytes)
 * @return - Pointer to the read text string if file was opened succesfully.
 * - NULL if failed to open the file by given name.
 * @code
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
    {
        *(text + i) = c;
        i++;
    }

    fclose(input);
    return text;
 * @endcode
 */
char* f_read (char *name, size_t* size);
/*!
 *This function looks for strings in the imported file, counts them and generates an array that contains pointers to those strings.
 * @param [in] text The text array that should be split into strings.
 * @param [in] size Size of the text array.
 * @param [out] num_of_strings Number of generated strings.
 * @return Pointer to the array of pointers to strings.
 * @code
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
 * @endcode
 */
char** f_split(char* text, size_t size, size_t *num_of_strings);
/*!
 *This function compares two strings just as strcmp() does but in from the end to the beginning.
 * It will also ignore non-letter characters before (after?) the words.
 * @param [in] str1 The first string.
 * @param [in] str2 The second string.
 * @return Result of the comparision.
 @code
 {
    size_t i1 = strlen(*str1) - 1, i2 = strlen(*str2) - 1;

    while( (!isalpha( *(*str1 + i1) ) ) && (i1 > 0 ) )
        i1--;
    while( (!isalpha( *(*str2 + i2) ) ) && (i2 > 0) )
        i2--;

    while ( (i1 > 0) && (i2 > 0) )
    {
        int d = *(*str1 + i1--) - *(*str2 + i2--);
        if (d != 0)
            return d;

    }

    return i1 - i2;
}
 @endcode
 */
int compare_end (const char** str1, const char** str2);
/*!
 *This function returns strcmp() value but ignoring non-letter characters before words.
 * @param [in] str1 The first string.
 * @param [in] str2 The second string.
 * @return Result of the comparision.
 *
 * @code
{
    size_t i1 = 0, i2 = 0;
    while( (!isalpha( *(*str1 + i1) ) ) && ( *(*str1 + i1) != '\n') )
        i1++;
    while( (!isalpha( *(*str2 + i2) ) ) && ( *(*str2 + i2) != '\n') )
        i2++;

    return strcmp(*str1 + i1, *str2 + i2);
}
 *@endcode
 */
int compare_head (const char** str1, const char** str2);
/*!
 *This function simply creates an output file snd prints the text to this file string by string.
 * @param address Name/address of the file where result should be written.
 * @param num_of_strings Number of strings to write.
 * @param index Pointer to the array of pointers to strings.
 * @code
 {
    FILE* output = fopen(address, "w");
    for(size_t i = 0; i < num_of_strings; i++)
        fputs(*(index + i), output);
    fclose(output);
}
 * @endcode
 */
void dump (char* address, size_t num_of_strings, char** index);

void f_send (char* str, FILE* box);


char* add_to_name (char* name, char* addition);

/*!
 * Main function is of course used to control other functions.
 * It does following:
 * - Prints greeting phrase;
 * - Imports text file as a single string;
 * - Sorts the strings in two different ways and sends the result into two text files;
 * - Sets allocated memory free;
 * @code
    char *text = NULL;
    size_t size = 0, n_str = 0;


    text = import(&size, text);

    char** index = f_split(text, size, &n_str);

    qsort(index, n_str, sizeof(char*), (int (*)(const void *, const void *)) compare_head);
    dump("Sorted from head.txt", n_str, index);

    qsort(index, n_str, sizeof(char*), (int (*)(const void *, const void *)) compare_end);
    dump("Sorted from end.txt", n_str, index);

    system("pause");

    free(text);
    free(index);
    return 0;
 * @endcode
 */
int main ()
{
    setlocale(LC_ALL, "Russian");
    char *text = NULL, name[MAXNAME] = "";
    size_t size = 0, n_str = 0;

    printf("Welcome to ONEGIN file sort\n");
    printf("Version 1.03\n");
    printf("Copyright @teldufalsari\n\n");

    text = import(&size, text, name);

    char** index = f_split(text, size, &n_str);
    char new_name [MAXNAME] = "";


    qsort(index, n_str, sizeof(char*), (int (*)(const void *, const void *)) compare_head);
    strcpy(new_name, name);
    dump(add_to_name(new_name, "_sorted_dy_heads.txt"), n_str, index);

    qsort(index, n_str, sizeof(char*), (int (*)(const void *, const void *)) compare_end);
    strcpy(new_name, name);
    dump(add_to_name(new_name, "_sorted_dy_ends.txt"), n_str, index);

    printf("File has been sorted succesfully!\n");
    system("pause");

    free(text);
    free(index);
    return 0;
}

char* f_read (char *name, size_t* size)
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
    {
        *(text + i) = c;
        i++;
    }

    fclose(input);
    return text;
}

char** f_split(char* text, size_t size, size_t *num_of_strings)
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

int compare_head (const char** str1, const char** str2)
{
    size_t i1 = 0, i2 = 0;
    while( (!isalpha( *(*str1 + i1) ) ) && ( *(*str1 + i1) != '\n') )
        i1++;
    while( (!isalpha( *(*str2 + i2) ) ) && ( *(*str2 + i2) != '\n') )
        i2++;

    return strcmp(*str1 + i1, *str2 + i2);
}

/*int compare_end (const char** str1, const char** str2)
{
    size_t i1 = strlen(*str1) - 1, i2 = strlen(*str2) - 1;

    while( (!isalpha( *(*str1 + i1) ) ) && (i1 > 0 ) )
        i1--;
    while( (!isalpha( *(*str2 + i2) ) ) && (i2 > 0) )
        i2--;

    while ( (i1 > 0) && (i2 > 0) )
    {
        int d = *(*str1 + i1--) - *(*str2 + i2--);
        if (d != 0)
            return d;

    }

    return strlen(*str1) - strlen(*str2);
}*/

int compare_end (const char** str1, const char** str2)
{
    size_t i1 = strlen(*str1) - 1, i2 = strlen(*str2) - 1;

    while( (!isalpha( *(*str1 + i1) ) ) && (i1 > 0 ) )
        i1--;
    while( (!isalpha( *(*str2 + i2) ) ) && (i2 > 0) )
        i2--;

    while ( (i1 > 0) && (i2 > 0) )
    {
        int d = *(*str1 + i1--) - *(*str2 + i2--);

        while( (!isalpha( *(*str1 + i1) ) ) && (i1 > 0 ) )
            i1--;
        while( (!isalpha( *(*str2 + i2) ) ) && (i2 > 0) )
            i2--;

        if (d != 0)
            return d;

    }

    return strlen(*str1) - strlen(*str2);
}

void getname(char *buffer)
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
        exit(-2);
    }

    int i = 0;
    while (buffer[i] != '\n')
        i++;
    buffer[i] = '\0';

}

void dump(char* address, size_t num_of_strings, char** index)
{
    FILE* output = fopen(address, "w");
    for(size_t i = 0; i < num_of_strings; i++)
        f_send(*(index + i), output);
    fclose(output);
}

char* import (size_t* size, char* text, char* name)
{
    printf("What is going to be sorted?\n");
    printf("(Enter either full path to the file or name of the file in the program's root folder)\n");

    for (int k = 0; k < MAXTRIES; k++)
    {
        getname(name);
        if ( (text = f_read(name, size) ) != NULL )
        {
            return text;
        }
        printf("Unable to open the file. Try again\n");
    }

    printf("You've failed too many tries. Find out the exact path to file and begin again.\n");
    exit(-1);
}

void f_send (char* str, FILE* box)
{
    for (size_t i = 0; i < strlen(str); i++)
        if   ( (isalpha(str[i])) &&  (str[i] != 'I') && (str[i] != 'V') && (str[i] != 'X') && (str[i] != 'L') )
        {
            fputs(str, box);
            break;
        }
}

char* add_to_name (char* name, char* addition)
{
    size_t length = 0 ;
    for(; name[length] != '.' ; length++)
        ;
    *(name + length) = '\0';

    return strcat(name, addition);
}