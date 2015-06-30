#ifndef _CRYPTO_H
#define _CRYPTO_H

/* opaque so we don't try to access inside it */
struct crypto;

struct crypto* init_pbc();
void clean_pbc(struct crypto *pbc);

#endif
