#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdbool.h>

#define LIMIT 2
#define NUMBER_OF_COLS 2

struct data_chunk {
   unsigned int current_number_of_rows;
   const unsigned int max_number_of_rows;
   const unsigned int number_of_cols;
   char **data;
};

char **get_row(struct data_chunk *chunk, unsigned int row) {
   char **data= chunk->data;
   return (data+row*(chunk->number_of_cols));
}

bool is_full(struct data_chunk * chunk) {
   return (chunk->current_number_of_rows < chunk->max_number_of_rows) ? false : true;
}

int store_row_to_mem(void *result_store, int number_of_cols, char **results, char **colNames) {
   struct data_chunk * result_holder = (struct data_chunk *)result_store;
   if (number_of_cols != result_holder->number_of_cols) { printf("result data don't align with pre-defiend data chunk dimensions" );}
   if (!is_full(result_holder)) {
       char **current_row = get_row(result_holder, result_holder->current_number_of_rows);
       for(int col=0; col < number_of_cols; col++) {
          static char * data_string;
          data_string = malloc(20*sizeof(char));
          strcpy(data_string, results[col]);
          ((struct data_chunk *)result_store)->data[result_holder->current_number_of_rows*NUMBER_OF_COLS+col] = data_string;

       }
   }
   result_holder->current_number_of_rows++;
   return 0;
}


struct data_chunk * read_table_to_mem(sqlite3* db, const char* table_name, const char ** cols, const unsigned int offset) {

   char cols_part[100]="";
   char query[100];
   char *err_msg = 0;
   static struct data_chunk chunk = { .current_number_of_rows = 0, .max_number_of_rows = LIMIT, .number_of_cols = NUMBER_OF_COLS};
   chunk.data = malloc(LIMIT*NUMBER_OF_COLS*sizeof(char *));

   for (int count=0; count < NUMBER_OF_COLS; count++) {
       strcat(cols_part, cols[count]);
       if (count < NUMBER_OF_COLS-1) {
          strcat(cols_part, ",");
       }
   }

   sprintf(query, "SELECT %s FROM %s", cols_part, table_name);
   int rc = sqlite3_exec(db, query, store_row_to_mem, &chunk, &err_msg);
   return &chunk;

}

void display_data(struct data_chunk * chunk) {
    for(int row=0; row < chunk->max_number_of_rows; row++) {
        for(int col=0; col < chunk->number_of_cols; col++) {
           printf("%s",*(chunk->data[row*NUMBER_OF_COLS+col]));
        }
        printf("\n");
    }
}
/**
int main() {

   sqlite3 *db;
   char *table = "friends";
   const char *cols[2] = {"firstname", "lastname"};

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
       fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
       sqlite3_close(db);
       return 1;
   }

   struct data_chunk * data = read_table_to_mem(db, table, cols, 0);
   display_data(data);
}
**/
