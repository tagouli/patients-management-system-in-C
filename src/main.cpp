#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define block_size 1024 //max characters in block (size of block in bytes)
#define SEEK_SET 0


typedef struct {
    char id[50];
    char name[50];
    char age[3];
    int deleted;
} Patient;  

typedef struct {
    char rec[256];
    int size;
} record;

typedef struct {
    char patients[block_size];
    int nr;              // number of records in block
    int used;            // used space in block
} Tbloc;

typedef struct {
    int nb; // number of blocks
    int nr; // number of records
    int ne; // number of deleted records
} Header;

int header(FILE* file, int i) {// read header
    Header a;
    int k;
    fseek(file, 0, SEEK_SET);
    fread(&a, sizeof(Header), 1, file);
    switch (i) {
    case 1:
        k = a.nb;
        break;
    case 2:
        k = a.nr;
        break;
    case 3:
        k = a.ne;
        break;
    default:
        printf("There is no header\n");
        return -1;
    }
    return k;
}

void setheader(FILE* file, int i, int n) {// set header in the file
    Header a;

    fseek(file, 0, SEEK_SET);
    fread(&a, sizeof(Header), 1, file);

    switch (i) {
    case 1:
        a.nb = n;
        break;
    case 2:
        a.nr = n;
        break;
    case 3:
        a.ne = n;
        break;
    default:
        printf("There is no header\n");
        return;
    }

    fseek(file, 0, SEEK_SET);
    fwrite(&a, sizeof(Header), 1, file);
    printf("Header updated successfully.\n");
}

Patient recordToPatient(record r) {// convert a record to a patient struct

    Patient p = {"", "", "", 0};  // Initialize with empty fields
    int pos = 0, field = 0;
    char temp[50];  
    int tempPos = 0;

    // loop through each char in the record
    while (r.rec[pos] != '\0') {
        //the || separator between fields
        if (r.rec[pos] == '|' && r.rec[pos + 1] == '|') {
            temp[tempPos] = '\0';  
            tempPos = 0;  // reset buffer for next field
            
            
            if (field == 0) {
                strcpy(p.id, temp);  // ID field
            } else if (field == 1) {
                strcpy(p.name, temp);  // Name field
            } else if (field == 2) {
                strcpy(p.age, temp);  // Age field 
            } else if (field == 3) {
                p.deleted = (strcmp(temp, "1") == 0);  //deleted status (0 or 1)
            }

            field++;  // move to the next field
            pos += 2;  // skip over the ||
        }
        //the end of the record
        else if (r.rec[pos] == '\n' || r.rec[pos] == '\0') {
            temp[tempPos] = '\0';  
            break;
        } else {
            temp[tempPos] = r.rec[pos];  // add current character to buffer
            tempPos++;  // move buffer position
        }
        pos++;  // move to the next character in the record
    }

    return p;  
    }

void readRecord(Tbloc* buff, record records[]) { // read records from a block
    int pos = 0, recordI = 0;

    while (pos < buff->used && recordI < buff->nr) {
        record r = {"", 0};
        int fieldPos = 0;

        while (buff->patients[pos] != '\0' && buff->patients[pos] != '\n') {
            r.rec[fieldPos++] = buff->patients[pos++];
        }

        if (fieldPos > 0) { // only non-empty records
            r.rec[fieldPos] = '\n';
            fieldPos++;
            r.rec[fieldPos] = '\0';
            r.size = fieldPos;

            records[recordI] = r;
            recordI++;
        }
        pos++; // move past the newline character
    }
}

void convertToPatientArray(record records[], int nr, Patient patients[]) {// convert an array of records to an array of patients
    for (int i = 0; i < nr; i++) {
        patients[i] = recordToPatient(records[i]);
    }
}

