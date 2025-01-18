#ifndef SHELLDON_H
#define SHELLDON_H

struct shelldon
{
    struct variable *var;
};

// déclaratin de la struct globale
extern struct shelldon shelldon;

void shelldon_init(void);
void shelldon_free(void);

#endif /* ! SHELLDON_H */
