/* pageio.c --- implements ppageio.h
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct 23 13:08:56 2020 (-0400)
 * Version: 1
 * 
 * Description: Implements pageio.h: pagesave() and pageload()
 * (Module 5 Step 1)
 * 
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "pageio.h"
#include "webpage.h"

#define MAXLEN 100

/* Saves the passed webpage <*pagep> in a file <id> in a directory <dirnm>
 *
 * returns: 0 for success; nonzero otherwise
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirnm) {
	FILE *outf;
	int nlen = strlen(dirnm) + 16, stat, pdep, plen;
	int32_t result = 1;
	char outn[5], mdir[nlen], cmod[nlen], remv[nlen], path[nlen], *purl, *html;
	
	purl = webpage_getURL(pagep);
	pdep = webpage_getDepth(pagep);
	plen = webpage_getHTMLlen(pagep);
	html = webpage_getHTML(pagep);
	
	sprintf(outn, "%d", id);
	strcpy(mdir, "mkdir ");
	strcat(mdir, dirnm);
	strcpy(path, "./");
	strcat(path, dirnm);
	strcat(path, "/");
	strcat(path, outn);
	strcpy(cmod, "chmod 700 ");
	strcat(cmod, path);
	strcpy(remv, "rm ");
	strcat(remv, path);
	
	if (access(dirnm, F_OK) != 0) {
		stat = (int)WEXITSTATUS(system(mdir));
	} else {
		stat = 0;
	}
	
	if (stat == 0) {
		if ((outf = fopen(path, "w")) != NULL) {
			system(cmod);
			
			if (access(path, W_OK) == 0 && purl != NULL && html != NULL) {
				fprintf(outf, "%s\n", purl);
				fprintf(outf, "%d\n", pdep);
				fprintf(outf, "%d\n", plen);
				fprintf(outf, "%s", html);
				
	 			result = 0;
			} else {
				system(remv);
			}
			
			if (fclose(outf) == EOF) {
				printf("ERROR: Error closing file %s\n", path);
				result = 1;
			}
		} else {
			printf("ERROR: Failed to make file %s\n", path);
		}
	} else {
		printf("ERROR: Failed to make directory \"%s\"\n", dirnm);
	}
	
	return result;
}

/* Loads the numbered filename <id> in directory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm) {
	//put later: webpage_t *result = webpage_new(url, depth, html);
	webpage_t *result = NULL;
	FILE *in_f;
	int trash, i, j, k, nlen = strlen(dirnm) + 16, u = 0, d = 0, s = 48, uc = 0, sc = 0, sd[10], depth, html_length;
	char ch, outn[5], path[nlen], url[MAXLEN];
	bool stop = false;
	
	sprintf(outn, "%d", id);
	strcpy(path, "./");
	strcat(path, dirnm);
	strcat(path, "/");
	strcat(path, outn);
	
	//check if directory and file exist and file is readable
	if (access(dirnm, F_OK) == 0) {
		if (access(path, R_OK) != 0) {
			printf("ERROR: File not found or is unreadable\n");
			//result = NULL;
		} else {
			if ((in_f = fopen(path, "r")) != NULL) {
				while (u != 10 && u != EOF && uc < MAXLEN) {
					u = fgetc(in_f);
					url[uc] = (char)u;
					uc++;
				}
				
				if (u == EOF || uc >= MAXLEN) {
					printf("ERROR: Save file ended prematurely and/or URL exceeds 100 characters\n");
					//result = NULL;
				} else {
					d = getc(in_f);
					
					if (d == EOF || d < 48 || d > 57) {
						printf("ERROR: Save file ended prematurely and/or invalid depth value\n");
						//result = NULL;
					} else {
						depth = d - 48;
						
						trash = getc(in_f);
						if (trash == EOF) {
							printf("ERROR: Save file ended prematurely\n");
							//result = NULL;
						} else {
							if (trash != 10) {
								printf("ERROR: Invalid depth read from save file\n");
								//result = NULL;
							} else {
								while (s != EOF && s > 47 && s < 58 && sc < 10) {
									printf("in while loop: s = %d\n", s);
									s = fgetc(in_f);
									sd[sc] = s - 48;
									
									for (i = 0; i < sc - 1; i++) {
										printf("in i for loop: s = %d\n", s);
										sd[i] = sd[i] * 10;
									}

									sc++;
									printf("finished this iter. of while loop: s = %d\n", s);
								}

								for (j = 0; j < sc - 1; j++) {
									printf("in j for loop: sd[%d] = %d\n", j, sd[j]);
									html_length += sd[j];
								}
								
								if (s != 10) {
									printf("ERROR: Save file ended prematurely and/or html length exceeds 1 billion or was formatted incorrectly. html_length = %d, j = %d, sc = %d, sd[0] = %d, s = %d, trash = %d, depth = %d, d = %d, u = %d, uc = %d\n", html_length, j, sc, sd[0], s, trash, depth, d, u, uc);
									//result = NULL;
								} else {
									char html[html_length];
									printf("html_length = %d\n", html_length);
									for (k = 0; k < (html_length - 1) && stop == false; k++) {
										ch = fgetc(in_f);
										if (ch == EOF) {
											stop = true;
											printf("EOF at k = %d\n", k);
										}
										html[k] = (char)ch;
										printf("%c", (char)ch);
									}
									
									if (stop == false) {
										result = webpage_new(url, depth, html);
									} else {
										printf("ERROR: Save file ended prematurely and/or actual html length is shorter than claimed: k = %d, html[%d] = %c\n", k, k - 1, html[k - 1]);
										//result = NULL;
									}
								}
							}
						}
					}
				}
			} else {
				printf("ERROR: Failed to open file\n");
				//result = NULL;
			}
		}
	} else {
		printf("ERROR: Directory not found\n");
		//result = NULL;
	}
	
	return result;
}
