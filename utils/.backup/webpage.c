/* 
 * webpage - utility functions for downloading, saving, and loading web pages.
 *
 * Ira Ray Jenkins - April 2014
 * 
 * Updated by David Kotz - April 2016, 2017
 * Updated by Xia Zhou - July 2016
 *
 */

/* students shouldn't take advantage of the gnu extensions, 
 * but parsing html without them is a pain.
 */
#define _GNU_SOURCE	      // strncasecmp, strdup

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curl/curl.h>
#include <webpage.h>

/* Private Section */

/* webpage_t: structure to represent a web page, and its contents.
 * The innards should not be visible to users of the webpage module.
 */
typedef struct webpage {
  char *url;                               // url of the page
  char *html;                              // html code of the page
  size_t html_len;                         // length of html code
  int depth;                               // depth of crawl
} webpage_t;

struct URL {
    char* scheme;             // http://
    char* user;		      // username:password@
    char* host;		      // www.example.com
    char* path;		      // /path/to/file.html
    char* query;	      // ?name1=val1&name2=val2
    char* fragment;	      // #top
};

/* Private function prototypes */
static char *RemoveDotSegments(char *input);
static void RemoveWhitespace(char* str);
static int ParseURL(char* str, struct URL* url);
static char *FixupRelativeURL(char *base, char *rel, size_t len);
static void *checkp(void *p, char *message);

/* Private global variables */
#define NUM_EXTS (3)			 // size of EXTS array
static const char* EXTS[NUM_EXTS] = {	 // valid extensions
  "html",
  "jsp",
  "php"
};

/* getter methods */
int   webpage_getDepth(const webpage_t *page) { return page ? page->depth : 0;    }
int   webpage_getHTMLlen(const webpage_t *page) { return page ? page->html_len : 0; }
char *webpage_getHTML(const webpage_t *page)  { return page ? page->html  : NULL; }
char *webpage_getURL(const webpage_t *page)   { return page ? page->url   : NULL; }


webpage_t *webpage_new(char *url, const int depth, char *html) {
  if (url == NULL || depth < 0) {
    return NULL;
  }
  webpage_t *page = checkp(malloc(sizeof(webpage_t)), "webpage_t");

  page->url = checkp(malloc(strlen(url)+1), "page->url");
  strcpy(page->url, url);
  page->depth = depth;
  page->html = html;
  page->html_len = html ? strlen(html) : 0;
  return page;
}


void webpage_delete(void *data)
{
  webpage_t *page = data;
  if (page != NULL) {
    if (page->url) free(page->url);
    if (page->html) free(page->html);
    free(page);
  }
}

/**************** webpage_getNextWord ****************/
/*
 * webpage_getNextWord - returns the next word from doc[pos] into word
 * See "webpage.h" for full documentation.
 * Code is courtesy of Ray Jenkins and/or Charles Palmer, 
 *   cleaned by David Kotz in April 2016, 2017.
 *
 * Pseudocode:
 *     1. skip any leading non-alphabetic characters
 *     2. if we find a tag, i.e., <...tag...>, skip that tag
 *     3. save beginning of the word
 *     4. find the end, i.e., first non-alphabetic character
 *     5. create a new word buffer
 *     6. copy the word into the new buffer
 *     7. return first position past end of word
 * 
 */
