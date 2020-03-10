#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"

#define MAX_COLS 512

void read_in_file(FILE *infile, struct universe *u){
    
    if (infile == NULL ) {
        fprintf(stderr, "ERROR: Input file not defined\n");
        exit(1);
    }

    int rows = 0;
    int cols = 0;
    int bufferLength = MAX_COLS + 2;
    char buffer[bufferLength];

    int fileBufferLength = 512;
    char *filebuffer = malloc(fileBufferLength);
    
    int fileStringLength;

    int current_line_cols;

    int trimLength;
    // get number of rows
    while(fgets(buffer, bufferLength, infile)) {

        // set column length if unset
        if (cols == 0) {
            if (buffer[strlen(buffer)-2] == 13){ // CRLF
                trimLength = 2;
            } else {  // LF
                trimLength = 1;
            }
            cols = strlen(buffer) - trimLength;
        }

        // check for inconsistent column length
        current_line_cols = strlen(buffer)-trimLength;
        if (current_line_cols != cols){            
            fprintf(stderr, "ERROR: Inconsistent column length\n");
            exit(1);
        }
        
        // check for column count greater than 512
        if ((buffer[cols] != '\n') && (buffer[cols] != '\r')) {
            fprintf(stderr, "ERROR: Too many columns\n");
            exit(1);
        }
        
        rows++;
        
        fileStringLength = strlen(filebuffer);

        if (fileStringLength >= fileBufferLength/2) {
            fileBufferLength *= 2;
            filebuffer = (char*)realloc(filebuffer, fileBufferLength*sizeof(char));
        }

        strcat(filebuffer, buffer);
    }
    
    //save data into struct
    u->num_rows = rows;
    u->num_columns = cols;

    //dynamically allocate memory
    u->grid = (int*)calloc((u->num_rows)*(u->num_columns), sizeof(int));

    double num_cells_living_total_current = 0;

    // iterate through chars
    char ch;
    int index = 0;
    fileStringLength = strlen(filebuffer);
    for (int i=0; i < fileStringLength; i++) {
        ch = filebuffer[i];
        if (ch == '*') {
            u->grid[index] = 1;
            num_cells_living_total_current += 1;
            index++;
        } else if (ch == '.') {
            index++;
        }
    }

    u->current_cells = (u->num_columns)*(u->num_rows);
    u->current_cells_alive = num_cells_living_total_current;
    u->total_cells = (u->num_columns)*(u->num_rows);
    u->total_cells_alive = num_cells_living_total_current;

    return;
}

void write_out_file(FILE *outfile, struct universe *u){
    if (outfile == NULL ) {
        fprintf(stderr, "ERROR: Output file not defined\n");
        exit(1);
    }

    char *fileBuffer = malloc(sizeof(char) * ((u->num_rows)*(u->num_columns+1)));//;
    fileBuffer[0] = '\0';

    for (int i = 0; i < ( (u->num_rows) * (u->num_columns) ); i++){
        
        if ( (u->grid[i]) == 0 ){ // dead cell
            //fprintf(outfile, ".");
            strcat(fileBuffer, ".");
        } else if ( (u->grid[i]) == 1 ) { // alive cell
            //fprintf(outfile, "*");
            strcat(fileBuffer, "*");
        }
        if ( !((i+1) % (u->num_columns)) ){ // row end
            //fprintf(outfile, "\n");
            strcat(fileBuffer, "\n");
        }
    }

    fprintf(outfile, "%s", fileBuffer);

    free(fileBuffer);

}

int is_alive(struct universe *u, int column, int row){
    return u->grid[column + row*(u->num_columns)];
}

int will_be_alive(struct universe *u, int column, int row){

    if ((column < 0) || (column >= u->num_columns)) {
        fprintf(stderr, "ERROR: Column %d out of range", column);
    }

    if ((row < 0) || (row >= u->num_rows)) {
        fprintf(stderr, "ERROR: Row %d out of range", row);
    }

    int cell = u->grid[column + row*(u->num_columns)];

    int columnLeft, columnRight, rowUp, rowDown;
    
    //no torus
    columnRight = column + 1;
    columnLeft = column - 1;
    rowUp = row - 1;
    rowDown = row + 1;

    //torus or no torus
    int cell_n  = u->grid[(column)      + ((rowUp)*(u->num_columns))];
    int cell_ne = u->grid[(columnRight) + ((rowUp)*(u->num_columns))];
    int cell_e  = u->grid[(columnRight) + ((row)*(u->num_columns))];
    int cell_se = u->grid[(columnRight) + ((rowDown)*(u->num_columns))];
    int cell_s =  u->grid[(column)      + ((rowDown)*(u->num_columns))];
    int cell_sw = u->grid[(columnLeft)  + ((rowDown)*(u->num_columns))];
    int cell_w  = u->grid[(columnLeft)  + ((row)*(u->num_columns))];
    int cell_nw = u->grid[(columnLeft)  + ((rowUp)*(u->num_columns))];

    //no torus
    if (rowUp < 0){ // top row
        cell_nw = cell_n = cell_ne = 0;
    }
    if (rowDown > u->num_rows-1){ // bottom row
        cell_sw = cell_s = cell_se = 0; 
    }
    if (columnLeft < 0) { // left row
        cell_nw = cell_w = cell_sw = 0;
    }
    if (columnRight > u->num_columns-1){ // right row
        cell_ne = cell_e = cell_se = 0;
    }


    int cells[8] = {cell_n, cell_ne, cell_e, cell_se, cell_s, cell_sw, cell_w, cell_nw};

    int num_living_cells = 0;

    for(int i = 0; i < 8; i++){
        num_living_cells += cells[i];
    }
    
    int cell_next = 0;

    if (((cell == 1) && (num_living_cells == 2)) || (num_living_cells == 3)) {
        cell_next = 1;
    } else if ((cell == 0) && (num_living_cells == 3)) {
        cell_next = 1;
    }

    return cell_next;
}

