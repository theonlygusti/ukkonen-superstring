#pragma once
/**
 * This is by morenice https://github.com/morenice/ahocorasick.git
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

struct aho_queue_node
{
    struct aho_queue_node *next, *prev;
    struct aho_trie_node *data;
};

struct aho_queue
{
    struct aho_queue_node *front;
    struct aho_queue_node *rear;
    unsigned int count;
};


void aho_queue_init(struct aho_queue * restrict que);
void aho_queue_destroy(struct aho_queue * restrict que);

bool aho_queue_enqueue(struct aho_queue * restrict que, struct aho_trie_node *node);
struct aho_queue_node* aho_queue_dequeue(struct aho_queue * restrict que);

/* inline */
bool aho_queue_empty(struct aho_queue * restrict que);

struct aho_text_t
{
    int id;
    char* text;
    int len;
    struct aho_text_t *prev, *next;
};

#define MAX_AHO_CHILD_NODE 256 /* Character 1 byte => 256 */

struct aho_trie_node
{
    unsigned char text;
    unsigned int ref_count;

    struct aho_trie_node* parent;
    struct aho_trie_node* child_list[MAX_AHO_CHILD_NODE];
    unsigned int child_count;

    bool text_end;
    struct aho_text_t* output_text; /* when text_end is true */

    struct aho_trie_node* failure_link;
    struct aho_trie_node* output_link;
};

struct aho_trie
{
    struct aho_trie_node root;
};

void aho_init_trie(struct aho_trie * restrict t);
void aho_destroy_trie(struct aho_trie * restrict t);

bool aho_add_trie_node(struct aho_trie * restrict t, struct aho_text_t * restrict text);
void aho_connect_link(struct aho_trie * restrict t);
void aho_clean_trie_node(struct aho_trie * restrict t);

struct aho_text_t* aho_find_trie_node(struct aho_trie_node** restrict start, const unsigned char text);

void aho_print_trie(struct aho_trie * restrict t);

/* TODO:
 * bool aho_del_trie_node(struct aho_trie* t, struct aho_text_t* text);
 */

struct aho_match_t
{
    int id;
    unsigned long long pos;
    int len;
};

struct ahocorasick
{
#define AHO_MAX_TEXT_ID INT_MAX
    int accumulate_text_id;
    struct aho_text_t* text_list_head;
    struct aho_text_t* text_list_tail;
    int text_list_len;

    struct aho_trie trie;

    void (*callback_match)(void* arg, struct aho_match_t*);
    void* callback_arg;
};

void aho_init(struct ahocorasick * restrict aho);
void aho_destroy(struct ahocorasick * restrict aho);

int aho_add_match_text(struct ahocorasick * restrict aho, const char* text, unsigned int len);
bool aho_del_match_text(struct ahocorasick * restrict aho, const int id);
void aho_clear_match_text(struct ahocorasick * restrict aho);

void aho_create_trie(struct ahocorasick * restrict aho);
void aho_clear_trie(struct ahocorasick * restrict aho);

unsigned int aho_findtext(struct ahocorasick * restrict aho, const char* data, unsigned long long data_len);

void aho_register_match_callback(struct ahocorasick * restrict aho,
        void (*callback_match)(void* arg, struct aho_match_t*),
        void *arg);

/* for debug */
void aho_print_match_text(struct ahocorasick * restrict aho);

void aho_queue_init(struct aho_queue * restrict que)
{
    memset(que, 0x00, sizeof(struct aho_queue));
}

void aho_queue_destroy(struct aho_queue * restrict que)
{
    struct aho_queue_node* que_node = NULL;
    while ((que_node = aho_queue_dequeue(que)) != NULL)
    {
        free(que_node);
    }
}

bool aho_queue_enqueue(struct aho_queue * restrict que, struct aho_trie_node* node)
{
    struct aho_queue_node* que_node;
    que_node = (struct aho_queue_node*) malloc(sizeof(struct aho_queue_node));

    if (!que_node)
    {
        /* free memory error!! */
        return false;
    }

    memset(que_node, 0x00, sizeof(struct aho_queue_node));
    que_node->data = node;

    if (que->count == 0)
    {
        que->rear = que_node;
        que->front = que_node;
        que->count++;
        return true;
    }

    que_node->prev = que->rear;
    que->rear->next = que_node;
    que->rear = que_node;
    que->count++;

    return true;
}