int webpage_getNextWord(webpage_t *page, int pos, char **word) {
  // make sure we have something to search, and a place for the result
  if (page == NULL || page->html == NULL || word == NULL) {
    return -1;
  }

  const char *doc = page->html;		   // the html document
  const char *beg;                         // beginning of word
  const char *end;                         // end of word

  // consume any non-alphabetic characters
  while (doc[pos] != '\0' && !isalpha(doc[pos])) {
    // if we find a tag, i.e., <...tag...>, skip it
    if (doc[pos] == '<') {
      end = strchr(&doc[pos], '>');    // find the close
			/* ST: end is a pointer -- cant compare to a char */
      if(*end == '\0' || *(++end) == '\0') { // ran out of html
				return -1;
      }
      pos = end - doc;	      // skip over the <...tag...>
    } else {
      pos++;		      // just move forward
    }
  }

  // ran out of html
  if (doc[pos] == '\0') {
    *word = NULL;
    return -1;
  }

  // pos is at the first character of a word
  beg = &(doc[pos]);

  // consume word
  while (doc[pos] != '\0' && isalpha(doc[pos])) {
    pos++;
  }
  // at this point, doc[pos] is the first character *after* the word.

  // pos refers to the last character of the word
  end = &(doc[pos-1]);

  // 'beg' points to first character of the word
  // 'end' points to last character of the word
  int wordlen = end - beg + 1;

  // allocate space for length of new word + '\0'
  *word = calloc(wordlen + 1, sizeof(char));
  if (*word == NULL) {	      // out of memory!
    return -1;
  }

  // copy the new word
  strncpy(*word, beg, wordlen);

  return pos;
}

/**************** webpage_getNextURL ****************/
/*
 * get the next url from html[pos] into result
 *
 * Assumptions:
 *     1. page is valid, contains html and base_url
 *     2. pos = 0 on initial call
 *     3. result is NULL and will be allocated internally
 *
 * Pseudocode:
 *     1. check arguments
 *     2. if pos = 0: remove whitespace from html
 *     3. find hyperlink starting tags "<a" or "<A"
 *     4. find next href attribute "href="
 *     5. find next end tag ">"
 *     6. check that href comes before end tag
 *     7. deal with quoted and unquoted urls
 *     8. determine if url is absolute
 *     9. fixup relative links
 *    10. create new character buffer for result and return it
 */
int webpage_getNextURL(webpage_t *page, int pos, char **result) {
  // make sure we have text and base url
  if (page == NULL || page->html == NULL || page->url == NULL) {
    return -1;
  }

  char *html = page->html;                 // the html document
  char *base_url = page->url;		   // the base URL for this html
  int bad_link;                            // is this link ill formatted?
  int relative;                            // is this link relative?
  char delim;                              // url delimiter: ' ', ''', '"'
  char *lnk;                               // hyperlink tags
  char *href;                              // href in a tag
  char *end;                               // end of hyperlink tag or url
  char *ptr;                               // absolute vs. relative
  char *hash;                              // hash mark character

  // condense html, makes parsing easier
  if (pos == 0) {
    RemoveWhitespace(html);
  }

  // parse for hyperlinks
  do {
    relative = 0;                        // assume absolute link
    bad_link = 0;                        // assume valid link

    // find tag "<a" or "<A""
    lnk = strcasestr(&html[pos], "<a");

    // no more links on this page
    if (!lnk) { result = NULL; return -1; }


    // find next href after hyperlink tag
    href = strcasestr(lnk, "href=");

    // no more links on this page
    if (!href) { result = NULL; return -1; }

    // find end of hyperlink tag
    end = strchr(lnk, '>');

    // if the href we have is outside the current tag, continue
    if (end && (end < href)) {
      bad_link = 1; pos+=2; continue;
    }

    // move href to beginning of url
    href+=5;

    // something went wrong, just continue
    if (!href) { bad_link=1; pos+=2; continue; }

    // is the url quoted?
    if (*href == '\'' || *href == '"') {  // yes, href="url" or href='url'
      delim = *(href++);               // remember delimiter
      end = strchr(href, delim);       // find next of same delimiter
    } else {			       // no, href=url
      end = strchr(href, '>');         // hope: <a ... href=url>
      // since we've stripped whitespace
      // this could mangle things like:
      // <a ... href=url name=val>
    }

    // if there is a # before the end of the url, exclude the #fragment
    hash = strchr(href, '#');
    if (hash && hash < end) {
      end = hash;
    }

    // if we don't know where to end the url, continue
    if (!end) {
      bad_link = 1; pos += 2; continue;
    }

    // have a link now
    if (*href == '#') {                   // internal reference
      bad_link = 1; pos += 2; continue;
    }

    // is the url absolute, i.e, ':' must precede any '/', '?', or '#'
    ptr = strpbrk(href, ":/?#");
    if (!ptr || *ptr != ':') { 
      relative = 1; 
    } else if (strncasecmp(href, "http", 4)) { // absolute, but not http(s)
      bad_link = 1; pos += 2; continue;
    }
  } while (bad_link);                       // keep parsing

  // have a good link now
  if (relative) {                           // need to fixup relative links
    *result = FixupRelativeURL(base_url, href, end - href);
    if (!*result) { return -2; }
  }
	else {
    // create new buffer
    *result = calloc(end-href+1, sizeof(char));
    if (!*result) { return -2; }          // check memory
    // copy over absolute url
    strncpy(*result, href, end - href);
  }
  // return position at the end of the url
  return end - html;
}

