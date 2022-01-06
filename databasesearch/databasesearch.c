
#include "a6.h"

int main(int argc, char** argv) {
    //Main method
    
    int buildingGetIdx;
    int roomGetIdx;
    char*filenameBuildingSet;
    char*filenameRoomSet;

    char*filenameRoomBuildingIntersectionSet;

    //must have 3 command line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s basename value\n", argv[0]);
        return (-1);
    }

    //getting index for building and room
    buildingGetIdx = getIdx(argv[1], "building");
    roomGetIdx = getIdx(argv[2], "room");

    //getting set files 
    filenameBuildingSet = query("code", -1, "building", buildingGetIdx);
    filenameRoomSet = query("code", -1, "room", roomGetIdx);

    //intersection set file
    filenameRoomBuildingIntersectionSet = intersection(filenameBuildingSet, filenameRoomSet, argv[1], argv[2]);

    //getting index and printing everything
    setToIdx(filenameRoomBuildingIntersectionSet);

    return 0;
}

int getIdx(char*name, char*type) {
    /*
     * Getting the index associated with the type and name (room or building + name of former)
     * name: the name of the building or room
     * type: whether it is a building or room
     * return: index associated with name and type
     */

    long hash_table[ HASHSIZE ];
    char idxname[BUFFER];
    char txtname[BUFFER];
    char *basename;
    char *value;
    int index;

    //creating filenames
    strcpy(idxname, type);
    strcat(idxname, ".idx");
    strcpy(txtname, type);
    strcat(txtname, ".txt");

    basename = type;

    value = name;

    get_hashtable(basename, hash_table);

    //opening files and getting index from hash_lookup
    FILE *idxfile = fopen(idxname, "r");
    FILE *txtfile = fopen(txtname, "r");
    index = hash_lookup(value, hash_table, idxfile, txtfile);
    fclose(idxfile);
    fclose(txtfile);

    return index;
}

char* query(char*code, int i1, char*type, int i2) {
    /*
     * Searching for all codes associated with type i1 in code_"type".rel and
     * storing them in "type"_"i1".set (0 if code is not associated)
     * code: "code"
     * i1: first integer index
     * type: whether it is a building or room
     * i2: second integer index
     * return: set filename string
     */
    char *filenameSet = malloc(sizeof (char) * BUFFER);
    //creating filenames based on input
    if (i2 != -1) {
        strcpy(filenameSet, type);
        strcat(filenameSet, "_");
        sprintf(filenameSet, "%s%d", filenameSet, i2);
        strcat(filenameSet, ".set");
    } else {
        strcpy(filenameSet, type);
        sprintf(filenameSet, "%s%d", filenameSet, i1);
        strcat(filenameSet, ".set");
    }

    //getting entry from "code" and building or room
    int n1 = entries(code);
    int n2 = entries(type);

    char filename[BUFFER];

    //creating .rel filename
    strcpy(filename, code);
    strcat(filename, "_");
    strcat(filename, type);
    strcat(filename, ".rel");

    //opening .rel file and reading
    FILE *fp = fopen(filename, "rb");
    char *array = malloc(n1 * n2);
    fread(array, 1, n1*n2, fp);
    fclose(fp);

    
    //printing codes to set file
    fp = fopen(filenameSet, "wb");
    if ((i1 == -1) && (i2 >= 0)) {
        for (int i = 0; i < n1; i++) {
            int index = i * n2 + i2;
            fwrite(array + index, 1, 1, fp);
        }
    }

    if ((i1 > 0) && (i2 == -1)) {
        for (int j = 0; j < n2; j++) {
            int index = i1 * n2 + j;
            fwrite(array + index, 1, 1, fp);
        }
    }

    fclose(fp);
    return filenameSet;
}

int entries(char *filebase) {
    /*
     * Checking the idx file
     * filebase: the name of the idx file
     * return: ent
     */
    char filename[BUFFER];
    int ent;

    strcpy(filename, filebase);
    strcat(filename, ".idx");
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    ent = ftell(fp) / sizeof (long);
    fclose(fp);

    return ent;
}

