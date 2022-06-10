/* connect to server with the CLIENT_MULTI_STATEMENTS option */
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

MYSQL mysql;
const char *host_name = NULL;
const char *user_name = "root";
const char *password = "";
const char *db_name = "friends";
unsigned int port_num = 3306;
const char *socket_name = NULL;
int status;
MYSQL_RES * result;



int main() {
				mysql_init(&mysql);


				if (mysql_real_connect(&mysql, "127.0.0.1", user_name, password,
        db_name, port_num, socket_name, CLIENT_MULTI_STATEMENTS) == NULL)
				{
								printf("mysql_real_connect() failed\n");
				    mysql_close(&mysql);
				    exit(1);
				}

				/* execute multiple statements */
				status = mysql_query(&mysql,
					"CREATE TABLE friend_names(first_name VARCHAR(20), last_name VARCHAR(20)); \
					INSERT INTO friend_names VALUES('Mark', 'Li');");
				if (status)
				{
				    printf("Could not execute statement(s)");
				    mysql_close(&mysql);
				    exit(0);
				}

				/* process each statement result */
				do {
				  /* did current statement return data? */
				   result = mysql_store_result(&mysql);
				   if (result)
				   {
				    /* yes; process rows and free the result set */
				       mysql_free_result(result);
				   }
				   else          /* no result set or error */
				   {
				       if (mysql_field_count(&mysql) == 0) {
				           printf("%lld rows affected\n",
					          mysql_affected_rows(&mysql));
				       } else {
				           printf("Could not retrieve result set\n");
				           break;
				       }
				  }
				  /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
				  if ((status = mysql_next_result(&mysql)) > 0)
				    printf("Could not execute statement\n");
	 				} while (status == 0);

				  mysql_close(&mysql);
}