/******************** NormalizeURL *******************************/
/* Normalize the url according to RFC 3986 chapter 3
 *
 * Assumptions:
 *     1. url has been allocated
 *     2. url is an absolute url
 *
 * Pseudocode:
 *     1. check arguments
 *     2. try to parse url
 *     3. check any file extensions
 *     4. clear the url
 *     5. remove dot segments and lowercase scheme and host
 *     6. put the url back together
 */
bool NormalizeURL(char *url) {
  bool status = true;
  int valid_ext;
  struct URL tmp;                          // url struct for parsing
  char *dot;
  char *slash;

  // initialize struct
  tmp.scheme = NULL;
  tmp.user = NULL;
  tmp.host = NULL;
  tmp.path = NULL;
  tmp.query = NULL;
  tmp.fragment = NULL;

  // test url
  if (!url) { return 0; }

  // try to parse the url
  if (!ParseURL(url, &tmp)) {
    status = false;
    goto cleanup;                        // sorry Dijkstra
  }

  // check file extension
  if (tmp.path) {                           // have a path
    dot = strrchr(tmp.path, '.');
    slash = strrchr(tmp.path, '/');

    if (dot && slash && dot > slash) {    // /path/to/file.ext
      dot++;                           // consume '.'

      if (strlen(dot) > 0) {
	// check all valid extensions
	valid_ext = 0;
	for (int i = 0; i < NUM_EXTS; i++) {
	  if (!strncasecmp(dot, EXTS[i], strlen(EXTS[i]))) {
	    valid_ext = 1; break;
	  }
	}

	// bad extension
	if (!valid_ext) {
	  status = false;
	  goto cleanup;            // sorry Dijkstra
	}
      }
    }
  }

  // clear url
  memset(url, '\0', strlen(url)-1);

  // put normalized url back together
  if (tmp.scheme) {                         // scheme
    strcat(url, tmp.scheme);
  }

  if (tmp.user) {                           // user
    strcat(url, tmp.user);
  }

  if (tmp.host) {                           // host
    strcat(url, tmp.host);
  }

  if (tmp.path) {                           // path
    // remove . and .. segments
    char *p = RemoveDotSegments(tmp.path);

    if (p) {
      strcat(url, p);

      free(p);
    }
  }

  if (tmp.query) {                          // query
    strcat(url, tmp.query);
  }

  if (tmp.fragment) {                       // fragment
    strcat(url, tmp.fragment);
  }

#ifdef REMOVE_SLASH
  // Remove trailing slash [DFK 2017].
  // This code allows crawler to realize that
  //    http://www.cs.dartmouth.edu == http://www.cs.dartmouth.edu/
  // but doing so actually prevents the crawler from following the 
  // server's implicit redirect to http://www.cs.dartmouth.edu/index.html
  // So, I've decided not to include it.
  if (*url != '\0') {
    char *last = url + strlen(url) - 1;
    if (*last == '/'){
      *last = '\0';
    }
  }
#endif // REMOVE_SLASH

 cleanup:                                     // cleanup memory
  if (tmp.scheme) { free(tmp.scheme); }
  if (tmp.user) { free(tmp.user); }
  if (tmp.host) { free(tmp.host); }
  if (tmp.path) { free(tmp.path); }
  if (tmp.query) { free(tmp.query); }
  if (tmp.fragment) { free(tmp.fragment); }

  return status;
}

