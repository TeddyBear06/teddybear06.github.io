#include <stdio.h>
#include <sqlite3.h> 

void profile(void *context, const char *sql, sqlite3_uint64 ns) {

   fprintf(stderr, "Query: %s\n", sql);
   fprintf(stderr, "Execution Time: %llu ms\n", ns / 1000000);

}

void printColumnValue(sqlite3_stmt* stmt, int col) {

  int colType = sqlite3_column_type(stmt, col);

  switch(colType) {

    case SQLITE_INTEGER:
         printf("  %3d   ", sqlite3_column_int(stmt, col));
         break;

    case SQLITE_FLOAT:
         printf("  %5.2f", sqlite3_column_double(stmt, col));
         break;

    case SQLITE_TEXT:
         printf("  %-5s", sqlite3_column_text(stmt, col));
         break;

    case SQLITE_NULL:
         printf("  null");
         break;

    case SQLITE_BLOB:
         printf("  blob");
         break;
    }

}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

   int i;
   
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   
   return 0;

}

int main(int argc, char* argv[]) {
   
   sqlite3 *db;
   int rc;
   char *zErrMsg = 0;
   sqlite3_stmt *stmt;

   if ( sqlite3_open("test.db", &db) != SQLITE_OK ) {
      fprintf(stderr, "Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      sqlite3_profile(db, &profile, NULL);
      fprintf(stderr, "Base de données ouverte avec succès !\n");
   }

   rc = sqlite3_exec(db, "SELECT * FROM Genre LIMIT 1", callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

   rc = sqlite3_prepare_v2(db, "SELECT * FROM Genre WHERE Name = ?", -1, &stmt, NULL);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   sqlite3_bind_text(stmt, 1, "Pop", -1, NULL);

   char * expandedSQL = sqlite3_expanded_sql(stmt);

   printf("%s", expandedSQL);
   printf("\n");

   while (sqlite3_step(stmt) != SQLITE_DONE) {
      for (int col=0; col<=1; col++) {
         printColumnValue(stmt, col);
      }
      printf("\n");
   }

   sqlite3_finalize(stmt);

   sqlite3_close(db);

}