inline bool aho_queue_empty(struct aho_queue * restrict que)
{
    return (que->count == 0);
}

struct aho_queue_node* aho_queue_dequeue(struct aho_queue * restrict que)
{
    struct aho_queue_node* deque_node;
    struct aho_queue_node* after_last_node;
    if (aho_queue_empty(que) == true)
    {
        return NULL;
    }

    if (que->count == 1)
    {
        deque_node = que->rear;
        que->front = que->rear = NULL;
        que->count--;
        return deque_node;
    }

    deque_node = que->rear;

    after_last_node = que->rear->prev;
    after_last_node->next = NULL;
    que->rear = after_last_node;
    que->count--;
    return deque_node;
}

void __aho_trie_node_init(struct aho_trie_node * restrict node)
{
    memset(node, 0x00, sizeof(struct aho_trie_node));
    node->text_end = false;
    node->ref_count = 1;
}

void aho_init_trie(struct aho_trie * restrict t)
{
    memset(t, 0x00, sizeof(struct aho_trie));
    __aho_trie_node_init(&(t->root));
}

void aho_destroy_trie(struct aho_trie * restrict t)
{
    aho_clean_trie_node(t);
}

bool aho_add_trie_node(struct aho_trie * restrict t, struct aho_text_t * restrict text)
{
    struct aho_trie_node* travasal_node = &(t->root);

    for (int text_idx = 0; text_idx < text->len; text_idx++)
    {
        unsigned char node_text = text->text[text_idx];
        bool find_node = false;
        int child_idx = 0;

        if (travasal_node->child_count == 0)
        {
            /* insert first node to child_list */
            travasal_node->child_list[0] =
                         (struct aho_trie_node*) malloc(sizeof(struct aho_trie_node));
            travasal_node->child_count++;

            __aho_trie_node_init(travasal_node->child_list[0]);
            travasal_node->child_list[0]->text = node_text;
            travasal_node->child_list[0]->parent = travasal_node;

            travasal_node = travasal_node->child_list[0];
            continue;
        }

        if (travasal_node->child_count == MAX_AHO_CHILD_NODE)
        {
            return false;
        }

        for (child_idx=0; child_idx < travasal_node->child_count; child_idx++)
        {
            if (travasal_node->child_list[child_idx]->text == node_text )
            {
                find_node = true;
                break;
            }
        }

        if (find_node == true)
        {
            travasal_node->child_list[child_idx]->ref_count++;
            travasal_node = travasal_node->child_list[child_idx];
        }
        else
        {
            /* push_back to child_list */
            struct aho_trie_node* child_node = NULL;

            travasal_node->child_list[travasal_node->child_count] =
                         (struct aho_trie_node*) malloc(sizeof(struct aho_trie_node));

            child_node = travasal_node->child_list[travasal_node->child_count];
            travasal_node->child_count++;

            __aho_trie_node_init(child_node);
            child_node->text = node_text;
            child_node->parent = travasal_node;

            travasal_node = child_node;
        }
    }

    // connect output link
    if (travasal_node)
    {
        travasal_node->text_end = true;
        travasal_node->output_text = text;
    }
    return true;
}

bool __aho_connect_link(struct aho_trie_node* p, struct aho_trie_node* q)
{
    struct aho_trie_node *pf = NULL;
    int i = 0;

    /* is root node */
    if (p->parent == NULL)
    {
        q->failure_link = p;
        return true;
    }

    pf = p->failure_link;
    for (i=0; i < pf->child_count; i++)
    {
        /* check child node of failure link(p) */
        if (pf->child_list[i]->text == q->text )
        {
            /* connect failure link */
            q->failure_link = pf->child_list[i];

            /* connect output link */
            if (pf->child_list[i]->text_end)
            {
                q->output_link = pf->child_list[i];
            }
            else
            {
                q->output_link = pf->child_list[i]->output_link;
            }
            return true;
        }
    }
    return false;
}

