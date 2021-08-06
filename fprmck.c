/*

    fprmck - File Permission Checker v. 1.0
    by Tymon Zagawa
    Thu, Aug 6, 2021 

    ======================================================================

    By this program you can check if permissions had changes during
    runtime of this program.

    ======================================================================

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    ======================================================================

    I know that code might be dirty, but it works. In future versions
    some problems will be eliminated, and some features will be added.


*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

// Handling of CTRL+C for exit code 0
static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

// Function for checking time
char* timestamp(){
time_t rawtime; struct tm * timeinfo; time ( &rawtime ); timeinfo = localtime ( &rawtime );
char* TM=asctime(timeinfo);
TM[strcspn(TM, "\n")] = 0;  //this trims '\n' character from TM; thanks to https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
return TM;
}

//function for printing help
void USAGE_INFO(int a){
	printf("USAGE: fprmck\n	-f [FILENAME] - path to the tested file\n	-m [TIME] - time in miliseconds, where max is 360000000\n	-s [TIME] - time in seconds, where max is 360000000\n	-l [FILENAME] - create log file\n	-h - help\n\n");
	exit(a);
}


// ****************** MAIN FUNCTION ******************
int main (int argc, char *argv[])
{
	//************ values
	int index;
	char* filepath;
	char* logpath;
	FILE *fp;
	int status;
	struct stat buffer;
		struct passwd *pw = getpwuid(buffer.st_uid); // Owner UID
		struct group  *gr = getgrgid(buffer.st_gid);   // Owner's group GID
		char OWN[12], SOWN[12], OWNG[12], SOWNG[12]; // OWNer, Summary OWNer, OWNer Group, Summary OWNer Group
	int OWP=0; int GRP=0; int OTP = 0; // Permisions
	int SOWP=0; int SGRP=0; int SOTP=0; // Summary Permissions
	int IV=0; // Iteration Value; check start value; if is on IV=1 than message about permission changes is enabled
	int LOG=0; // logs - 0 means disable, 1 means enable
	int FPTH=0; // for checking if filepath exists

	char* IVT="1"; // interval time
	int IVTT=0; // interval time type - 2 for microseconds, 0 or 1 for seconds
	
	char IVTS[12]; // limit is 3600000000 - so number can fits within 12 characters
	long int NUMER;
	int opt; // value for command line arguments


	//************ command line arguments
	while((opt = getopt(argc, argv, "f:hil:m:s:")) != -1) 
	{ 
		switch(opt)
		{
			case 'l': // log enabled with default log path
				LOG=1;
				logpath=optarg;
				fp = fopen(logpath, "w");
				if(fp==NULL){
				printf("Cannot create %s log file.\n", logpath);
				exit(1);
				}
				fclose(fp);

                                break;
			case 'f': // check if file exists
				filepath=optarg;	
				if( access( filepath, F_OK ) == 0 ) {
					FPTH=1;
				} else {
					printf("File %s does not exists.\n", filepath);
					return 1; //Traditionally in UNIX and UNIX-like systems error code 1 typically denoting a generic failure
					}
				break;

			case 'm':
				IVTT=IVTT+2;
				IVT=optarg;
				break;
			case 's':
				IVTT=IVTT+1;
				IVT=optarg;
				break;
			case ':':
				printf("option requires an argument --%s\n", opt);
				break;
			case '?':
				USAGE_INFO(1);
			case 'h':
				USAGE_INFO(0);
		}


	}
		// optind is for the extra arguments
		// which are not parsed
		for(; optind < argc; optind++){
		printf("extra arguments: %s\n", argv[optind]);
		}



	//************ introduction; check if option -f was used
	if(FPTH==1){
	printf ("CHECK PERMISSIONS CHANGES OF FILE: %s\n", filepath);
	}else{
	printf("Option -f is necessary.\n");
	}

	//************ check if option -m or -s was choosen
	if(IVTT==3){
	printf("You can't use option -s and -m in the same time.\n");
	USAGE_INFO(1);
	}

	//************ check if time value isn't too big

	if(strlen(IVT)>9){
		printf("Interval time value in miliseconds is too big.\n");
		USAGE_INFO(1);
	}

        //************ check if necessary parameters has been used
	if(FPTH==0){
		USAGE_INFO(1);
	}

	
	//************ check interval time

	sprintf(IVTS, "%d", atoi(IVT));  // 
	if(isalpha(*IVT)||ispunct(*IVT)){
		USAGE_INFO(1);
	}

	if(strlen(IVTS)!=strlen(IVT)){
		USAGE_INFO(1);
	}		


	//************ check size of IVT value
	NUMER=atoi(IVT);
	if(NUMER>360000000){  // powinno byc nie 360000000, ale 3600000000 - ale wypierdala licznik :V 
		printf("Interval time value is too big.\nInstead of miliseconds you can use just seconds.\n");
		USAGE_INFO(1);
	}


	//************ create log file
	if(LOG==1){
		printf("LOG: enabled\n");

		printf("LOGPATH:  %s\n", logpath);
		printf("FILEPATH:  %s\n\n", filepath);
	
		fp = fopen(logpath, "a");
		fprintf(fp, "======== LOG FILE OF %s ========\n", filepath);
		fclose(fp);
	}
	else{
	printf("LOG: disabled\n");
	printf("FILEPATH:  %s\n\n", filepath);
	}


        //************ write timestamp into log file
        if(LOG==1){
		fp = fopen(logpath, "a");
		fprintf(fp, "START: %s\n", timestamp());
		fprintf(fp, "INTERVAL: %d ", NUMER);
			if(IVTT==2){
				if(NUMER==1){fprintf(fp, "microsecond\n");
				}else{fprintf(fp, "microseconds\n");}}
			if(IVTT==1){
				if(NUMER==1){fprintf(fp, "second\n");
				}else{fprintf(fp, "seconds\n");}}
			if(IVTT==0){fprintf(fp, "second\n");}	
		fclose(fp);
	}

	status = stat(filepath, &buffer);
	pw = getpwuid(buffer.st_uid);
	strcpy(OWN, pw->pw_name);
	strcpy(SOWN, pw->pw_name);
	gr = getgrgid(buffer.st_gid);
	strcpy(OWNG, gr->gr_name);
	strcpy(SOWNG, gr->gr_name);



	// ****************** LOOP ******************
	signal(SIGINT, intHandler); // Handling of CTRL+C
	while(keepRunning){
	//************ check if something goes wrong with file during the executing of program
		if( access( filepath, F_OK ) != 0 ) {
			printf ("Error: Something went wrong durng the execution of file.\nRe-run the program, or manually check if file %s exists.\n", filepath);
			return 1;
		} 

	//************ reset values
		status = stat(filepath, &buffer);	
		int OWP=0; int GRP=0; int OTP = 0;
		pw = getpwuid(buffer.st_uid);
		strcpy(OWN, pw->pw_name);
		gr = getgrgid(buffer.st_gid);
		strcpy(OWNG, gr->gr_name);

	//************ collect values	
		// Reminder:
		// 0 - no permissions 
		// 1 - execute
		// 2 - write
		// 4 - read
			
		//Owner permissions
		if( buffer.st_mode & S_IRUSR ){
			OWP=OWP+4;
			}
		if( buffer.st_mode & S_IWUSR ){	
			OWP=OWP+2;
			}
		if( buffer.st_mode & S_IXUSR ){
			OWP=OWP+1;
			}

		//Group permissions
		if( buffer.st_mode & S_IRGRP ){
			GRP=GRP+4;
			}
		if( buffer.st_mode & S_IWGRP ){
			GRP=GRP+2;
			}
		if( buffer.st_mode & S_IXGRP ){
			GRP=GRP+1;
			}

		//Others permissions
		if( buffer.st_mode & S_IROTH ){
			OTP=OTP+4;
			}
		if( buffer.st_mode & S_IWOTH ){
			OTP=OTP+2;
			}
		if( buffer.st_mode & S_IXOTH ){
			OTP=OTP+1;	
			}

		//Compare permissions and write informations to the log file 
		if(IV==1){
                if(SOWP!=OWP | SGRP!=GRP | SOTP!=OTP){
			printf("%s Permissions have chaged from %i%i%i to %i%i%i.\n",timestamp(),SOWP, SGRP, SOTP, OWP, GRP, OTP);
			if(LOG==1){
				fp = fopen(logpath, "a");
				fprintf(fp,"%s: Permision have changed from %i%i%i to %i%i%i.\n",timestamp(), SOWP, SGRP, SOTP, OWP, GRP, OTP);

				fclose(fp);
			}
                }

		//Compare ownership
		if(strcmp(SOWN,OWN)){   // Compare ownership
			printf("%s File owner has changed from %s to %s.\n", timestamp(), SOWN, OWN);
			if(LOG==1){
				fp = fopen(logpath, "a");
				fprintf(fp,"%s: File owner has changed from %s to %s.\n", timestamp(), SOWN, OWN);

 				fclose(fp);
			}

		}
		//Compare groups
		if(strcmp(SOWNG,OWNG)){
			printf("%s File owner group has changed from %s to %s.\n", timestamp(), SOWNG, OWNG);
			if(LOG==1){
			fp = fopen(logpath, "a");
			fprintf(fp,"%s: File owner group has changed from %s to %s.\n", timestamp(), SOWNG, OWNG);

			fclose(fp);

			}
		}
		}
		
		// Values to compare	
		SOWP=OWP;
		SGRP=GRP;
		SOTP=OTP;
		strcpy(SOWN, OWN);
		strcpy(SOWNG, OWNG);

		IV=1;
	
		// sleep and do next iteration	
		if(IVTT==2){
			usleep(NUMER);	// sleep in microseconds
		}
		if(IVTT==1){
			sleep(NUMER); // sleep in seconds
		}
		if(IVTT==0){
			sleep(1);
		}


	}

for (index = optind; index < argc; index++)
	printf ("Non-option argument %s\n", argv[index]);
	
	//exit with 0    
	return  0;
}
