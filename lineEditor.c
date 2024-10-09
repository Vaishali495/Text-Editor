#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct fileLine
{
    char *data;
    int lineNumber;
    struct fileLine *next;
} FileLine;
FileLine *head = NULL;

typedef struct stack
{
    FileLine *FileLineCopy;
    struct stack *next;
} Stack;
Stack *undoHead = NULL;
Stack *redoHead = NULL;

void pushToStack(Stack **top, FileLine *copiedData)
{
    Stack *newNode = (Stack *)malloc(sizeof(Stack));
    newNode->FileLineCopy = copiedData;
    newNode->next = *top;
    *top = newNode;
}

FileLine *copyFileLine(FileLine *top)
{
    if (top == NULL)
        return NULL;
    FileLine *newnode = (FileLine *)malloc(sizeof(FileLine));
    newnode->data = (char *)malloc(strlen(top->data) + 1);
    strcpy(newnode->data, top->data);
    newnode->lineNumber = top->lineNumber;
    newnode->next = copyFileLine(top->next);
    return newnode;
}

FileLine *popFromStack(Stack **top)
{
    if (top == NULL)
        return NULL;
    Stack *temp = *top;
    *top = (*top)->next;
    FileLine *fileState = temp->FileLineCopy;
    free(temp);
    return fileState;
}

void freeLine(FileLine *top)
{
    FileLine *temp;
    while (top != NULL)
    {
        temp = top;
        top = top->next;
        free(temp->data);
        free(temp);
    }
}

void undoTask()
{
    if (undoHead == NULL)
    {
        printf("\nNothing to undo");
        return;
    }
    pushToStack(&redoHead, copyFileLine(head));
    FileLine *prevState = popFromStack(&undoHead);
    freeLine(head);
    head = prevState;
    printf("Undo successful");
}

void redoTask()
{
    if (redoHead == NULL)
    {
        printf("\nNothing to redo");
        return;
    }
    pushToStack(&undoHead, copyFileLine(head));
    FileLine *nextState = popFromStack(&redoHead);
    freeLine(head);
    head = nextState;
    printf("Redo successful");
}

void readText(FILE *fp)
{
    printf("\nreadText function called");
    int lineNo = 1;
    char text[100];
    while (fgets(text, sizeof(text), fp) != NULL)
    {
        FileLine *newNode = (FileLine *)malloc(sizeof(FileLine));
        newNode->data = (char *)malloc(strlen(text) + 1);

        strcpy(newNode->data, text);
        newNode->next = NULL;
        newNode->lineNumber = lineNo;

        if (head == NULL)
        {
            head = newNode;
        }

        else
        {
            FileLine *temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        lineNo++;
    }
}

void insertLine(char *str)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    FileLine *newNode = (FileLine *)malloc(sizeof(FileLine));
    newNode->data = (char *)malloc(strlen(str) + 1);
    strcpy(newNode->data, str);
    strcat(newNode->data, "\n");
    newNode->next = NULL;
    if (head == NULL)
    {
        head = newNode;
        newNode->lineNumber = 1;
        printf("\nData inserted in the first line");
        return;
    }
    FileLine *temp = head;
    int lineNo = 1;
    while (temp->next != NULL)
    {
        temp = temp->next;
        lineNo++;
    }
    temp->next = newNode;
    newNode->lineNumber = lineNo + 1;
    // newNode->lineNumber = temp->lineNumber + 1;

    printf("\nData inserted in the new line");
}

void insertAtLine(char *str, int lineNo, int pos)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    FileLine *temp = head;
    while (temp != NULL)
    {
        if (temp->lineNumber == lineNo)
        {
            int len1 = strlen(temp->data);
            int len2 = strlen(str);

            if (pos > len1)
            {
                printf("\nInvalid position %d. The position is greater than the length of the line.", pos);
                return;
            }

            char *newStr = (char *)malloc(len1 + len2 + 1);
            strncpy(newStr, temp->data, pos);                  // copy before text
            strcpy(newStr + pos, str);                         // insert new text
            strcat(newStr + pos + len2, " ");                  // add space after insert text
            strcpy(newStr + pos + len2 + 1, temp->data + pos); // remaining

            free(temp->data);
            temp->data = newStr;
            printf("\nData inserted at position %d in line %d", pos, lineNo);
            return;
        }
        temp = temp->next;
    }
    printf("\nLine not found!!\nTry Again");
}

