#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <windows.h>
#include <mysql.h>
#include<string.h>
#include <time.h>


MYSQL *conn;
int main()
{
    printf("\t\t\t\t\tLIBRARY MANAGEMENT SYSTEM\n\n\n");
    conn = mysql_init(NULL);
    conn = mysql_real_connect(conn,"192.168.43.151", "admin" , "admin" , "library",0,NULL,0);
    if (conn){
        printf("CONNECTED\n");
    }
    else{
        printf("NOPE\n");
    }

    char str[20];
    int l,terminate;
    printf("\nEnter the password: ");
    scanf("%s",str);
    l=password(str);
    while(l==0)
    {
       printf("\nEnter the password again: ");
       scanf("%s",str);
       l=password(str);
    }
    printf("\nEnter -1 to terminate the program or any other number to continue: ");
    scanf("%d",&terminate);
    while(terminate!=-1)
	{
	    main_menu();
	    printf("\nEnter -1 to terminate the program or any other number to continue: ");
	    scanf("%d",&terminate);
	}
	return 0;
}

int password(char str[])
{
   char str1[]={'s','p','i','t'};
   if(strcmp(str,str1)==0)
	return  1;
	else return 0;
}

int main_menu()
{
  int choice,rv;
  printf("\n1: Add books\n2: Delete books\n3: Search books\n4: Issue books\n5: View booklist\n6: Return book\n");
  printf("\nEnter your choice: ");
  scanf("%d",&choice);

  	switch(choice)
	 {
       case 1:addbook(conn);break;
       case 2:deleteb(conn);break;
	   case 3:{
	      rv=search(conn);
	      if(rv==1)
            printf("\nBook Available");
          else printf("\nNot Available");

	   };break;
	   case 4:issuebook(conn);break;
 	   case 5:viewbk(conn);break;
 	   case 6:returnb(conn);break;
       default:printf("\nEnter choice from 1 to 5:");
       	}

}

void viewbk(MYSQL *conn){
    MYSQL_RES *res;
    MYSQL_ROW row;

    mysql_query(conn,"SELECT * FROM book");

    res = mysql_store_result(conn);
    while(row = mysql_fetch_row(res)){
        printf("%20s|\t%20s|\t%20s|\t%20s|\n", row[0], row[1], row[2], row[3]);
    }
}

int search(MYSQL *conn){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char bn [30];
    printf("\nEnter the book to be searched: ");
    scanf(" %[^\n]%*c",bn);

      mysql_query(conn,"SELECT Book_Name FROM book");

      res = mysql_store_result(conn);
      while(row = mysql_fetch_row(res)){
        if(strcmp(bn,row[0]) == 0)
        return 1;

    }
    return 0;
}

int addbook(MYSQL *conn){
    struct newbook{
    int booki;
    char bookn[30];
    char aut[30];
    }b[100];
    int i,n,res;
    char query[128] = {0};
    char stat[30]= {'O','n',' ','s','h','e','l','f','\0'};
    printf("\nEnter the number of books to be added: ");
    scanf("%d",&n);
    for(i=0;i<n;i++)
    {
        printf("\nEnter the book name: ");
        scanf(" %[^\n]%*c",b[i].bookn);
        printf("\nEnter book id number: ");
        scanf("%d",&b[i].booki);
         printf("\nEnter author name: ");
        scanf(" %[^\n]%*c",b[i].aut);
        snprintf(query, 128, "INSERT INTO book VALUES('%s',%d,'%s','%s')",b[i].bookn,b[i].booki,b[i].aut,stat);

    if(mysql_query(conn,query) == 0)
        printf("\nBook added");
    }

}

int deleteb(MYSQL *conn)
{
    int booki;
    char query[128] = {0};
    printf("\nEnter the book id of the book to be deleted: ");
    scanf("%d",&booki);
    snprintf(query,128,"DELETE FROM book where Book_id=%d and Status='On shelf'",booki);


    if(mysql_query(conn,query) == 0)
        printf("\nBook deleted");

}


void issuebook(MYSQL *conn)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char uid[20],query[500];
    printf("Enter UID : ");
    fflush(stdin);
    scanf("%s",uid);
    sprintf(query,"SELECT * FROM student WHERE UID_NO=%s",uid);
    if(mysql_query(conn,query)){
        fprintf(stderr,"Query Failed ");
        return;
    }

    res = mysql_store_result(conn);

    if((int)mysql_num_rows(res) < 3 ){
        char book_id[5],query_1[500],query_2[500],query_3[500];
        //if( (int)mysql_num_rows(res) != 0){

            while(row = mysql_fetch_row(res)){
                if(atoi(row[4]) > 0){
                    printf("Penalties Found\n");
                    return;
                }

            }
        //}
        printf("Enter BooK ID : ");
        fflush(stdin);
        scanf("%s",book_id);


        sprintf(query_1,"INSERT INTO student (UID_NO, Book_id, Issue_Date, Return_Date, Penalties) VALUES (%s, %s,CURRENT_DATE(),'0000-00-00','0')",uid,book_id);
        sprintf(query_2,"UPDATE book SET Status ='Issued' WHERE Book_id=%s",book_id);

        if(mysql_query(conn,query_1)){
            fprintf(stderr,"Query Failed");
            return;
        }
        if(mysql_query(conn,query_2)){
            fprintf(stderr,"Query Failed");
            return;
        }
        return;

    }

    else printf("\nCan't issue more than three books");



}

int returnb(MYSQL *conn)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char book_id [5],query[500],query_1[500],query_2[500],query_3[100],date[30];
    char issue_data[100];//to store issued date of a book
   	int penalties = 0,days;

    printf("\nEnter Book id: ");
    scanf("%s",book_id);
    printf("\nEnter return date in (YYYY-MM-DD)format: ");
    scanf("%s",date);
     sprintf(query,"SELECT * FROM student WHERE Book_id=%s",book_id);
    if(mysql_query(conn,query)){
        fprintf(stderr,"Query Failed ");
        printf("Failed");
        return;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);

    for(int i=0; i<5; i++)
    {
        strcpy(issue_data,row[i]);
    mysql_free_result(res);
   // printf("%s\n",issue_data);
    }


    sprintf(query_1,"SELECT DATEDIFF('%s','%s')",issue_data,date);

     if(mysql_query(conn,query_1)){
            fprintf(stderr,"Query1 Failed");
            return;
        }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    days = atoi(row[1]);

    if( days > 7){
    	penalties = (days-7)*5;
    }
    mysql_free_result(res);
    sprintf(query_2,"UPDATE student SET Penalties=%d, Return_Date='%s' WHERE Book_id=%s",penalties,date,book_id);

    if(mysql_query(conn,query_2)){
    	fprintf(stderr, "Query2 Failed \n");
    	return;
    }
    sprintf(query_3,"UPDATE book SET Status ='On shelf' WHERE Book_id=%s",book_id);
    if(mysql_query(conn,query_3)){
            fprintf(stderr,"Query Failed");
            return;
        }


    mysql_free_result(res);

}
