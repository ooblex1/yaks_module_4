/* query_test.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Fri Oct 30 08:35:52 2020 (-0400)
 * Version: 
 * 
 * Description: Test query via generate_query function
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void generate_query(const wordlist_t *wordlist, const wordlist_t *dictionary) {
  // some parameters that affect query generation
  const int max_words = 6;        // generate 1..max_words
  const float or_probability = 0.3;   // P(OR between two words)
  const float and_probability = 0.2;  // P(AND between two words)
  const float dict_probability = 0.2; // P(draw from dict instead of wordlist)

  int qwords = random() % max_words + 1; // number of words in query
  for (int qw = 0; qw < qwords; qw++) {
    // draw a word either dictionary or wordlist
    if ((random() % 100) < (dict_probability * 100)) {
      printf("%s ", dictionary->words[random() % dictionary->nwords]);
    } else {
      printf("%s ", wordlist->words[random() % wordlist->nwords]);
    }

    // last word?
    if (qw < qwords-1) {
      // which operator to print?
      int op = random() % 100;
      if (op < (and_probability * 100)) {
        printf("AND ");
      }
      else if (op < (and_probability * 100 + or_probability * 100)) {
        printf("OR ");
      }
    }
  }
  printf("\n");
}

int main(int argc,char *argv[]) {
    generate_query(argv[0],argv[1]);
    return 0;
}