void insertText(char *str)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    if (head == NULL)
    {
        // file is empty then insert at beg
        FileLine *newNode = (FileLine *)malloc(sizeof(FileLine));
        newNode->data = (char *)malloc(strlen(str) + 1);
        strcpy(newNode->data, str);
        newNode->next = NULL;
        newNode->lineNumber = 1;
        head = newNode;
        printf("\nText Inserted successfully");
        return;
    }
    // if file has some content
    FileLine *temp = head;
    int lineNo = 1;
    while (temp->next != NULL)
    {
        temp = temp->next;
        lineNo++;
    }
    int len1 = strlen(temp->data);
    int len2 = strlen(str);
    temp->data = realloc(temp->data, len1 + len2 * sizeof(char) + 1);
    // strcat(temp->data, " ");                //only need when cursor is not in the next line
    strcat(temp->data, str);
    temp->lineNumber = lineNo + 1;
    printf("\nSuccessfuly inserted text");
}

void writeTextToFile(FILE *fp)
{
    FileLine *temp = head;
    if (head == NULL)
    {
        printf("\nFile is empty");
        return;
    }
    while (temp != NULL)
    {
        fprintf(fp, "%s", temp->data);
        temp = temp->next;
    }
    printf("\nCheck your file");
}

void displayLine(int lineNo)
{
    if (head == NULL)
    {
        printf("\nFile is Empty");
        return;
    }
    printf("\nContents of the line: \n");
    FileLine *temp = head;
    while (temp->next != NULL)
    {
        if (temp->lineNumber == lineNo)
        {
            printf("\nData of line");
            printf("%s", temp->data);
            break;
        }
        temp = temp->next;
    }
    printf("\nDisplay line exit");
}

void displayFileLine()
{
    if (head == NULL)
    {
        printf("\nFile is empty");
        return;
    }
    FileLine *temp = head;
    while (temp != NULL)
    {
        printf("%s", temp->data);
        temp = temp->next;
    }
}

void searchWord(char *word, int *lineNo, int *pos)
{
    FileLine *temp = head;
    while (temp != NULL)
    {
        char *position = strstr(temp->data, word);
        if (position != NULL)
        {
            *lineNo = temp->lineNumber;
            *pos = position - temp->data;
            return;
        }
        temp = temp->next;
    }
}

void replaceWord(int lineNo, int pos, char *word)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    FileLine *temp = head;

    while (temp != NULL)
    {
        if (temp->lineNumber == lineNo)
        {
            int datalen = strlen(temp->data);
            int endpos = pos;
            while (endpos < datalen && temp->data[endpos] != ' ' && temp->data[endpos] != '\n')
                endpos++;
            int beforelen = pos;
            int afterlen = datalen - endpos;
            char *newStr = (char *)malloc(beforelen + strlen(word) + afterlen + 1);
            strncpy(newStr, temp->data, beforelen);
            strcpy(newStr + beforelen, word);
            strcpy(newStr + beforelen + strlen(word), temp->data + endpos);

            free(temp->data);
            temp->data = newStr;
            printf("\nWord replace successfully");
        }
        temp = temp->next;
    }
}

void deleteline(int lineNo)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    if (head == NULL)
        printf("\nFile is empty");
    FileLine *temp = head;
    FileLine *prev = NULL;
    // when lineNo == 1
    if (temp != NULL && temp->lineNumber == lineNo)
    {
        head = temp->next;
        temp->next = NULL;
        printf("\nline %d deleted", temp->lineNumber);
        return;
    }
    while (temp != NULL && temp->lineNumber != lineNo)
    {
        prev = temp;
        temp = temp->next;
    }
    prev->next = temp->next;
    temp->next = NULL;
    printf("\n line %d deleted Successfully", temp->lineNumber);
}

