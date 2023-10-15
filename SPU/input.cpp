#include <assert.h>
#include <ctype.h>

#include "input.h"
#include "../common/colorlib.h"

static void ClearInput(FILE* fp);
static bool ReadElem(elem_t* val);
static bool RepeatQuestion();

ERRORS GetElement(elem_t* val)
{
    bool get_flag = ReadElem(val);

    while (!get_flag)
    {
        bool repeat_flag = RepeatQuestion();

        if (!repeat_flag)
            return ERRORS::USER_QUIT;

        get_flag = ReadElem(val);
    }

    return ERRORS::NONE;
}

//------------------------------------------------------------------------------------------------------------------

static void ClearInput(FILE* fp)                       // clears input from '\n' char and else trash
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

//------------------------------------------------------------------------------------------------------------------

static bool ReadElem(elem_t* val)
{
    assert(val);

    printf("Input pushing element: ");

    while (scanf("%lld", val) == 0)
    {
        ClearInput(stdin);

        PrintRedText(stdout, "INVALID ELEMENT\n", "");

        return false;
    }
    int character = 0;
    while ((character = getchar()) != '\n' && character != EOF)
    {
        if (!isspace(character))
            {
                PrintRedText(stdout, "INVALID ELEMENT\n", "");
                ClearInput(stdin);
                return false;
            }
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------

static bool RepeatQuestion()          // asks user for repeat
{
    printf("Do you want to try again? (1 - Yes): ");

    int repeat_flag = false;
    scanf("%d", &repeat_flag);
    ClearInput(stdin);

    if (repeat_flag != 1) printf("Bye Bye");    // invalid answer
    return (repeat_flag == 1) ? true : false;
}
