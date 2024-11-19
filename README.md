# Files in C (T¬OV¬C)
The program handles files of type T¬OV¬C.
- Records are manually structured with `||` between fields and `\n` at the end of each record.

---

# Patient Management System

This is a patient management system written in C that stores patient records in a file. The system allows users to display, search, and insert records. Each record contains an ID, name, age, and a deleted flag.

## Features:
- **Display all records**
- **Search records by ID**
- **Insert new records**
- **Search records by ID range**

## Functions

### Main Functions

```c
int main() { ... } // Line 44: Main 

void display(FILE* file) { .} // Line 104: Display records

    Line 104: Display records 
    Line 107: Reads the block and records data from the file.
    Line 114: Displays each record in a block.

void search(FILE* file, const char* searchID) {} // Line 128: Search record by ID

    Line 130: Retrieves the total block count from the header.
    Line 132: Compares the ID with the search string and displays it.

void searchInRange(FILE* file, const char* startID, const char* endID) { } // Line 140: Search in ID range IDs between startID and endID

    Line 149: Compares IDs to check if they are in the range.

void insert(FILE* file, Patient p) { ... } // Line 173: Insert record manual separate

    Line 174: Creates the record from the Patient data.//separate fields with '||' and end of record '\n'
    Line 186: Creates a new block when there’s no space.
    Line 192: Update header - Updates the header with the new number of blocks and records.

void setheader(FILE* file, int i, int n) { ... } // Line 95: Set header value

Patient recordToPatient(record r) { ... } // Line 76: Convert record to patient

    Line 81: Split fields - Splits the record string into individual fields (ID, Name, Age, Deleted).

void readRecord(Tbloc* buff, record records[]) { ... } // Line 112: Read records

    Line 112: Reads all records from a block and stores them in an array.
    Line 115: Loops through the block’s data, extracting individual records.

void convertToPatientArray(record records[], int nr, Patient patients[]) { ... } // Line 123: Convert to array
```
### Data Structures
record:
```c
Represents a single record with its raw data (as a string) and its size.

typedef struct {
    char rec[256];
    int size;
} record;
```
### Notes:

- **ID: A string of up to 50 characters.**
- **Name: A string of up to 50 characters.**
- **Age: A string of up to 3 characters.**
- **Deleted Flag: Integer (1 for true, 0 for false) indicating whether the record is marked as deleted.**

## Creators

**This project was created by [tagouli].**

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Copyright (c) 2024 [tagouli].

![License](https://img.shields.io/badge/license-MIT-blue.svg)

---
