# Files in C (T¬OV¬C)
The program handles files of type T¬OV¬C.
- Records are manually structured with `||` between fields and `\n` at the end of each record.

---

# Patient Management System

This is a patient management system written in C that stores patient records in a file. The system allows users to display, search, and insert records. Each record contains an ID, name, age, and a deleted flag.
There is also an index file that saves the ID along with its corresponding block and offset.

## Features:
- **Display all records**
- **Search records by ID**
- **Insert new records**
- **Delete records by ID**
- **Search records using an Index File**
- **Maintain and display an Index Table**
- **Search records by ID range**

## Functions

## Main Functions

```c
int main() { ... } // Line 44: Main 

void display(FILE* file) { .} // Line 184: Display records

    Line 184: Display records 
    Line 197: Reads the block and records data from the file.
    Line 199: Displays each record in a block.

void search(FILE* file, const char* searchID) {} // Line 433: Search record by ID

    Line 435: Retrieves the total block count from the header.
    Line 443: Compares the ID with the search string and displays it.

void searchInRange(FILE* file, const char* startID, const char* endID) { } // Line 448: Search in ID range IDs between startID and endID

    Line 461: Compares IDs to check if they are in the range.

void insert(FILE* file, Patient p) { ... } // Line 291: Insert record manual separate

    Line 300: Creates the record from the Patient data.//separate fields with '||' and end of record '\n'
    Line 370: Creates a new block when there’s no space.
    Line 381: Update header - Updates the header with the new number of blocks and records.

void setheader(FILE* file, int i, int n) { ... } // Line 69: Set header value

Patient recordToPatient(record r) { ... } // Line 95: Convert record to patient

    Line 104: Split fields - Splits the record string into individual fields (ID, Name, Age, Deleted).

void readRecord(Tbloc* buff, record records[]) { ... } // Line 137: Read records

    Line 140: Reads all records from a block and stores them in an array.
    Line 145: Loops through the block’s data, extracting individual records.

void convertToPatientArray(record records[], int nr, Patient patients[]) { ... } // Line 161: Convert to array
```
### Index Functions
```c
void loadIndex(FILE* indexF, int indexT[][3], int* num) { ... } //Line 208 : Load index table from index file

void saveIndex(FILE* indexF, int indexT[][3], int num) { ... } //Line 221: Save index table to index file

void deleteIndex(int id, int indexT[][3], int* num) { ... } //Line 239: Remove an entry from the index table(physical deletion)

void searchIndex(int id, int pos[2], int indexT[][3], int num) { ... } //Line 250: Search for a record using the index table

void displayFromIndex(FILE* file, int pos[2]) { ... } //Line 261: Display a record from the file using its index position

void InsertInIndex(int indexT[][3], const char* id, int blockPos, int recPos, int* num) { ... } //Line 271: Add a new value to the index table

void displayIndexTable(int indexT[][3], int num) { ... } //Line 283: Display the current index table

```
### Data Structures
**record:**
- **Represents a single record with its raw data (as a string) and its size.**

```c
typedef struct {
    char rec[256];
    int size;
} record;
```
**Patient:**
Represents a patient's information.
```c
typedef struct {
    char id[50];    // Patient's ID
    char name[50];  // Patient's Name
    char age[3];    // Patient's Age
    int deleted;    // Deleted status (1 for true, 0 for false)
} Patient;
```
**IndexF:**
- **Represents an index entry for fast lookup.**
```c
typedef struct {
    int id;       // Patient ID
    int blockNum; // Block number in the file
    int recNum;   // Record number in the block
} IndexF;
```
**HeaderIndex:**
- **Represents the index file's header information.**
```c
typedef struct {
    int numOfrec; // Number of records in the index file
} HeaderIndex;
```
**Tbloc:**
- **Represents a block in the file.**
```c
typedef struct {
    char patients[block_size]; // Raw data of patient records
    int nr;                    // Number of records in the block
    int used;                  // Used space in the block
} Tbloc;
```
**Header:**
- **Represents the main file's header information.**
```c
typedef struct {
    int nb; // Number of blocks
    int nr; // Number of records
    int ne; // Number of deleted records
} Header;
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