int will_be_alive_torus(struct universe *u,  int column, int row){

    if ((column < 0) || (column >= u->num_columns)) {
        fprintf(stderr, "ERROR: Column %d out of range", column);
    }

    if ((row < 0) || (row >= u->num_rows)) {
        fprintf(stderr, "ERROR: Row %d out of range", row);
    }

    int cell = u->grid[column + row*(u->num_columns)];

    int columnLeft, columnRight, rowUp, rowDown;

    //torus
    if (row <= 0){
        rowUp = u->num_rows-1;
    } else {
        rowUp = row - 1;
    }
    if (row >= u->num_rows-1){
        rowDown = 0;
    } else {
        rowDown = row + 1;
    }
    if (column <= 0) {
        columnLeft = u->num_columns-1;
    } else {
        columnLeft = column - 1;
    }
    if (column >= u->num_columns-1){
        columnRight = 0;
    } else {
        columnRight = column + 1;
    }

    int cell_n  = u->grid[(column)      + ((rowUp)*(u->num_columns))];
    int cell_ne = u->grid[(columnRight) + ((rowUp)*(u->num_columns))];
    int cell_e  = u->grid[(columnRight) + ((row)*(u->num_columns))];
    int cell_se = u->grid[(columnRight) + ((rowDown)*(u->num_columns))];
    int cell_s =  u->grid[(column)      + ((rowDown)*(u->num_columns))];
    int cell_sw = u->grid[(columnLeft)  + ((rowDown)*(u->num_columns))];
    int cell_w  = u->grid[(columnLeft)  + ((row)*(u->num_columns))];
    int cell_nw = u->grid[(columnLeft)  + ((rowUp)*(u->num_columns))];

    int cells[8] = {cell_n, cell_ne, cell_e, cell_se, cell_s, cell_sw, cell_w, cell_nw};

    int num_living_cells = 0;

    for(int i = 0; i < 8; i++){
        num_living_cells += cells[i];
    }

    int cell_next = 0;

    if (((cell == 1) && (num_living_cells == 2)) || (num_living_cells == 3)) {
        cell_next = 1;
    } else if ((cell == 0) && (num_living_cells == 3)) {
        cell_next = 1;
    }

    return cell_next;

}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){

    if (rule == NULL ) {
        fprintf(stderr, "ERROR: Null rule pointer\n");
        exit(1);
    }
    
    struct universe u2;
    (u2).num_rows = u->num_rows;
    (u2).num_columns = u->num_columns;
    (u2).grid = (int*)calloc((u2).num_rows*(u2).num_columns, sizeof(int));


    int row, column;
    int cell_next;
    int current_cells_alive = 0;
    
    // apply rules to new universe so that earlier operations don't affect later ones
    for ( row = 0; row < u->num_rows; row++){
        for ( column = 0; column < u->num_columns; column++){
            cell_next = rule(u, column, row);
            (u2).grid[column + row*(u->num_columns)] = cell_next;
            current_cells_alive += cell_next;
        }
    }

    // update universe metadata (current cells remains constant)
    u->total_cells += (u->num_columns)*(u->num_rows);
    u->current_cells_alive = current_cells_alive;
    u->total_cells_alive += current_cells_alive;

    // update universe data
    for ( row = 0; row < u->num_rows; row++){
        for ( column = 0; column < u->num_columns; column++){
            u->grid[column + row*(u->num_columns)] = 
             (u2).grid[column + row*(u->num_columns)];
        }
    }

    free(u2.grid);
    
    return;

}

void print_statistics(struct universe *u){
    
    float current_alive_perc = (u->current_cells_alive)/(u->current_cells) * 100;
    float total_alive_perc = (u->total_cells_alive)/(u->total_cells) * 100;

    printf("%.3f%% of cells currently alive\n", current_alive_perc);
    printf("%.3f%% of cells alive on average\n", total_alive_perc);

}
