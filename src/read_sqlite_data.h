struct data_chunk {
   unsigned int current_number_of_rows;
   const unsigned int max_number_of_rows;
   const unsigned int number_of_cols;
   char **data;
};
char **get_row(struct data_chunk *chunk, unsigned int row);
bool is_full(const struct data_chunk * chunk);
int *process_row(void *result_holder, int number_of_cols, char **results, char **colNames);
int store_row_to_mem(void *result_store, int number_of_cols, char **results, char **colNames);
struct data_chunk * read_table_to_mem(sqlite3* db, const char* table_name, const char ** cols, const unsigned int offset);
void display_data(struct data_chunk * chunk);
