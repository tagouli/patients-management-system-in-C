#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define block_size 1024 //max characters in block (size of block in bytes)
#define SEEK_SET 0
#define indexRow 1000
#define indexCol 3


typedef struct {
    int numOfrec; // number of records
} HeaderIndex;//header index struct

typedef struct {
    int id;
    int blockNum;
    int recNum;
} IndexF; //index struct

typedef struct {
    char id[50];
    char name[50];
    char age[3];
    int deleted;
} Patient;  //patient struct

typedef struct {
    char rec[256];
    int size;
} record; //record struct

typedef struct {
    char patients[block_size];
    int nr;              // number of records in block
    int used;            // used space in block
} Tbloc;//block struct

typedef struct {
    int nb; // number of blocks
    int nr; // number of records
    int ne; // number of deleted records
} Header;//header struct


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
//Index
void loadIndex(FILE* indexF, int indexT[][indexCol], int *num) {
    HeaderIndex headerIndex;
    // Read the number of records
    fseek(indexF, 0, SEEK_SET);  // Go to the beginning of the file
    fread(&headerIndex, sizeof(HeaderIndex), 1, indexF);
    
    *num = headerIndex.numOfrec;  // Set the number of records from the header

    // Read the indexTable values from the file
    for (int i = 0; i < *num; i++) {
        IndexF indexValues;
        fread(&indexValues, sizeof(IndexF), 1, indexF);  // Read one values of IndexF from the file
        indexT[i][0] = indexValues.id;       // Store the id
        indexT[i][1] = indexValues.blockNum; // Store the block number
        indexT[i][2] = indexValues.recNum;   // Store the record number
    }
}