void aho_connect_link(struct aho_trie * restrict t)
{
    struct aho_queue queue;
    aho_queue_init(&queue);
    aho_queue_enqueue(&queue, &(t->root));

    /* BFS access
     *  connect failure link and output link
     */
    while (true)
    {
        /* p :parent, q : child node */
        struct aho_queue_node *queue_node = NULL;
        struct aho_trie_node *p = NULL;
        struct aho_trie_node *q = NULL;
        int i = 0;

        queue_node = aho_queue_dequeue(&queue);
        if (queue_node == NULL)
        {
            break;
        }

        p = queue_node->data;
        free(queue_node);

        /* get child node list of p */
        for (i=0; i < p->child_count; i++)
        {
            struct aho_trie_node *pf = p;

            aho_queue_enqueue(&queue, p->child_list[i]);
            q = p->child_list[i];

            while (__aho_connect_link(pf, q) == false)
            {
                pf = pf->failure_link;
            }
        }
    }

    aho_queue_destroy(&queue);
}

void aho_clean_trie_node(struct aho_trie * restrict t)
{
    struct aho_queue queue;
    aho_queue_init(&queue);
    aho_queue_enqueue(&queue, &(t->root));

    /* BFS */
    while (true)
    {
        struct aho_queue_node *queue_node = NULL;
        struct aho_trie_node *remove_node = NULL;
        int i = 0;

        queue_node = aho_queue_dequeue(&queue);
        if (queue_node == NULL)
        {
            break;
        }

        remove_node = queue_node->data;
        free(queue_node);

        for (i=0; i < remove_node->child_count; i++)
        {
            aho_queue_enqueue(&queue, remove_node->child_list[i]);
        }

        /* is root node */
        if (remove_node->parent == NULL)
        {
            continue;
        }

        free(remove_node);
    }
}

bool __aho_find_trie_node(struct aho_trie_node** restrict start, const unsigned char text)
{
    struct aho_trie_node* search_node = NULL;
    int i = 0;

    search_node = *start;
    for (i = 0; i < search_node->child_count; i++)
    {
        if (search_node->child_list[i]->text == text)
        {
            /* find it! move to find child node! */
            *start = search_node->child_list[i];
            return true;
        }
    }

    /* not found */
    return false;
}

struct aho_text_t* aho_find_trie_node(struct aho_trie_node** restrict start, const unsigned char text)
{
    while (__aho_find_trie_node(start, text) == false)
    {
        /* not found!
         * when root node stop
         */
        if( (*start)->parent == NULL)
        {
            return NULL;
        }

        /* retry find. move failure link. */
        *start = (*start)->failure_link;
    }

    /* found node... */
    /* match case1: find text end! */
    if ((*start)->text_end)
    {
        return (*start)->output_text;
    }

    /* match case2: exist output_link */
    if ((*start)->output_link)
    {
        return (*start)->output_link->output_text;
    }

    /* keep going */
    return NULL;
}

void aho_print_trie(struct aho_trie * restrict t)
{
    struct aho_queue queue;
    aho_queue_init(&queue);
    aho_queue_enqueue(&queue, &(t->root));

    /* BFS */
    while (true)
    {
        struct aho_queue_node *queue_node = NULL;
        struct aho_trie_node *travasal_node = NULL;
        int i = 0;

        queue_node = aho_queue_dequeue(&queue);
        if (queue_node == NULL)
        {
            break;
        }

        travasal_node = queue_node->data;
        free(queue_node);

        for (i=0; i < travasal_node->child_count; i++)
        {
            aho_queue_enqueue(&queue, travasal_node->child_list[i]);
        }

        /* is root node */
        if(travasal_node->parent == NULL)
        {
            printf("root node %p\n", travasal_node);
            continue;
        }

        printf("%c (textend:%d) node %p ref %u (parent %p) failure_link(%p) output_link(%p)",
                travasal_node->text, travasal_node->text_end,
                travasal_node, travasal_node->ref_count,
                travasal_node->parent, travasal_node->failure_link,
                travasal_node->output_link);

        printf("\n");
    }

    aho_queue_destroy(&queue);
}

void aho_init(struct ahocorasick * restrict aho)
{
    memset(aho, 0x00, sizeof(struct ahocorasick));
}

