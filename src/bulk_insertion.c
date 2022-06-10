#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdbool.h>
#include "read_sqlite_data.h"

#define LIMIT 2
#define NUMBER_OF_COLS 2


char* form_insert_query(const char *table_name, const unsigned int number_of_cols) {

      static char query[100];
      char values[50];

      sprintf(query, "INSERT INTO %s ", table_name);
      strcat(query, " VALUES (");
      for (int i=0; i<number_of_cols-1; i++){
         strcat(query, "?,");
      }
      strcat(query, "?);");

      return query;
}

void insertDataChunk(sqlite3* db, const char *table_name, struct data_chunk* chunk, const unsigned int number_of_cols) {

     char* errorMessage = 0;
     const char *szSQL = form_insert_query(table_name, number_of_cols);
     sqlite3_stmt * stmt = 0;
     int rc = sqlite3_prepare(db, szSQL, 100, &stmt, 0);

     sqlite3_mutex_enter(sqlite3_db_mutex(db));

     if (rc == SQLITE_OK) {
        for (int x=0; x<chunk->max_number_of_rows; x++){
            //insert every row
            char **row = get_row(chunk, x);
            for (int col=0; col<chunk->number_of_cols; col++){
                sqlite3_bind_text(stmt, col+1, *(row+col), -1 , NULL);
                printf("%s\n", *(row+col));
            }
            int retVal = sqlite3_step(stmt);
            if (retVal != SQLITE_DONE)
            {
                printf("Commit Failed! %d\n", retVal);
            }
            sqlite3_reset(stmt);
        }
        sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
        sqlite3_finalize(stmt);
     }else{
         fprintf(stderr, "SQL error: %s\n", errorMessage);
         sqlite3_free(errorMessage);
     }

     sqlite3_mutex_leave(sqlite3_db_mutex(db));

}

int main() {

   static struct data_chunk chunk = { .current_number_of_rows = 0, .max_number_of_rows = LIMIT, .number_of_cols = NUMBER_OF_COLS};
   chunk.data = malloc(LIMIT*NUMBER_OF_COLS*sizeof(char *));
   chunk.data[0] = "Chris"; chunk.data[1] = "Paul";
   chunk.data[2] = "Jalen"; chunk.data[3] = "Brown";
   sqlite3 *db;
   char *table = "friends";

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
       fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
       sqlite3_close(db);
       return 1;
   }

   insertDataChunk(db, table, &chunk, NUMBER_OF_COLS);
}
