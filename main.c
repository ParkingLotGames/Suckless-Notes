#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NOTES 1000
#define MAX_NOTE_LENGTH 4096

// The Note struct represents a single note in the db.
// It contains the note's content and a flag indicating whether it's hidden.
typedef struct
{
    char content[MAX_NOTE_LENGTH]; // The note's content (max length defined by MAX_NOTE_LENGTH).
    int hidden;                    // A flag indicating whether the note is hidden (1) or not (0).
} Note;

// The Database struct represents the entire db of notes.
// It contains an array of Note structs and a count of the number of notes in the db.
typedef struct
{
    Note notes[MAX_NOTES]; // An array of Note structs (max size defined by MAX_NOTES).
    int noteCount;         // The current number of notes in the db.
} Database;

// The global variable 'db' represents the current state of the db.
// It's initialized with all fields set to 0.
Database db = {0};

// The global variable 'dbPath' stores the path to the file where the db is saved.
char dbPath[512];

// The saveDb function saves the current state of the db.
// It writes the entire Db struct to the file.
void saveDb()
{
    FILE *file = fopen(dbPath, "w"); // Open the file for writing.
    if (!file)
        return;                                   // If the file couldn't be opened, exit the function.
    fwrite(&db, sizeof(Database), 1, file); // Write the Db struct to the file.
    fclose(file);                                 // Close the file.
}

// The loadDb function loads the db from file.
// It reads the entire Db struct from the file into the 'db' variable.
void loadDb()
{
    const char *homeDir = getenv("HOME");                                  // Get the path to the user's home directory.
    snprintf(dbPath, sizeof(dbPath), "%s/.notes_db", homeDir); // Construct the path to the db.
    FILE *file = fopen(dbPath, "r");                                 // Open the file for reading.
    if (file)
    {
        fread(&db, sizeof(Database), 1, file); // Read the Db struct from the file.
        fclose(file);                                // Close the file.
    }
}

// The clearScreen function clears the terminal screen.
// It executes the appropriate command based on the operating system.
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // Execute the "cls" command for Windows.
#elif __APPLE__
    system("clear"); // Execute the "clear" command for macOS.
#elif __unix__
    system("clear"); // Execute the "clear" command for Unix-like systems.
#else
    system("clear"); // Fallback to "clear" command for unrecognized systems.
#endif
}
// The addNote function allows the user to add a new note to the db.
// It checks if the db is full and, if it's not, it prompts the user to enter the note's content.
// The note's content is read line by line, until the user enters "EOF" or the maximum note length is reached.
// The note is then added to the db, marked as not hidden, and the db is saved.
void addNote()
{
    // Check if the db is full.
    if (db.noteCount >= MAX_NOTES)
    {
        printf("Db full!\n"); // Inform the user if the db is full.
        return;                     // Exit the function if the db is full.
    }

    // Prompt the user to enter the note's content.
    printf("Enter note (EOF to finish):\n");
    char buffer[MAX_NOTE_LENGTH] = {0}; // Initialize a buffer to store the note's content.
    char line[MAX_NOTE_LENGTH];         // Initialize a variable to store each line of the note's content.
    int position = 0;                   // Initialize a variable to keep track of the current position in the buffer.

    // Read the note's content line by line, until the user enters "EOF" or the maximum note length is reached.
    while (printf("> ") && fgets(line, sizeof(line), stdin))
    {
        // Check if the user wants to finish entering the note.
        if (strncmp(line, "EOF", 3) == 0)
            break; // Exit the loop if the user enters "EOF".

        // Check if the note's content is too long.
        int lineLen = strlen(line);
        if (position + lineLen >= MAX_NOTE_LENGTH)
            break; // Exit the loop if the maximum note length is reached.

        // Add the line to the buffer.
        strcat(buffer, line);
        position += lineLen; // Update the current position in the buffer.
    }

    // Add the note to the db.
    strcpy(db.notes[db.noteCount].content, buffer); // Copy the note's content to the db.
    db.notes[db.noteCount].hidden = 0;              // Mark the note as not hidden.
    db.noteCount++;                                       // Increment the note count.
    saveDb();                                             // Save the db.
}
// The displayNotes function is responsible for printing the notes in the db to the console.
// It takes a single parameter, showHidden, which is a flag indicating whether to show hidden notes or not.
// If showHidden is 0, it will only display visible notes. If showHidden is 1, it will only display hidden notes.
void displayNotes(int showHidden)
{
    // Loop through all the notes in the db.
    for (int index = 0; index < db.noteCount; index++)
    {
        // If showHidden is 1 and the note is not hidden, skip this note.
        if (showHidden && !db.notes[index].hidden)
            continue;
        // If showHidden is 0 and the note is hidden, skip this note.
        if (!showHidden && db.notes[index].hidden)
            continue;
        // Print the note number, a visual indicator (either "• " or "[HIDDEN] "), and the note content.
        printf("\x1b[32m%d.\033[0m %s%s\n", index + 1,
               db.notes[index].hidden ? "[HIDDEN] " : "• ",
               db.notes[index].content);
    }
}

