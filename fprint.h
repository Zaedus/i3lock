#ifndef FPRINT_H
#define FPRINT_H
/*
 * vim:ts=4:sw=4:expandtab
 *
 * See LICENSE for licensing information
 *
 */
#include "compat.h"

/* FIXME: just a constant that is not a PAM return status */
#define	FPRINT_PAM_CONV	128

int	fprint_main(struct imsgbuf *ibuf, const char *username);

#endif /* ndef FPRINT_H */
