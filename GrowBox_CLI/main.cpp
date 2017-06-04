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
#include <curl/curl.h>
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
	MYSQL *conn;      // the connection
	MYSQL_RES *res;   // the results
	MYSQL_ROW row;    // the results row (line by line)

	struct connection_details mysqlD;
	mysqlD.server = "localhost";  // where the mysql database is
	mysqlD.user = "root";     // the root user of mysql   
	mysqlD.password = "qweqwe!23"; // the password of the root user in mysql
	mysqlD.database = "GrowBox";    // the databse to pick
	conn = mysql_connection_setup(mysqlD);// connect to the mysql database

	WINDOW * mainwin;
	if ((mainwin = initscr()) == NULL) {
		fprintf(stderr, "Error initialising ncurses.\n");
		return 1;
	}
	int errorcount = 0;
	int cyclecount = 0;
	int lightfanrpm = 0;
	int mainfanrpm = 0;
	float lighttemp = 0.0;
	float ambienttemp = 0.0;
	float ambienthum = 0.0;
	float soil = 0.0;
	int heartbeats = 0;
	bool lighton = false;
	bool lightfanon = false;
	bool mainfanon = false;
	bool sprinkleron = false;
	bool wateringon = false;
	bool reservedon = false;
	bool windon = false;
	while (true)
	{
		if (cyclecount > 0) sleep(1);
		bool success = false;
		char query[1024]; //declare query char variable
		sprintf(query, "SELECT datetime,timestamp,lightfanon,lightfanrpm,mainfanon,mainfanrpm,lighttemp,lighton,ambienttemp,ambienthum,soilsensorvalue,sprinkleron,wateringon,reservedon,windon,heartbeats,timestamp FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1");
		mysql_query(conn, query);
		res = mysql_store_result(conn);
		if (mysql_num_fields(res) > 0)
		{
			success = true;
			row = mysql_fetch_row(res);
			int n = strcmp(row[7], "1");
			lighton = (n == 0);
			n = strcmp(row[2], "1");
			lightfanon = (n == 0);
			n = strcmp(row[4], "1");
			mainfanon = (n == 0);
			n = strcmp(row[11], "1");
			sprinkleron = (n == 0);
			n = strcmp(row[12], "1");
			wateringon = (n == 0);
			n = strcmp(row[13], "1");
			reservedon = (n == 0);
			n = strcmp(row[14], "1");
			windon = (n == 0);
			lightfanrpm = atoi(row[3]);
			mainfanrpm = atoi(row[5]);
			lighttemp = atof(row[6]);
			ambienttemp = atof(row[8]);
			ambienthum = atof(row[9]);
			soil = atof(row[10]);
			heartbeats = atoi(row[15]);
			time_t _tm = time(NULL);
			struct tm * curtime = localtime(&_tm);
			if (((unsigned long)time(NULL) - atoi(row[16])) > 10)
			{
				success = false;
				errorcount++;
			}
		}
		cyclecount++;
		start_color();
		init_pair(2, COLOR_YELLOW, COLOR_BLACK);
		init_pair(3, COLOR_RED, COLOR_BLACK);
		init_pair(8, COLOR_RED, -1);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
		init_pair(7, COLOR_GREEN, -1);
		init_pair(5, COLOR_WHITE, COLOR_BLACK);
		init_pair(6, COLOR_BLUE, COLOR_BLACK);
		attron(COLOR_PAIR(5));
		mvaddstr(2, 1, "Light Fan RPM: ");
		char * lfrpmstr = "";
		if (lightfanon)
		{
			if (lightfanrpm > 1500) attron(COLOR_PAIR(4)); else attron(COLOR_PAIR(3));
			lfrpmstr = "%d, Status: ON\t\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			lfrpmstr = "%d, Status: OFF\t\t";
		}
		printw(lfrpmstr,lightfanrpm);
		attron(COLOR_PAIR(5));
		mvaddstr(3, 1, "Main Fan RPM: ");
		char * mfrpmstr = "";
		if (mainfanon)
		{
			if (mainfanrpm > 750) attron(COLOR_PAIR(4)); else attron(COLOR_PAIR(3));
			mfrpmstr = "%d, Status: ON\t\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			mfrpmstr = "%d, Status: OFF\t\t";
		}
		printw(mfrpmstr, mainfanrpm);
		attron(COLOR_PAIR(5));
		mvaddstr(4, 1, "Light Temp, 'C: ");
		if ((lighttemp - ambienttemp) <= 5) attron(COLOR_PAIR(4));
		if ((lighttemp - ambienttemp) > 5) attron(COLOR_PAIR(2));
		if (((lighttemp - ambienttemp) > 30) || (lighttemp > 70)) attron(COLOR_PAIR(3));
		if (lighttemp == 0) attron(COLOR_PAIR(6));
		printw("%.2f\t", lighttemp);
		attron(COLOR_PAIR(5));
		mvaddstr(5, 1, "Light status: ");
		char * lightstatus = "";
		if (lighton)
		{
			attron(COLOR_PAIR(4));
			lightstatus = "ON\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			lightstatus = "OFF\t";
		}
		printw(lightstatus);
		attron(COLOR_PAIR(5));
		mvaddstr(6, 1, "Ambient Temp, 'C: ");
		if (ambienttemp < 35) attron(COLOR_PAIR(4));
		if (ambienttemp >= 35) attron(COLOR_PAIR(2));
		if (ambienttemp <= 5) attron(COLOR_PAIR(6));
		printw("%.2f\t", ambienttemp);
		attron(COLOR_PAIR(5));
		mvaddstr(7, 1, "Ambient Relative Humidity, %: ");
		if (ambienthum < 35) attron(COLOR_PAIR(2));
		if ((ambienthum >= 35) && (ambienthum <= 80)) attron(COLOR_PAIR(4));
		if (ambienthum >= 81) attron(COLOR_PAIR(3));
		printw("%.2f\t", ambienthum);
		attron(COLOR_PAIR(5));
		mvaddstr(8, 1, "Soil Sensor, %: ");
		if (soil <= 20) attron(COLOR_PAIR(3));
		if (soil < 50) attron(COLOR_PAIR(2));
		if (soil >= 50) attron(COLOR_PAIR(4));
		printw("%.2f\t", soil);
		attron(COLOR_PAIR(5));
		mvaddstr(9, 1, "Sprinkler status: ");
		char * sprinklerstatus = "";
		if (sprinkleron)
		{
			attron(COLOR_PAIR(4));
			sprinklerstatus = "ON\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			sprinklerstatus = "OFF\t";
		}
		printw(sprinklerstatus);
		attron(COLOR_PAIR(5));
		mvaddstr(10, 1, "Watering status: ");
		char * wateringstatus = "";
		if (wateringon)
		{
			attron(COLOR_PAIR(4));
			wateringstatus = "ON\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			wateringstatus = "OFF\t";
		}
		printw(wateringstatus);
		attron(COLOR_PAIR(5));
		mvaddstr(11, 1, "Reserved Relay status: ");
		char * wreservedstatus = "";
		if (reservedon)
		{
			attron(COLOR_PAIR(4));
			wreservedstatus = "ON\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			wreservedstatus = "OFF\t";
		}
		printw(wreservedstatus);
		attron(COLOR_PAIR(5));
		mvaddstr(12, 1, "Wind status: ");
		char * windstatus = "";
		if (windon)
		{
			attron(COLOR_PAIR(4));
			windstatus = "ON\t";
		}
		else
		{
			attron(COLOR_PAIR(5));
			windstatus = "OFF\t";
		}
		printw(windstatus);
		attron(COLOR_PAIR(5));
		mvaddstr(13, 1, "Last status: ");
		if (success)
		{
			attron(COLOR_PAIR(4));
			printw("Device online\t");
		}
		else
		{
			attron(COLOR_PAIR(3));
			printw("Device offline\t");
		}
		attron(COLOR_PAIR(5));
		mvaddstr(14, 1, "Heartbeats since start (relative reliability):");
		attron(COLOR_PAIR(4));
		printw("%d\t\t", heartbeats);
		attron(COLOR_PAIR(5));
		//mvaddstr(15, 1, "I2C bus read error rate: ");
		//attron(COLOR_PAIR(3));
		//attron(COLOR_PAIR(3));
		//printw("%d", errorcount);
		//attron(COLOR_PAIR(5));
		//printw("/%d (%.4f%%)\t", cyclecount, (float)errorcount * 100 / cyclecount);
		refresh();
		//getch();
		//delwin(mainwin);
		//endwin();
		//refresh();
		//return 0;
		//CURL *curl;
		//CURLcode res;

		///* In windows, this will init the winsock stuff */
		//curl_global_init(CURL_GLOBAL_ALL);

		///* get a curl handle */
		//curl = curl_easy_init();
		//if (curl) {
		//	/* First set the URL that is about to receive our POST. This URL can
		//	just as well be a https:// URL if that is what should receive the
		//	data. */
		//	curl_easy_setopt(curl, CURLOPT_URL, "http://postit.example.com/moo.cgi");
		//	/* Now specify the POST data */
		//	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

		//	/* Perform the request, res will get the return code */
		//	res = curl_easy_perform(curl);
		//	/* Check for errors */
		//	if (res != CURLE_OK)
		//		fprintf(stderr, "curl_easy_perform() failed: %s\n",
		//			curl_easy_strerror(res));

		//	/* always cleanup */
		//	curl_easy_cleanup(curl);
		//}
		//curl_global_cleanup();
		}

    //return 0;
}