// The processNumbers function is responsible for parsing a string of comma-separated numbers into an array of integers.
// It takes three parameters: input (the string to parse), numbers (an array to store the parsed numbers), and count (a pointer to an integer to store the number of parsed numbers).
void processNumbers(char *input, int *numbers, int *count)
{
    // Get the first token (substring) in the input string, using a comma as the delimiter.
    char *token = strtok(input, ",");
    // Initialize the count of parsed numbers to 0.
    *count = 0;
    // Loop through all the tokens in the input string, up to a maximum of MAX_NOTES.
    while (token && *count < MAX_NOTES)
    {
        // Convert the token to an integer and store it in the numbers array.
        // Subtract 1 from the result to account for the note numbers being 1-indexed.
        numbers[*count] = atoi(token) - 1;
        // Increment the count of parsed numbers.
        (*count)++;
        // Get the next token in the input string.
        token = strtok(NULL, ",");
    }
}

// The hideNotes function is responsible for hiding notes in the db.
// It prompts the user to enter note numbers to hide, separated by commas.
// It then parses the input string into an array of integers, and for each integer in the array,
// it sets the corresponding note's hidden flag to 1.
void hideNotes()
{
    // Prompt the user to enter note numbers to hide.
    printf("Enter note numbers to hide (comma-separated): ");
    char input[100];                         // Initialize a buffer to store the user's input.
    if (!fgets(input, sizeof(input), stdin)) // Read the user's input from standard input.
        return;                              // Exit the function if no input was read.

    int numbers[MAX_NOTES], count;          // Initialize an array to store the note numbers and a variable to store the count.
    processNumbers(input, numbers, &count); // Parse the input string into an array of integers.

    // Loop through all the note numbers in the array.
    for (int i = 0; i < count; i++)
    {
        // Check if the note number is valid.
        if (numbers[i] >= 0 && numbers[i] < db.noteCount)
        {
            // Set the note's hidden flag to 1.
            db.notes[numbers[i]].hidden = 1;
        }
    }
    // Save the db.
    saveDb();
}

// The unhideNotes function is responsible for unhiding notes in the db.
// It prompts the user to enter note numbers to unhide, separated by commas.
// It then parses the input string into an array of integers, and for each integer in the array,
// it sets the corresponding note's hidden flag to 0.
void unhideNotes()
{
    // Prompt the user to enter note numbers to unhide.
    printf("Enter note numbers to unhide (comma-separated): ");
    char input[100];                         // Initialize a buffer to store the user's input.
    if (!fgets(input, sizeof(input), stdin)) // Read the user's input from standard input.
        return;                              // Exit the function if no input was read.

    int numbers[MAX_NOTES], count;          // Initialize an array to store the note numbers and a variable to store the count.
    processNumbers(input, numbers, &count); // Parse the input string into an array of integers.

    // Loop through all the note numbers in the array.
    for (int i = 0; i < count; i++)
    {
        // Check if the note number is valid.
        if (numbers[i] >= 0 && numbers[i] < db.noteCount)
        {
            // Set the note's hidden flag to 0.
            db.notes[numbers[i]].hidden = 0;
        }
    }
    // Save the db.
    saveDb();
}

