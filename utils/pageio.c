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
				fprintf(outf, "%d", plen);
				fprintf(outf, "%s\n\n", html);
				
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
	webpage_t *result;
	FILE *fp;
	char filename[MAXLEN], url[MAXLEN], *html, *p;
	int i, depth, html_length;
	
	sprintf(filename, "%s/%d", dirnm, id);
	if((fp = fopen(filename, "r")) == NULL)
		return NULL;
	fscanf(fp, "%s", url);
	fscanf(fp, "%d", &depth);
	fscanf(fp, "%d", &html_length);
	if((html = (char *)malloc(html_length)) == NULL)
		return NULL;
	for(p=html,i=0; i<html_length; i++,p++)
		*p = fgetc(fp);
	if((result=webpage_new(url,depth,html)) == NULL)
		return NULL;
	return result;
}
