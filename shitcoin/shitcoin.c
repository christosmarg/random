#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sha256.h>

#define HASH_LEN 64
#define PENDING_MAX 128

struct data {
	char *saddr;
	char *raddr;
	long amount;
};

struct block {
	struct data *data;
	char hash[HASH_LEN + 1];
	char prevhash[HASH_LEN + 1];
	char tstmp[20];
	int nonce;
};

struct blockchain {
	struct block **blocks;
	struct block *pending[PENDING_MAX];
	size_t nblocks;
	size_t npending;
	int difficulty;
	int reward;
};

static void transaction(const char *, const char *, long);
static struct block *newblock(const char *, const char *, long, const char *);
static char *calchash(struct block *);
static void initchain(void);
static void minepending(const char *);
static void mineblock(struct block *);
static int validchain(void);
static struct block *lastblock(void);
static long balance(const char *);
static void printchain(void);
static void cleanchain(void);
static void *emalloc(size_t);

static struct blockchain *chain;

static void
transaction(const char *from, const char *to, long amount)
{
	if (chain->npending < PENDING_MAX)
		chain->pending[chain->npending++] =
		    newblock(from, to, amount, NULL);
	else
		warnx("transaction array is full");
}

static struct block *
newblock(const char *saddr, const char *raddr, long amount, const char *prevhash)
{
	struct block *b;
	struct tm *tm;
	time_t rtime;

	b = emalloc(sizeof(struct block));
	b->data = emalloc(sizeof(struct data));
	b->data->saddr = strdup(saddr);
	b->data->raddr = strdup(raddr);
	b->data->amount = amount;
	time(&rtime);
	tm = localtime(&rtime);
	strftime(b->tstmp, sizeof(b->tstmp), "%F %T", tm);
	strcpy(b->prevhash, prevhash == NULL ? "" : prevhash);
	strcpy(b->hash, calchash(b));
	b->nonce = 0;

	return b;
}

static char *
calchash(struct block *b)
{
	SHA256_CTX sha256;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	char buf[HASH_LEN + 19 + strlen(b->data->saddr) + strlen(b->data->raddr) + 10 + 10 + 1];
	char *res;
	int i = 0;

	res = emalloc(HASH_LEN + 1);
	sprintf(buf, "%s%s%s%s%ld%d",
	    b->prevhash, b->tstmp,
	    b->data->saddr, b->data->raddr, b->data->amount, b->nonce);

	SHA256_Init(&sha256);
	SHA256_Update(&sha256, buf, strlen(buf));
	SHA256_Final(hash, &sha256);
	for (; i < SHA256_DIGEST_LENGTH; i++)
		sprintf(&res[i << 1], "%02x", hash[i]);
	res[HASH_LEN] = '\0';

	return res;
}

static void
initchain(void)
{
	chain = emalloc(sizeof(struct blockchain));
	chain->blocks = emalloc(sizeof(struct block *));
	memset(chain->pending, 0, sizeof(chain->pending));
	chain->nblocks = 1;
	chain->npending = 0;
	chain->difficulty = 4;
	chain->reward = 100;
	chain->blocks[0] = newblock("Genesis", "Genesis", 0, "0");
}

static void
minepending(const char *rewaddr)
{
	struct block *b, *last;
	int i = 0;

	if (chain->npending < 1)
		return;

	if ((chain->blocks = realloc(chain->blocks,
	    sizeof(struct block *) * (chain->nblocks + chain->npending + 1))) == NULL)
		err(1, "realloc");

	for (; i < chain->npending; i++) {
		b = chain->pending[i];
		last = lastblock();
		if (!strcmp(b->prevhash, ""))
			strcpy(b->prevhash, last->hash);
		mineblock(b);
		chain->blocks[chain->nblocks++] = b;
	}
	chain->npending = 0;
	memset(chain->pending, 0, sizeof(chain->pending));
	transaction("Mining Award", rewaddr, chain->reward);
}

static void
mineblock(struct block *b)
{
	int d = chain->difficulty, i = 0;
	char z[d];

	for (; i < d; i++)
		z[i] = '0';
	while (strncmp(b->hash, z, d)) {
		b->nonce++;
		strcpy(b->hash, calchash(b));
	}
	printf("struct block mined: %s\n", b->hash);
}
static struct block *
lastblock(void)
{
	return chain->blocks[chain->nblocks - 1];
}

static int
validchain(void)
{
	int i = 0;

	for (; i < chain->nblocks; i++) {
		if (i != 0 && strcmp(chain->blocks[i]->prevhash,
		    chain->blocks[i-1]->hash))
			return 0;
		if (i != 0 && strcmp(chain->blocks[i]->hash,
		    calchash(chain->blocks[i])))
			return 0;
	}

	return 1;
}

static long
balance(const char *addr)
{
	long bal = 0;
	int i;

	for (i = 0; i < chain->nblocks; i++) {
		if (!strcmp(chain->blocks[i]->data->saddr, addr))
			bal -= chain->blocks[i]->data->amount;
		if (!strcmp(chain->blocks[i]->data->raddr, addr))
			bal += chain->blocks[i]->data->amount;
	}

	return bal;
}

static void
printchain(void)
{
	int i = 0;

	for (; i < chain->nblocks; i++) {
		printf("HASH:	    %s\n", chain->blocks[i]->hash);
		printf("PREVHASH:   %s\n", chain->blocks[i]->prevhash);
		printf("TIMESTAMP:  %s\n", chain->blocks[i]->tstmp);
		printf("DATA:\n");
		printf("    FROM:   %s\n", chain->blocks[i]->data->saddr);
		printf("    TO:     %s\n", chain->blocks[i]->data->raddr);
		printf("    AMOUNT: %ld\n", chain->blocks[i]->data->amount);
		printf("\n");
	}
}

static void
cleanchain(void)
{
	int i = 0;

	for (; i < chain->nblocks; i++) {
		free(chain->blocks[i]->data->saddr);
		free(chain->blocks[i]->data->raddr);
		free(chain->blocks[i]->data);
		free(chain->blocks[i]);
	}
	free(chain->blocks);
	free(chain);
}

static void *
emalloc(size_t nb)
{
	void *p;

	if ((p = malloc(nb)) == NULL)
		err(1, "malloc");

	return p;
}

int
main(int argc, char *argv[])
{
	initchain();

	transaction("Christos", "Lol", 1000);
	transaction("N", "Lol", 1000);
	minepending("miner");

	printchain();
	printf("Valid chain: %s\n", validchain() ? "Yes" : "No");
	printf("Lol's balance: %ld\n", balance("Lol"));

	cleanchain();

	return 0;
}