char * intersection(char*fileBuidlingSet, char*fileRoomSet, char*name, char*number) {
    /*
     * Creating intersection set of building set file and room set file
     * fileBuidlingSet: building set file
     * fileRoomSet: room set file
     * name: the name of the building 
     * number: the number of the room
     * return: intersection file name
     */
    char *andSetFile = malloc(sizeof (char) * BUFFER);
    //creating file name
    strcpy(andSetFile, name);
    strcat(andSetFile, number);
    strcat(andSetFile, ".set");
    
    //opening all relevant files
    FILE *fp1 = fopen(fileBuidlingSet, "rb");
    FILE *fp2 = fopen(fileRoomSet, "rb");
    FILE *fp3 = fopen(andSetFile, "wb");
    char b1, b2, b3;

    //writing the intersection of room set and building set to intersection set
    while (fread(&b1, 1, 1, fp1) == 1 && fread(&b2, 1, 1, fp2)) {
        b3 = b1&&b2;
        fwrite(&b3, 1, 1, fp3);
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    return andSetFile;
}

void setToIdx(char* filenameSet) {
    /*
     * Gets all the codes that are in filenameSet and sends them off to 
     * finalSetDisplay to get strings
     * filenameSet: building and room intersection set file
     */
    char boolean;
    FILE *fp = fopen(filenameSet, "rb");

    for (int i = 0; fread(&boolean, 1, 1, fp) == 1; i++) {
        //if code found
        if (boolean) {
            //sending index to go through last 3 functions and the string 
            //will be displayed
            finalSetDisplay(i);
        }
    }
}

int setToIdxCopy(char* filenameSet) {
    /*
     * Gets all the codes that are in filenameSet and returns them to 
     * finalSetDisplay (copy of previous function so they do not overlap)
     * filenameSet: building and room intersection set file
     */
    char boolean;
    FILE *fp = fopen(filenameSet, "rb");

    for (int i = 0; fread(&boolean, 1, 1, fp) == 1; i++) {
        if (boolean) {
            //returning the code index 
            return i;
        }
    }
    return -1;
}

void finalSetDisplay(int index) {
    /*
     * Gathers necessary information for subject, courseno, days, from, to using
     * query, setToIdxCopy, and getString. Then displays in formatted string
     * index: index from code in setToIdx
     */
    
    //creating arrays for each respective type of information and index
    char **filenamesSet = malloc(sizeof (char*) * 5);
    int indexArray[5];
    char * finalString = malloc(sizeof (char) * BUFFER);

    //allocating enough space
    for (int i = 0; i < 5; i++) {
        filenamesSet[i] = malloc(sizeof (char) * BUFFER);
    }

    //going through query for each data type and index
    filenamesSet[0] = query("code", index, "subject", -1);
    filenamesSet[1] = query("code", index, "courseno", -1);
    filenamesSet[2] = query("code", index, "days", -1);
    filenamesSet[3] = query("code", index, "from", -1);
    filenamesSet[4] = query("code", index, "to", -1);

    //getting index from setfiles
    for (int i = 0; i < 5; i++) {
        indexArray[i] = setToIdxCopy(filenamesSet[i]);
    }

    //displaying formatted final string
    strcpy(finalString, getString("subject", indexArray[0]));
    finalString[strlen(finalString) - 1] = 0;
    strcat(finalString, "*");
    strcat(finalString, getString("courseno", indexArray[1]));
    finalString[strlen(finalString) - 1] = 0;
    strcat(finalString, " ");
    strcat(finalString, getString("days", indexArray[2]));
    finalString[strlen(finalString) - 1] = 0;
    strcat(finalString, " ");
    strcat(finalString, getString("from", indexArray[3]));
    finalString[strlen(finalString) - 1] = 0;
    strcat(finalString, " - ");
    strcat(finalString, getString("to", indexArray[4]));

    printf("%s", finalString);
}

char * getString(char * basename, int index) {
    /*
     * Gets the string from the corresponding basename txt and idx file, at the
     * specified index
     * basename: type (subject, courseno, days, from, to)
     * index: index to find in txt and idx file
     * return: corresponding string for txt file
     */
    char * info = malloc(sizeof (char) * BUFFER);
    int idx2;

    char txtfile[BUFFER];
    char idxfile[BUFFER];
    char buffer[BUFFER];

    FILE *fptxt, *fpidx;

    //creating file names
    strcpy(txtfile, basename);
    strcat(txtfile, ".txt");

    strcpy(idxfile, basename);
    strcat(idxfile, ".idx");

    fptxt = fopen(txtfile, "r");
    fpidx = fopen(idxfile, "r");

    fseek(fpidx, sizeof (long)*index, SEEK_SET);

    //index not found for unexpected reason
    if (fread(&idx2, sizeof (long), 1, fpidx) != 1) {
        fprintf(stderr, "Error: invalid index\n");
        exit(-1);
    }

    //finding and getting the string contents to buffer
    fseek(fptxt, idx2, SEEK_SET);

    fgets(buffer, BUFFER, fptxt);

    strcpy(info, buffer);
    fclose(fptxt);
    return info;
}
