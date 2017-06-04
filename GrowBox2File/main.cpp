#include <cstdio>
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <mysql/mysql.h>

struct connection_details
{
	char *server;
	char *user;
	char *password;
	char *database;
};

MYSQL* mysql_connection_setup(struct connection_details mysql_details)
{
	// first of all create a mysql instance and initialize the variables within
	MYSQL *connection = mysql_init(NULL);

	// connect to the database with the details attached.
	if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
		printf("Conection error : %s\n", mysql_error(connection));
		return NULL;
	}
	return connection;
}


MYSQL_RES* mysql_perform_query(MYSQL *connection, char *sql_query)
{
	// send the query to the database
	if (mysql_query(connection, sql_query))
	{
		printf("MySQL query error : %s\n", mysql_error(connection));
		return NULL;
	}
	return mysql_use_result(connection);
}

int main()
{
	struct connection_details mysqlD;
	mysqlD.server = "localhost";  // where the mysql database is
	mysqlD.user = "root";     // the root user of mysql   
	mysqlD.password = "qweqwe!23"; // the password of the root user in mysql
	mysqlD.database = "GrowBox";    // the databse to pick
	unsigned char buffer[25] = { 0 };
	FILE* pFile;
	pFile = fopen("/home/perdidor/GrowBox.data", "w");
	fwrite(buffer, 1, sizeof(buffer), pFile);
	fclose(pFile);
    //printf("hello from GrowBox2File!\n");
	int file_i2c;
	int length = 25;
	char *filename = (char*)"/dev/i2c-2";
	bool success = false;
	while (!success)
	{
		success = true;
		if ((file_i2c = open(filename, O_RDWR)) < 0)
		{
			//ERROR HANDLING: you can check errno to see what went wrong
			//printf("Failed to open the i2c bus");
			success = false;
			//return 1;
		}
		int addr = 0x33;          //<<<<<The I2C address of the slave
		if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
		{
			//printf("Failed to acquire bus access and/or talk to slave.\n");
			success = false;
			//ERROR HANDLING; you can check errno to see what went wrong
			//return 1;
		}
		sleep(1);
	}
	while (true)
	{
		
		unsigned char buffer[25] = { 0 };
		(read(file_i2c, buffer, length) != length);		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
		int firstlfrpmbyte = buffer[2];
		int secondlfrpmbyte = buffer[3];
		int firstmfrpmbyte = buffer[4];
		int secondmfrpmbyte = buffer[5];
		int wholelighttemp = buffer[6];
		int fractlighttemp = buffer[7];
		int wholeambienttemp = buffer[8];
		int fractambienttemp = buffer[9];;
		int wholeambienthum = buffer[10];
		int fractambienthum = buffer[11];
		int firstsoilbyte = buffer[12];
		int secondsoilbyte = buffer[13];
		bool lighton = (buffer[14] == 1);
		bool lightfanon = (buffer[15] == 1);
		bool mainfanon = (buffer[16] == 1);
		bool sprinkleron = (buffer[17] == 1);
		bool wateringon = (buffer[18] == 1);
		bool reservedon = (buffer[19] == 1);
		bool windon = (buffer[20] == 1);
		int firsthbbyte = buffer[21];
		int secondhbbyte = buffer[22];
		int thirdhbbyte = buffer[23];
		int fourthhbbyte = buffer[24];
		int heartbeats = firsthbbyte * 16777215 + secondhbbyte * 65535 + thirdhbbyte * 255 + fourthhbbyte;
		int lightfanrpm = firstlfrpmbyte * 255 + secondlfrpmbyte;
		int mainfanrpm = firstmfrpmbyte * 255 + secondmfrpmbyte;
		double lighttemp = wholelighttemp + fractlighttemp * 0.01;
		double ambienttemp = wholeambienttemp + fractambienttemp * 0.01;
		double ambienthum = wholeambienthum + fractambienthum * 0.01;
		double soil = (firstsoilbyte * 255 + secondsoilbyte) / 10.24;
		char query[1024]; //declare query char variable
		time_t _tm = time(NULL);
		struct tm * curtime = localtime(&_tm);
		sprintf(query, "INSERT INTO GrowBoxTelemetry (datetime,timestamp,lightfanon,lightfanrpm,mainfanon,mainfanrpm,lighttemp,lighton,ambienttemp,ambienthum,soilsensorvalue,sprinkleron,wateringon,reservedon,windon,heartbeats) VALUES(\'%s\',%lu,%d,%d,%d,%d,%.2f,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d)", asctime(curtime), (unsigned long)time(NULL), (int)lightfanon, lightfanrpm, (int)mainfanon, mainfanrpm,lighttemp,(int)lighton,ambienttemp,ambienthum,soil,(int)sprinkleron,(int)wateringon,(int)reservedon,(int)windon,heartbeats);
		MYSQL *conn = mysql_connection_setup(mysqlD);// connect to the mysql database
		if (conn != NULL)
		{
				mysql_query(conn, query);
				//printf(mysql_error(conn));
				mysql_close(conn);
				//printf(mysql_error(conn));
		}
		sleep(2);
	}
    return 0;
}