/***********************************************************************
 * IsInternalURL - see webpage.h for interface description.
 */
bool IsInternalURL(char *url) {
  if (NormalizeURL(url)) {
    if (strncmp(url, INTERNAL_URL_PREFIX, strlen(INTERNAL_URL_PREFIX)) == 0) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


/***********************************************************************
 * INTERNAL FUNCTIONS
 ***********************************************************************/

/***********************************************************************
 * ParseURL - attempts to parse str into a URL struct
 * @str: absolute url to parse
 * @url: struct containing parts of a url
 *
 * Expects str to be an absolute url. Returns 0 if str cannot be
 * successfully parsed; otherwise, returns 1.
 *
 * From RFC 3986 chapter 3:
 *
 * The following are two example URIs and their component parts:
 *
 *       foo://example.com:8042/over/there?name=ferret#nose
 *       \_/   \______________/\_________/ \_________/ \__/
 *        |           |            |            |        |
 *     scheme     authority       path        query   fragment
 *        |   _____________________|__
 *       / \ /                        \
 *       urn:example:animal:ferret:nose
 *
 * Should have no use outside of this file, thus declared static.
 */
static int ParseURL(char* str, struct URL* url) {
  int host_len;                            // length of host segment
  char *scheme_end;                        // scheme end point, : or :/ or ://
  char *user_end;                          // end of user info, @
  char *host_beg;                          // beginning of host, : or @
  char *host_end;                          // end of host, scheme:host/
  char *host_e;                            // end of host, / or end of url
  char *path_end;                          // end of path, ? or # or end of url
  char *query_beg;                         // end of query, # or end of url
  char *frag_beg;                          // end of fragment, end of url

  // make sure we have a str and url struct
  if (!str || !url) {
    return 0;
  }

  // make sure absolute url, i.e., ':' must preceede any '/', '?', or '#'
  scheme_end = strpbrk(str, ":/?#");
  if (!scheme_end || *scheme_end != ':') {
    return 0;
  }

  scheme_end++;                            // consume ':'

  // do we have scheme:<path> or scheme:<host><path>
  if (!strncmp(scheme_end, "//", 2)) {     // have host
    scheme_end += 2;                    // consume "//"
  }

  // allocate url scheme
  url->scheme = calloc(scheme_end - str + 1, sizeof(char));
  if (!url->scheme) {
    return 0;
  }

  // lowercase it
  for (char *ptr = str; ptr < scheme_end; ptr++) {
    url->scheme[ptr - str] = tolower(*ptr);
  }


  // get user information, anything between scheme and first '@'
  user_end = strpbrk(scheme_end, "@/");
  if (user_end && *user_end == '/') {       // no user info
    user_end = NULL;
  }

  if (user_end) {                           // have user info
    user_end++;                          // consume '@'

    // allocate user
    url->user = calloc(user_end - scheme_end + 1, sizeof(char));
    if (!url->user) {
      return 0;
    }

    // copy user info
    strncpy(url->user, scheme_end, user_end - scheme_end);
  }

  // get host information
  host_end = strchr(scheme_end, '/');

  if (!host_end && !user_end) {             // scheme:host
    host_len = strlen(str) - (scheme_end - str);
    host_beg = scheme_end;
    host_e = &str[strlen(str)];
  } else if (!host_end && user_end) {       // scheme:user@host
    host_len = strlen(str) - (user_end - str);
    host_beg = user_end;
    host_e = &str[strlen(str)];
  } else if (host_end && !user_end) {       // scheme:host/path...
    host_len = host_end - scheme_end + 1;
    host_beg = scheme_end;
    host_e = host_end;
  } else {                                 // scheme:user@host/path...
    host_len = host_end - user_end + 1;
    host_beg = user_end;
    host_e = host_end;
  }

  // allocate host
  url->host = calloc(host_len + 1 , sizeof(char));
  if (!url->host) {
    return 0;
  }

  // lowercase it
  for (char *ptr = host_beg; ptr < host_e; ptr++) {
    url->host[ptr - host_beg] = tolower(*ptr);
  }

  // get path part, between host and query and/or fragment
  path_end = strpbrk(scheme_end, "?#");

  if (path_end) {                           // .../path? or .../path#
    url->path = calloc(path_end - host_e + 1, sizeof(char));
    if (!url->path) {
      return 0;
    }

    strncpy(url->path, host_e, path_end - host_e);
  } else {                                 // .../path
    url->path = calloc(&str[strlen(str)] - host_e + 1, sizeof(char));
    if (!url->path) {
      return 0;
    }

    strcpy(url->path, host_e);
  }

  // get fragment, anything after first '#'
  frag_beg = strchr(scheme_end, '#');

  if (frag_beg) {                           // have fragment
    url->fragment = calloc(&str[strlen(str)] - frag_beg + 1, sizeof(char));
    if (!url->fragment) {
      return 0;
    }

    strcpy(url->fragment, frag_beg);
  }

  // get query, anything after first '?' before any '#'
  query_beg = strchr(scheme_end, '?');

  if (query_beg && !frag_beg) {             // ...?name=value
    url->query = calloc(&str[strlen(str)] - query_beg + 1, sizeof(char));
    if (!url->query) {
      return 0;
    }

    strcpy(url->query, query_beg);
  } else if (query_beg && frag_beg && query_beg < frag_beg) { // ...?name=value#top
    url->query = calloc(frag_beg - query_beg + 1, sizeof(char));
    if (!url->query) {
      return 0;
    }

    strncpy(url->query, query_beg, frag_beg - query_beg);
  }

  return 1;                                // if we got this far, good
}

/* ***************************************************************** */
/*
 * RemoveDotSegments - removes . and .. segments from url paths
 * @input: the character buffer to cleanse
 *
 * Returns a newly allocated character buffer with . and .. segments removed
 * according to the algorithm in RFC 3986 section 5.2.4 "Remove Dot Segments".
 * See: http://www.ietf.org/rfc/rfc1738.txt
 *
 * Should have no use outside of this file, thus declared static.
 *
 * Implementation adapted for CS50 TSE Crawler from the cURL library:
 *
 * Copyright (c) 1996 - 2014, Daniel Stenberg, <daniel@haxx.se>.
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization of the copyright holder.
 */
static char *RemoveDotSegments(char *input)
{
  size_t in_len;                           // input length
  size_t copy_len;                         // copy length
  char *copy;                              // copy of input that is modified
  char *copyptr;                           // handle for free'ing copy'
  char *out;                               // output buffer
  char *outptr;                            // pointer to current write point

  if (!input || strlen(input) < 1)
    return NULL;

  // save lengths
  in_len = copy_len = strlen(input);

  // create output buffer
  out = outptr = calloc(in_len + 1, sizeof(char));
  if (!out)
    return NULL; /* out of memory */

  // copy the input buffer
  copy = copyptr = strdup(input);
  if (!copy) {
    free(out);
    return NULL;
  }

  // 2.  While the input buffer is not empty, loop as follows:
  do {
    // A. If the input buffer begins with a prefix of "../" or "./",
    //    then remove that prefix from the input buffer; otherwise,
    if (!strncmp("./", copy, 2)) {
      copy += 2;
      copy_len -= 2;
    }
    else if (!strncmp("../", copy, 3)) {
      copy += 3;
      copy_len -= 3;
    }

    // B. if the input buffer begins with a prefix of "/./" or "/.",
    //    where "." is a complete path segment, then replace that
    //    prefix with "/" in the input buffer; otherwise,
    else if (!strncmp("/./", copy, 3)) {
      copy += 2;
      copy_len -= 2;
    }
    else if (!strcmp("/.", copy)) {
      copy[1] = '/';
      copy++;
      copy_len -= 1;
    }

    // C. if the input buffer begins with a prefix of "/../" or "/..",
    //    where ".." is a complete path segment, then replace that
    //    prefix with "/" in the input buffer and remove the last
    //    segment and its preceding "/" (if any) from the output
    //    buffer; otherwise,
    else if (!strncmp("/../", copy, 4)) {
      copy += 3;
      copy_len -= 3;

      // remove the last segment
      while (outptr > out) {
	outptr--;
	if (*outptr == '/')
	  break;
      }
      *outptr = 0;
    }
    else if (!strcmp("/..", copy)) {
      copy[2] = '/';
      copy += 2;
      copy_len -= 2;

      // remove the last segment
      while (outptr > out) {
	outptr--;
	if (*outptr == '/')
	  break;
      }
      *outptr = 0;
    }

    // D. if the input buffer consists only of "." or "..", then remove
    //    that from the input buffer; otherwise, */
    else if (!strcmp(".", copy) || !strcmp("..", copy)) {
      *copy = 0;
    }

    // E. move the first path segment in the input buffer to the end of
    //    the output buffer, including the initial "/" character (if
    //    any) and any subsequent characters up to, but not including,
    //    the next "/" character or the end of the input buffer. */
    else {
      do {
	*outptr++ = *copy++;
	copy_len--;
      } while (*copy && (*copy != '/'));
      *outptr = '\0';
    }
  } while (*copy != '\0');                          // keep going

  // cleanup copy
  free(copyptr);

  return out;
}

/* ***************************************************************** */
/*
 * FixupRelativeURL - resolves a relative url to an absolute url
 * @base: base url to resolve from
 * @rel: relative url to resolve
 * @len: length of the relative url
 *
 * Returns a newly allocated character buffer that represents the
 * absolute url from the base and relative urls. Returns NULL if
 * an absolute url cannot be established.
 *
 * This is a quick attempt at RFC 3986 section 5.2.
 *
 * Should have no use outside of this file, thus declared static.
 */

static char *FixupRelativeURL(char *base, char *rel, size_t len)
{
  char *abs_url;                           // absolute url to build
  char *slash;                             // right-most '/' in a path
  struct URL tmp;                          // parsed url

  // we need a base url to work with
  if (!base) {
    return NULL;
  }

  // allocate new absolute url
  abs_url = calloc(strlen(base) + len + 2, sizeof(char));
  if (!abs_url) {
    return NULL;
  }

  // initialize struct
  tmp.scheme = NULL;
  tmp.user = NULL;
  tmp.host = NULL;
  tmp.path = NULL;
  tmp.query = NULL;
  tmp.fragment = NULL;

  // attempt to parse the base url
  if (!ParseURL(base, &tmp)) {
    free(abs_url);                       // cleanup the absolute url

    abs_url = NULL;                      // going to return NULL
    goto cleanup;                        // sorry Dijkstra
  }

  // put absolute url back together
  if (tmp.scheme) {                         // scheme
    strcat(abs_url, tmp.scheme);
  }

  if (tmp.user) {                           // user
    strcat(abs_url, tmp.user);
  }

  if (tmp.host) {                           // host
    strcat(abs_url, tmp.host);	      // always ends in slash
  }

  // do we have a relative url?
  if (rel) { // yes, add it to the abs_url
    // is the relative URL relative to domain root, or relative to base?
    if (rel[0] == '/') {
      // relative to domain root
      strncat(abs_url, rel, len);	     // add relative url
    } else {
      // relative to base_url
      // add the base path up to the right-most '/'
      if (tmp.path
	  && (slash = strrchr(tmp.path, '/'))
	  && (slash != tmp.path)) {
	strncat(abs_url, tmp.path, slash - tmp.path);
      }
      strcat(abs_url, "/");            // separate base and relative path
      strncat(abs_url, rel, len);	     // add relative url
    }
  } else { // no relative url; finish the abs_url
    // DFK: not sure this case occurs, or if it does, what action to take.
    // add the base path up to the right-most '/'
    if (tmp.path
	&& (slash = strrchr(tmp.path, '/'))
	&& (slash != tmp.path)) {
      strncat(abs_url, tmp.path, slash - tmp.path);
    }
  }

  // we can ignore the base query and fragment, they shouldn't apply

 cleanup:                                     // cleanup memory
  if (tmp.scheme)   { free(tmp.scheme); }
  if (tmp.user)     { free(tmp.user); }
  if (tmp.host)     { free(tmp.host); }
  if (tmp.path)     { free(tmp.path); }
  if (tmp.query)    { free(tmp.query); }
  if (tmp.fragment) { free(tmp.fragment); }

  return abs_url;
}


/* ***************************************************************** */
/*
 * RemoveWhitespace - removes whitespace from str
 * @str: char buffer to modify
 *
 * Eliminates whitespace by shifting and condensing all the non-whitespace
 * toward the beginning of the buffer. This does not alter the size of the
 * buffer.
 *
 * Should have no use outside of this file, thus declared static.
 */
static void RemoveWhitespace(char* str)
{
  char *prev;                              // previous whitespace
  char *cur;                               // current non-whitespace

  // start at beginning of str
  cur = prev = str;

  do {
    while (isspace(*cur)) cur++;          // consume any whitespace
  } while ((*prev++ = *cur++));            // condense to front of str
}

/**************** checkp ****************/
/* if pointer p is NULL, print error message and die,
 * otherwise, return p unchanged.
 */
static void *
checkp(void *p, char *message)
{
  if (p == NULL) {
    fprintf(stderr, "CHECKP: %s\n", message);
    exit (99);
  }
  return p;
}


/* Private Functions */

/* WriteMemoryCallback - curl callback for writing retrieved data
 *
 * For implementation details see:
 * http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTWRITEFUNCTION
 *
 * Should have no use outside of this file, thus declared static.
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  webpage_t *page = (webpage_t*) userp;

  page->html = realloc(page->html, page->html_len + realsize + 1);
  if (page->html == NULL) {
    return 0;
  }
  memcpy(&(page->html[page->html_len]), contents, realsize);
  page->html_len += realsize;
  page->html[page->html_len] = 0;
  return realsize;
}


/* ************* webpage_fetch ******************** */
/* see webpage.h for usage documentation.
 *
 * Pseudocode:
 *     1. check for valid page pointer
 *     2. allocate buffer to page->html, set page->html_len to 0
 *     3. setup curl
 *     4. curl the page->url
 *     5. check return status
 *     6. cleanup
 */
bool webpage_fetch(webpage_t *page) {
  const int MAX_TRY = 3;               // maximum attempts to fetch
  static char errbuf[CURL_ERROR_SIZE]; // buffer for error messages
  int tries = 0;		       // number of attempts at curl
  bool status = true;		       // return value
  CURL* curl_handle;		       // curl handle
  CURLcode res;		               // curl response code

  // check page
  if (page == NULL) { return false; }

  // allocate space for the html, curl will realloc as needed
  page->html = calloc(1, sizeof(char));
  page->html_len = 0;

  // init curl session
  curl_handle = curl_easy_init();

  // specify url
  curl_easy_setopt(curl_handle, CURLOPT_URL, page->url);

  // send all data to this function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  // pass page struct to callback function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)page);

  // add a user agent just in case servers need it
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  // make 404+ an error
  curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);

  // save error messages
  curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, &errbuf);

  // get the page; repeat MAX_TRY times
  do {
    res = curl_easy_perform(curl_handle);
#ifndef NOSLEEP // CS50 students: please don't turn off the sleep!
    sleep(1);   // sleep one second between fetches, to lighten load on server
#endif
  } while (res != CURLE_OK && ++tries < MAX_TRY);

  // check response code
  if (res != CURLE_OK) {
    // we're going to return the curl error message on failure
    free(page->html);
    page->html = calloc(strlen(errbuf) + 1, sizeof(char));
    page->html_len = strlen(errbuf);
    strcpy(page->html, errbuf);

    status = false;                          // signal failure
  }

  // cleanup curl stuff
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return status;
}