void searchAndDisplayRecord(record records[], int numRecords, const char* searchID) {
    for (int i = 0; i < numRecords; i++) {
        if (strncmp(records[i].rec, searchID, strlen(searchID)) == 0) {  
            printf("%s\n", records[i].rec);  // print the matching record
            return;                          
        }
    }
    printf("No records found with ID '%s'.\n", searchID);
}

void displayRecords(record records[], int numRecords) {
    for (int i = 0; i < numRecords; i++) {
        if (strlen(records[i].rec) > 1) {  // ensure the record is not empty
            printf("%s", records[i].rec); // print the entire record string
        }
    }
}

void display(FILE* file) {// display all records in the file
    Tbloc block;
    int blocks = header(file, 1);  // number of blocks

    for (int i = 1; i <= blocks; i++) {
        fseek(file, sizeof(Header) + (i - 1) * sizeof(Tbloc), SEEK_SET);  // move to the block
        fread(&block, sizeof(Tbloc), 1, file);

        printf("\nBlock %d:\n", i);

        record records[block.nr];  // create an array of records
        memset(records, 0, sizeof(records)); // clear the array 
        readRecord(&block, records);  // fill the records array

        displayRecords(records, block.nr);  // use function to display all records in the block
    }
}
void insert(FILE* file, Patient p) {// insert a patient record into the file
    Tbloc buff1;
    record record;
    
    int pos = 0;
    memset(record.rec, 0, sizeof(record.rec)); // clear the record

    // copy ID to the record
    strcpy(record.rec + pos, p.id);  
    pos += strlen(p.id);  // update position + length of ID
    record.rec[pos] = '|';  
    pos++;
    record.rec[pos] = '|';  
    pos++;

    // copy Name to the record
    strcpy(record.rec + pos, p.name);  
    pos += strlen(p.name);  
    record.rec[pos] = '|';  
    pos++;
    record.rec[pos] = '|';  
    pos++;

    // copy Age to the record
    strcpy(record.rec + pos, p.age);  
    pos += strlen(p.age);  
    record.rec[pos] = '|';  
    pos++;
    record.rec[pos] = '|';  
    pos++;

    // Set deletion status
    if (p.deleted == 1) {
        strcpy(record.rec + pos, "1");  
        pos++;
    } else {
        strcpy(record.rec + pos, "0");  
        pos++;
    }
    record.rec[pos++] = '\n';  // add a newline
    pos++;  
    record.rec[pos] = '\0';   // terminate the record string

    record.size = pos;  

    fseek(file, sizeof(Header), SEEK_SET);
    while (fread(&buff1, sizeof(Tbloc), 1, file) == 1) {
        if (block_size - buff1.used >= record.size) {
            // append the record to the block
            for (int i = 0; i < record.size; i++) {
                buff1.patients[buff1.used + i] = record.rec[i];
            }
            buff1.used += record.size;
            buff1.nr++;

            fseek(file, -sizeof(Tbloc), SEEK_CUR);
            fwrite(&buff1, sizeof(Tbloc), 1, file);
            
            setheader(file, 2, header(file, 2) + 1); // update number of records
            printf("Record inserted successfully.\n");
            return;
        }
    }

    // no space in existing blocks, create a new block
    Tbloc buff2;
    buff2.used = 0;
    for (int i = 0; i < record.size; i++) {
        buff2.patients[i] = record.rec[i];
    }
    buff2.used = record.size;
    buff2.nr = 1; // Only one record in the new block
    fwrite(&buff2, sizeof(Tbloc), 1, file);
    setheader(file, 1, header(file, 1) + 1); // update number of blocks
    setheader(file, 2, header(file, 2) + 1); // update number of records
    printf("Record inserted in a new block.\n");
}

