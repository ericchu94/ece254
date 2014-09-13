#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char *argv[]) {
	DIR *p_dir;
	struct dirent *p_dirent;
	int time_mode;
	time_t now_time;
	struct tm *now;
	int current_year;

	time(&now_time);
	now = localtime(&now_time);
	current_year = now->tm_year;

	// verifying number of arguments
	if (argc != 3) {
		printf("%s: invalid syntax\n", argv[0]);
		exit(0);
	}

	// verifying option argument
	if (strcmp(argv[1], "-u") == 0) {
		time_mode = 0;
	} else if (strcmp(argv[1], "-c") == 0) {
		time_mode = 1;
	} else if (strcmp(argv[1], "-l") == 0) {
		time_mode = 2;
	} else {
		printf("%s: invalid option -- %s\n", argv[0], argv[1]);
		exit(0);
	}

	// check directory exists
	if ((p_dir = opendir(argv[2])) == NULL) {
		printf("opendir(%s) failed\n", argv[2]);
		exit(1);
	}

	chdir(argv[2]);

	// iterate through files
	while ((p_dirent = readdir(p_dir)) != NULL) {
		struct stat buf;
		char *name = p_dirent->d_name;
		mode_t mode;
		char perm[] = "----------\0";
		struct passwd *pwuid;
		char *username;
		struct group *grgid;
		char *group;
		off_t size;
		time_t time;
		struct tm *ltime; 
		char month[3];
		int day;
		char year[5];
		char *link = "";
		char *arrow = "";

		// filename
		if (name == NULL) {
			perror("filename error");
			continue;
		}


		// permissions
		if (lstat(name, &buf) < 0) {
			perror("lstat error");
			continue;
		}

		mode = buf.st_mode;

		if (S_ISDIR(mode)) {
			perm[0] = 'd';
		} else if (S_ISLNK(mode)) {
			perm[0] = 'l';

			// get destination
			link = (char *)malloc(buf.st_size + 1);
			int r = readlink(name, link, buf.st_size + 1);
			link[r] = '\0';

			arrow = " -> ";
		}

		perm[1] = (mode & S_IRUSR) ? 'r' : '-';
		perm[2] = (mode & S_IWUSR) ? 'w' : '-';
		perm[3] = (mode & S_IXUSR) ? 'x' : '-';
		perm[4] = (mode & S_IRGRP) ? 'r' : '-';
		perm[5] = (mode & S_IWGRP) ? 'w' : '-';
		perm[6] = (mode & S_IXGRP) ? 'x' : '-';
		perm[7] = (mode & S_IROTH) ? 'r' : '-';
		perm[8] = (mode & S_IWOTH) ? 'w' : '-';
		perm[9] = (mode & S_IXOTH) ? 'x' : '-';


		// username
		if ((pwuid = getpwuid(buf.st_uid)) == NULL) {
			perror("getpwuid error");
			continue;
		}

		username = pwuid->pw_name;


		// group name
		if ((grgid = getgrgid(buf.st_gid)) == NULL) {
			perror("getgrgid error");
			continue;
		}

		group = grgid->gr_name;


		// size
		size = buf.st_size;


		// time
		switch (time_mode) {
			case 0:
				time = buf.st_atime;
				break;
			case 1:
				time = buf.st_ctime;
				break;
			case 2:
				time = buf.st_mtime;
				break;
		}
		ltime = localtime(&time);

		// month
		strftime(month, 4, "%b", ltime);

		// day
		day = ltime->tm_mday;

		// year
		if (current_year == ltime->tm_year) {
			strftime(year, 6, "%R", ltime);
		} else {
			sprintf(year, "%d", ltime->tm_year + 1900);
		}

		printf("%s %s %s %d %s %d %s %s%s%s\n", perm, username, group, size, month, day, year, name, arrow, link);

		if (link[0] != '\0') {
			free(link);
		}
	}

	return 0;
}

