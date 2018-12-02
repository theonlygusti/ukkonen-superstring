#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ahocorasick.h"

int g(struct ahocorasick * restrict ac_machine, int state, char text) {
  struct aho_trie_node* travasal_node = &(ac_machine->trie.root);
}

void print_aho_trie_node(struct aho_trie_node *node, int indent_level) {
  printf("%*s%c %u\n", indent_level * 4, "",  node->text, node->ref_count);
  if (node->child_count) {
    for (int i = 0; i < node->child_count; i++) {
      print_aho_trie_node(node->child_list[i], indent_level + 1);
    }
  }
}

void print_text_list(struct ahocorasick * restrict ac_machine) {
  struct aho_text_t *current_text = ac_machine->text_list_head;
  do {
    printf("%s\n", current_text->text);
    current_text = current_text->next;
  } while (current_text != ac_machine->text_list_tail);
}

char *ukkonen_superstring(int n, char **strings) {

  /**
   * We write g(s, a) = t if there is a goto transition from state s to state t for symbol a.
   * If there is no applicable goto transition in some state, then the machine uses a failure
   * transition. Except for the start state 0 (which has a goto transition for every symbol),
   * there is exactly one failure transition starting from each state. The failure transitions
   * that do not go to the start state 0 are shown by dashed arrows in Figure 2 while those
   * pointing to 0 are omitted. A failure transition is a so-called e-move which does not consume
   * any symbol from the string scanned. We write f(s) = t if there is a failure transition from
   * s to t.
   */

  /**
   * Given a set R = {x1, ..., xm} of strings, we first construct the AC machine for it.
   */

  struct ahocorasick ac_machine;
  int *id;
  id = (int *) calloc(n, sizeof(int));

  aho_init(&ac_machine);

  for (int i = 0; i < n; i++) {
    id[i] = aho_add_match_text(&ac_machine, strings[i], strlen(strings[i]));
  }

  aho_create_trie(&ac_machine);

  /**
   * Output depth d(s), list L(s), and link b(s) for each state s of the AC machine;
   * pointer B to the first state in the b-link chain; state F(i) representing
   * string xi for each xi with the exception that if xi is a substring of another
   * string in R then F(i) =0.
   */

  /*
  for (int i = 0; i < n; i++) {
    char *a = strings[i];
    int s = 0;
    for (int j = 0; i < k; i++) {
      s = g(s, a[j]);
      L(s) = L(s) . {j}
      if (j == k) {
        F(i) = s;
        E(s) = i;
        if (s is not a leaf of the AC machine) {
          F(i) = 0;
        }
      }
    }
  }

  queue = 0;
  d(0) = 0;
  B = 0;
  while (queue not empty) {
    let r be the next state in queue;
    queue = queue - r;
    for each s such that g(r, a) = s for some a {
      queue = queue . s;
      d(s) = d(r) + 1;
      b(s) = B;
      B = s;
      F(E(f(s))) = 0
    }
  }


  // alg 2

  let initially each P(s) be empty;
  for (int j = 0; j < n; j++) {
    if (F(j) != 0) {
      P(f(F(j))) = P(f(F(j))) . { j };
      FIRST(j) = LAST(j) = j;
    } else {
      forbidden(j) = true;
    }
  }

  s = b(B);

  while (s != 0) {
    if (P(s) is not empty) {
      // use direct mapping
      for (each j in L(s) such that forbidden(j) = false) {
        i = the first element of P(s);
        if (FIRST(i) == j) {
          if (P(s) has only one element) continue;
          else
            i = the second element of P(s)
        }
        H = H . {(x[i], x[j])}
        forbidden(j) = true;
        P(s) = P(s) - {i}
        FIRST(LAST(j)) = FIRST(i);
        LAST(FIRST(i)) = LAST(j);
      }
      P(f(s)) = P(f(s)) . P(s);
    }
    s = b(s);
  }
  */

  print_aho_trie_node(&(ac_machine.trie.root), 0);
  print_text_list(&ac_machine);

  aho_destroy(&ac_machine);
  free(id);

  return "whatnot";
}