// The deleteNotes function allows the user to delete notes from the db.
// It prompts the user to enter note numbers to delete, separated by commas.
// It then parses the input string into an array of integers, and for each integer in the array,
// it shifts the notes in the db down to fill in the gap left by the deleted note.
// It does this by looping through the notes in the db starting from the end of the array,
// and for each note that is after the current note to be deleted, it copies the note to the previous index in the array.
// It then decrements the note count and saves the db.
void deleteNotes()
{
    // Prompt the user to enter note numbers to delete.
    printf("Enter note numbers to delete (comma-separated): ");
    char input[100];                         // Initialize a buffer to store the user's input.
    if (!fgets(input, sizeof(input), stdin)) // Read the user's input from standard input.
        return;                              // Exit the function if no input was read.

    int numbers[MAX_NOTES], count;          // Initialize an array to store the note numbers and a variable to store the count.
    processNumbers(input, numbers, &count); // Parse the input string into an array of integers.

    // Loop through all the note numbers in the array, starting from the end.
    for (int i = count - 1; i >= 0; i--)
    {
        // Check if the note number is valid.
        if (numbers[i] >= 0 && numbers[i] < db.noteCount)
        {
            // Shift the notes in the db down to fill in the gap left by the deleted note.
            for (int j = numbers[i]; j < db.noteCount - 1; j++)
            {
                db.notes[j] = db.notes[j + 1];
            }
            // Decrement the note count.
            db.noteCount--;
        }
    }
    // Save the db.
    saveDb();
}

// Lifecycle:
// Load the db from file
// Enter a loop to display the notes,
// Prompt the user to enter a command
// Execute the corresponding function.
// The loop continues until the user enters the 'q' command to quit the program.
int main()
{
    loadDb(); // Load the db from file.
    char command;   // Initialize a variable to store the user's command.

    // Enter a loop where the program will display the notes, prompt the user to enter a command,
    // and then execute the corresponding function.
    while (1)
    {
        clearScreen();
        printf("\nNotes:\n");                                                 // Print a header for the list of notes.
        displayNotes(0);                                                      // Display the notes in the db.
        printf("\nCommands: [a]dd [h]ide [s]how-hidden [d]elete [q]uit\n> "); // Print the available commands.

        command = getchar(); // Read the user's command from standard input.
        while (getchar() != '\n')
            ; // Discard remaining characters in the input buffer.

        // Execute the corresponding function based on the user's command.
        switch (command)
        {
        case 'a':
            clearScreen();
            addNote(); // Add a new note to the db.
            clearScreen();
            break;
        case 'h':
            hideNotes(); // Hide notes in the db.
            clearScreen();
            break;
        case 's':
            clearScreen();
            printf("\nHidden notes:\n");      // Print a header for the list of hidden notes.
            displayNotes(1);                  // Display the hidden notes in the db.
            printf("\n[u]nhide or [b]ack: "); // Print the available options.
            command = getchar();              // Read the user's commad.
            while (getchar() != '\n')
                ; // Discard any remaining characters in the input buffer.
            if (command == 'u')
            {
                unhideNotes(); // Unhide notes in the db.
            }
            clearScreen();
            break;
        case 'd':
            deleteNotes(); // Delete notes from the db.
            clearScreen();
            break;
        case 'q':
            clearScreen();
            return 0;
        }
    }
    return 0;
}