void aho_destroy(struct ahocorasick * restrict aho)
{
    aho_clear_match_text(aho);
    aho_clear_trie(aho);
}

int aho_add_match_text(struct ahocorasick * restrict aho, const char* text, unsigned int len)
{
    struct aho_text_t* a_text = NULL;
    if (aho->accumulate_text_id == AHO_MAX_TEXT_ID)
    {
        return -1;
    }

    a_text = (struct aho_text_t*) malloc(sizeof(struct aho_text_t));
    if (!a_text)
        goto lack_free_mem;

    a_text->text = (char*) malloc(sizeof(char)*len);
    if (!a_text->text)
        goto lack_free_mem;

    a_text->id = aho->accumulate_text_id++;
    memcpy(a_text->text, text, len);
    a_text->len = len;
    a_text->prev = NULL;
    a_text->next = NULL;

    if (aho->text_list_head == NULL)
    {
        aho->text_list_head = a_text;
        aho->text_list_tail = a_text;
        aho->text_list_len++;
        return a_text->id;
    }

    aho->text_list_tail->next = a_text;
    a_text->prev = aho->text_list_tail;
    aho->text_list_tail = a_text;
    aho->text_list_len++;
    return a_text->id;

lack_free_mem:
    return -1;
}

bool aho_del_match_text(struct ahocorasick * restrict aho, const int id)
{
    struct aho_text_t* iter = NULL;
    for (iter = aho->text_list_head; iter != NULL; iter = iter->next)
    {
        /*if (iter->id > id)
        {
            return false;
        }
        */

        if (iter->id == id)
        {
            if (iter == aho->text_list_head)
            {
                aho->text_list_head = iter->next;
            }
            else if (iter == aho->text_list_tail)
            {
                aho->text_list_tail = iter->prev;
            }
            else
            {
                iter->prev->next = iter->next;
                iter->next->prev = iter->prev;
            }
            free(iter);
            aho->text_list_len--;
            return true;
        }
    }
    return false;
}

void aho_clear_match_text(struct ahocorasick * restrict aho)
{
    for(int i = 0; i < aho->accumulate_text_id; i++)
    {
        aho_del_match_text(aho, i);
    }

    // reset id
    aho->accumulate_text_id = 0;
}


void aho_create_trie(struct ahocorasick * restrict aho)
{
    struct aho_text_t* iter = NULL;
    aho_init_trie(&(aho->trie));

    for (iter = aho->text_list_head; iter != NULL; iter = iter->next)
    {
        aho_add_trie_node(&(aho->trie), iter);
    }

    aho_connect_link(&(aho->trie));

    /* debugging */
    //aho_print_trie(&(aho->trie));
}

void aho_clear_trie(struct ahocorasick * restrict aho)
{
    aho_destroy_trie(&aho->trie);
}

unsigned int aho_findtext(struct ahocorasick * restrict aho, const char* data, unsigned long long data_len)
{
    int i = 0;
    int match_count = 0;
    struct aho_trie_node* travasal_node = NULL;

    travasal_node = &(aho->trie.root);

    for (i = 0; i < data_len; i++)
    {
        struct aho_match_t match;
        struct aho_text_t* result;

        result = aho_find_trie_node(&travasal_node, data[i]);
        if (result == NULL)
        {
            continue;
        }

        match.id = result->id;
        match.len = result->len;

        match.pos = i - result->len + 1;
        if (result->len == 1)
        {
            match.pos = i;
        }

        match_count++;
        if (aho->callback_match)
        {
            aho->callback_match(aho->callback_arg, &match);
        }
    }

    return match_count;
}

inline void aho_register_match_callback(struct ahocorasick * restrict aho,
        void (*callback_match)(void* arg, struct aho_match_t*),
        void *arg)
{
    aho->callback_arg = arg;
    aho->callback_match = callback_match;
}

void aho_print_match_text(struct ahocorasick * restrict aho)
{
    struct aho_text_t* iter = NULL;
    for (iter = aho->text_list_head; iter != NULL; iter = iter->next)
    {
        printf("id:%d text:%s len:%d this:%p prev:%p next:%p\n",
                iter->id, iter->text, iter->len, iter, iter->prev, iter->next);
    }
}