void deleteWord(char *word, int lineNo, int pos)
{
    // push current state to undo
    pushToStack(&undoHead, copyFileLine(head));

    FileLine *temp = head;
    while (temp != NULL)
    {
        if (temp->lineNumber == lineNo)
        {
            int datalen = strlen(temp->data);
            int endpos = pos;
            while (endpos < datalen && temp->data[endpos] != ' ' && temp->data[endpos] != '\n')
                endpos++;
            int beforelen = pos;
            int afterlen = datalen - endpos;
            char *newStr = (char *)malloc(beforelen + afterlen + 1);
            strncpy(newStr, temp->data, beforelen);
            strcpy(newStr + beforelen, temp->data + endpos);

            free(temp->data);
            temp->data = newStr;
            printf("\nWord deleted successfully");
        }
        temp = temp->next;
    }
}
void displayText()
{
    printf("\nContents of the file: \n");
    FileLine *temp = head;
    while (temp != NULL)
    {
        printf("line Number: %d\tcontent: %s", temp->lineNumber, temp->data);
        temp = temp->next;
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Discard characters until end of line or end of file
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char str1[100];
    // Handle Command Line Arguments
    if (argc == 1)
    {
        fp = fopen("newfile.txt", "w+");
        printf("\nFile open when argc is 1");
        if (fp == NULL)
            printf("Not able to open the file");
    }

    else if (argc == 2)
    {
        fp = fopen(argv[1], "r+");
        printf("\nFile open when argc is 2");
        if (fp == NULL)
        {
            fp = fopen(argv[1], "w+");
            if (fp == NULL)
                printf("\nNot able to open the file");
        }
        readText(fp);
    }

    else if (argc == 3)
    {
        strcpy(str1, argv[2]);
        strcat(str1, "\\");
        strcat(str1, argv[1]);
        fp = fopen(str1, "w+");
    }
    // More than 2 arguments
    else
    {
        printf("\nError Can't able to open the file!!!!\nDo not enter more than two arguments");
        exit(1);
    }

    int ch, lineNo, pos, choice;
    char str[50], newWord[50];
    while (1)
    {
        printf("\n1. Insert\n2. Read\n3. Update\n4. Delete\n5. Find\n6. Undo\n7. Redo\n0. Exit\nEnter your choice: ");
        // scanf("%d", choice);
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue; // Start the loop again
        }
        switch (choice)
        {
        case 1:
            printf("\n1. Insert a new line\n2. Insert at particular line\n3. Insert at cursor position\nEnter your choice: ");
            scanf("%d", &ch);
            if (ch == 1)
            {
                printf("\nEnter the data: ");
                fflush(stdin);
                scanf("%[^\n]s", str);
                fflush(stdin);
                insertLine(str);
            }
            else if (ch == 2)
            {
                printf("\nEnter the data: ");
                fflush(stdin);
                scanf("%[^\n]s", str);
                fflush(stdin);
                printf("Enter Line Number and position: ");
                scanf("%d %d", &lineNo, &pos);
                insertAtLine(str, lineNo, pos);
            }
            else if (ch == 3)
            {
                printf("\nEnter the data: ");
                fflush(stdin);
                scanf("%[^\n]s", str);
                fflush(stdin);
                insertText(str);
            }
            else
                printf("\nWrong choice!!Try again");
            displayText();
            break;
        case 2:
            printf("\n1. Display the content of the particular line\n2. Read the content of the file line by line\nEnter your choice: ");
            scanf("%d", &ch);
            if (ch == 1)
            {
                printf("\nEnter the line number: ");
                scanf("%d", &lineNo);
                displayLine(lineNo);
            }
            else if (ch == 2)
            {
                printf("\nContent of the file line by line: \n");
                displayFileLine();
            }
            else
                printf("\nWrong choice");
            displayText();
            break;
        case 3:
            printf("\n1. Replacing a word with another\n2. Insert a new word at cursor position\nEnter your choice: ");
            scanf("%d", &ch);
            if (ch == 1)
            {
                printf("\nEnter the new word and existing word: ");
                scanf("%s %s", newWord, str);
                searchWord(str, &lineNo, &pos);
                printf("Pos = %d Line Number = %d", pos, lineNo);
                if (pos != -1)
                    replaceWord(lineNo, pos, newWord);
            }
            else if (ch == 2)
            {
                printf("\nEnter the Text: ");
                fflush(stdin);
                scanf("%[^\n]s", str);
                fflush(stdin);
            }
            else
                printf("\nWrong Choice!!");
            break;
        case 4:
            printf("\n1. Delete Line\n2. Delete word\nEnter your choice: ");
            scanf("%d", &ch);
            if (ch == 1)
            {
                printf("\nEnter the line number: ");
                scanf("%d", &lineNo);
                deleteline(lineNo);
            }
            else if (ch == 2)
            {
                printf("\nEnter the word: ");
                scanf("%s", str);
                searchWord(str, &lineNo, &pos);
                deleteWord(str, lineNo, pos);
            }
            else
                printf("Wrong Choice!!");
            break;
        case 5:
            printf("\nEnter the word to find: ");
            scanf("%s", str);
            searchWord(str, &lineNo, &pos);
            printf("\nLine Number: %d\tPosition: %d", lineNo, pos);
            break;
        case 6:
            undoTask();
            break;
        case 7:
            redoTask();
            break;
        case 0:
            fclose(fp);
            if (argc == 1)
                fp = fopen("newfile.txt", "w+");
            else if (argc == 2)
                fp = fopen(argv[1], "w+");
            if (fp != NULL)
            {
                writeTextToFile(fp);
            }
            exit(1);
        default:
            printf("\nWrong choice!!!!");
        }
    }
    if (fp != NULL)
    {
        fclose(fp);
    }
    return 0;
}









// displayText();
// copy the top operation to the redo stack
// undo stack se pop krke FileLine type ke variable bnaya prev state usme daal diya
// free FileLine - free all line
// head ke and prev state daldi(head = prevstate)