void saveIndex(FILE* indexF, int indexT[][indexCol], int num) {
    HeaderIndex headerIndex;
    headerIndex.numOfrec = num;  // Set the number of records in the header

    // Write the header to the file
    fseek(indexF, 0, SEEK_SET);  // Go to the beginning of the file
    fwrite(&headerIndex, sizeof(HeaderIndex), 1, indexF);

    // Write the indexTable values to the file
    for (int i = 0; i < num; i++) {
        IndexF indexFEntry;
        indexFEntry.id = indexT[i][0];        // Set id
        indexFEntry.blockNum = indexT[i][1];  // Set block number
        indexFEntry.recNum = indexT[i][2];    // Set record number

        fwrite(&indexFEntry, sizeof(IndexF), 1, indexF);  // Write one value of IndexF to the file
    }
}
void deleteIndex(int id,int indexT[indexRow][indexCol], int *num) {
    for (int i = 0; i < *num; i++) {
        if (indexT[i][0] == id) {
            for (int j = i; j < indexCol; j++) {
            indexT[i][j]=indexT[*num-1][j];
            }
            (*num)--;
            return;
        }
    }
}
void searchIndex(int id,int pos[2],int indexT[indexRow][indexCol], int num) {//blockPos,recnum
    for (int i = 0; i < num; i++) {
        if (indexT[i][0] == id) {
            pos[0]=indexT[i][1];
            pos[1]=indexT[i][2];
            printf("Record Found\n");
            return;
        }
    }
    printf("Record Not Found\n");
}
void displayFromIndex(FILE* file,int pos[2]) {
    Tbloc block;
    int blocks = header(file, 1);
    fseek(file, sizeof(Header) + (pos[0] - 1) * sizeof(Tbloc), SEEK_SET);
    fread(&block, sizeof(Tbloc), 1, file);
    record records[block.nr];
    readRecord(&block, records);
    printf("%s", records[pos[1]].rec);
    
}
void InsertInIndex(int indexT[indexRow][indexCol],const char* id,int blockPos,int recPos,int *num) {
    int idn=atoi(id);
    int i=0;
    while (indexT[i][0]!=0) {
        i++;
    }
    indexT[i][0]=idn;
    indexT[i][1]=blockPos;
    indexT[i][2]=recPos;
    (*num)++;
    
}
void displayIndexTable(int indexT[indexRow][indexCol], int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < indexCol; j++) {
            printf("%d ", indexT[i][j]);
        }
        printf("\n");
    }
}
void insert(FILE* file, Patient p, int indexT[indexRow][indexCol],int *num) {// insert a patient record into the file
    Tbloc buff1;
    record record;
    int currentBlock=0;//to save in index
    int recordnum = 0;//to save in index
    int pos = 0;
    int insert=0;
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
//save the record string to the file
    fseek(file, sizeof(Header), SEEK_SET);
    while (fread(&buff1, sizeof(Tbloc), 1, file) == 1) {
        currentBlock++;
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
             int recordnum = header(file, 2);     // record start position to save in index
            printf("Record inserted successfully.\n");
            InsertInIndex(indexT, p.id, currentBlock, recordnum, num);
            printf("Id inserted successfully In index File.\n");

            insert=1;
            return;
        }
    if(insert==1)
    break;
       
    }

    // no space in existing block, create a new block
    Tbloc buff2;
    currentBlock++;
    buff2.used = 0;
    recordnum = 1;          // record start number to save in index

    for (int i = 0; i < record.size; i++) {
        buff2.patients[i] = record.rec[i];
    }
    buff2.used = record.size;
    buff2.nr = 1; // Only one record in the new block
    fwrite(&buff2, sizeof(Tbloc), 1, file);
    setheader(file, 1, header(file, 1) + 1); // update number of blocks
    setheader(file, 2, header(file, 2) + 1); // update number of records
    printf("Record inserted in a new block.\n");
    InsertInIndex(indexT, p.id, currentBlock, recordnum, num);
    printf("Id inserted successfully In index File.\n");

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
    // open files
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

    FILE* indexF = fopen("indexF.txt", "r+");
    if (indexF == NULL) {
        indexF = fopen("indexF.txt", "w+");
        if (indexF == NULL) {
            printf("Error opening index file.\n");
            return -1;
        }
        HeaderIndex header2_init = {0};
        fwrite(&header2_init, sizeof(HeaderIndex), 1, indexF);
    }
    
    // Read the header of the index file to get the number of records
    HeaderIndex header2;
    fseek(indexF, 0, SEEK_SET);
    fread(&header2, sizeof(HeaderIndex), 1, indexF);
    
    // Initialize index table and number of records
    int numRecordsInIndex = header2.numOfrec;
    int indexTable[numRecordsInIndex][indexCol] = {0};


    // Load the index table from the file
    loadIndex(indexF, indexTable, &numRecordsInIndex);

    int choice;
    do {
        printf("\nMenu:\n");
        printf("1. Display All Records\n");
        printf("2. Search Record by ID\n");
        printf("3. Insert Record\n");
        printf("4. Delete Record by ID\n");
        printf("5. Search Record using Index\n");
        printf("6. Display Index Table\n");
        printf("7. Exit\n");
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
            printf("\nEnter Patient ID: ");
            scanf("%s", p.id);
            printf("Enter Patient Name: ");
            scanf("%s", p.name);
            printf("Enter Patient Age: ");
            scanf("%s", p.age);
            p.deleted = 0;
            insert(file, p, indexTable, &numRecordsInIndex);
        } else if (choice == 4) {
            char deleteID[50];
            printf("\nEnter ID to delete: ");
            scanf("%s", deleteID);
            deletem(file, deleteID);

            // Update index table
            int idToDelete = atoi(deleteID);
            deleteIndex(idToDelete, indexTable, &numRecordsInIndex);
        } else if (choice == 5) {
            int searchID;
            printf("\nEnter ID to search using index: ");
            scanf("%d", &searchID);

            int pos[2] = {0};
            searchIndex(searchID, pos, indexTable, numRecordsInIndex);

            if (pos[0] > 0 && pos[1] >= 0) {
                printf("\nDisplaying record from index:\n");
                displayFromIndex(file, pos);
            }
        } else if (choice == 6) {
            printf("\nDisplaying index table:\n");
            displayIndexTable(indexTable, numRecordsInIndex);
        } else if (choice == 7) {
            printf("\nExiting program.\n");
            break;
        } else {
            printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 7);

    // Save the index table to the file
    fseek(indexF, 0, SEEK_SET);
    saveIndex(indexF, indexTable, numRecordsInIndex);

    fclose(file);
    fclose(indexF);
    return 0;
}
