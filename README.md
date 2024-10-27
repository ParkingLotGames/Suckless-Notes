# Notes Database Program

## Description

This program is a simple command-line application for managing a personal database of notes. It provides functionalities for adding, viewing, hiding, unhiding, and deleting notes. The database is stored in a file named `.notes_db` in the user's home directory.

## Functionality

The program offers the following features:

- **Add notes:** Users can add new notes to the database.
- **View notes:** Users can view existing notes, including the option to view hidden notes separately.
- **Hide notes:** Users can mark specific notes as hidden.
- **Unhide notes:** Users can unhide previously hidden notes.
- **Delete notes:** Users can delete notes from the database.

## Installation
```bash
./build.sh && ./install.sh
```

## Usage

Upon execution, the program presents a menu with the following options:

- **a:** Add a new note.
- **h:** Hide existing notes.
- **s:** Show hidden notes.
- **d:** Delete notes.
- **q:** Quit the program.

Users can interact with the program by entering the corresponding command character followed by pressing the Enter key.

**Example:**

To add a new note, the user would enter the character 'a' and press Enter. The program will then prompt the user to enter the content of the note.

To hide notes numbered 1 and 3, the user would enter the character 'h' and press Enter. The program will then prompt for the note numbers to hide, and the user would enter "1,3".

To delete notes numbered 2 and 4, the user would enter the character 'd' and press Enter. The program will then prompt for the note numbers to delete, and the user would enter "2,4".

## Limitations

- The program has a maximum limit of 1000 notes.
- Each note can have a maximum length of 4096 characters.
- The program does not implement any form of encryption or security for the stored data.

## Notes

- The program uses the `clear` command to clear the screen, which may not be compatible with all systems.
- The program assumes a Unix-like environment with a home directory defined by the `HOME` environment variable.

## Disclaimer

This program is intended for personal use and has not been extensively tested for security or robustness. Use at your own discretion.

## License

This program is licensed under the MIT license.