void deletem(FILE* file, const char* id) {// Delete a patient record by ID
    Tbloc block;
    int blocks = header(file, 1);

    for (int i = 1; i <= blocks; i++) {
        fseek(file, sizeof(Header) + (i - 1) * sizeof(Tbloc), SEEK_SET);
        fread(&block, sizeof(Tbloc), 1, file);

        record records[block.nr];
        readRecord(&block, records);

        for (int j = 0; j < block.nr; j++) {
            Patient p = recordToPatient(records[j]);
            if (strcmp(p.id, id) == 0 && p.deleted == 0) {
                p.deleted = 1;//deleted

                // store the record string
                sprintf(records[j].rec, "%s||%s||%s||1\n", p.id, p.name, p.age);
                records[j].size = strlen(records[j].rec);

                // update block content
                int pos = 0;
                for (int k = 0; k < j; k++) {
                    pos += records[k].size;
                }
                // copy up to records[j].size characters from records[j].rec to block.patients starting at position pos
                strncpy(block.patients + pos, records[j].rec, records[j].size);

                // write back to file
                fseek(file, sizeof(Header) + (i - 1) * sizeof(Tbloc), SEEK_SET);
                fwrite(&block, sizeof(Tbloc), 1, file);

                // update header
                setheader(file, 3, header(file, 3) + 1);
                printf("Record with ID %s marked as deleted.\n", id);
                return;
            }
        }
    }
    printf("Record with ID %s not found.\n", id);
}

void search(FILE* file, const char* searchID) {//search for a specific ID
    Tbloc block;
    int blocks = header(file, 1);  // number of blocks

    for (int i = 1; i <= blocks; i++) {
        fseek(file, sizeof(Header) + (i - 1) * sizeof(Tbloc), SEEK_SET);  // Move to block
        fread(&block, sizeof(Tbloc), 1, file);

        record records[block.nr];  // create an array 
        readRecord(&block, records);  

        
        searchAndDisplayRecord(records, block.nr, searchID);
    }
}


void searchInRange(FILE* file, const char* startID, const char* endID) { //search for a specific ID in a range
    Tbloc block;
    int blocks = header(file, 1);
    bool found = false;
    for (int i = 1; i <= blocks; i++) {
        fseek(file, sizeof(Header) + (i - 1) * sizeof(Tbloc), SEEK_SET);
        fread(&block, sizeof(Tbloc), 1, file);
        record records[block.nr];
        memset(records, 0, sizeof(records));
        readRecord(&block, records);

        for (int j = 0; j < block.nr; j++) {
            // compare ID within the range
            if (strncmp(records[j].rec, startID, strlen(startID)) >= 0 &&
                strncmp(records[j].rec, endID, strlen(endID)) <= 0) {
                printf("%s", records[j].rec);
                found = true;
            }
        }
    }
    if (!found) {
        printf("No records found in the range from '%s' to '%s'.\n", startID, endID);
    }
}



int main() {
    FILE* file = fopen("patientsA.dat", "rb+");
    if (file == NULL) {
        file = fopen("patientsA.dat", "wb+");
        if (file == NULL) {
            printf("Error opening file.\n");
            return -1;
        }
        Header header_init = {0, 0, 0};
        fwrite(&header_init, sizeof(Header), 1, file);
    }

    int choice;
    do {
        printf("\n1. Display Records\n");
        printf("2. Search Record\n");
        printf("3. Insert Record\n");
        printf("4. Search by ID Range\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("\nDisplaying all records:\n");
            display(file);
        } else if (choice == 2) {
            char searchID[50];
            printf("\nEnter ID to search: ");
            scanf("%s", searchID);
            search(file, searchID);
        } else if (choice == 3) {
            Patient p;
            printf("\nEnter ID: ");
            scanf("%s", p.id);
            printf("Enter Name: ");
            scanf("%s", p.name);
            printf("Enter Age: ");
            scanf("%s", p.age);
            p.deleted = 0;
            insert(file, p);
        } else if (choice == 4) {
            char startID[50], endID[50];
            printf("\nEnter start ID: ");
            scanf("%s", startID);
            printf("Enter end ID: ");
            scanf("%s", endID);
            searchInRange(file, startID, endID);
        } else if (choice != 5) {
            printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);

    fclose(file);
    return 0;
}


