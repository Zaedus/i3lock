/*
 * vim:ts=4:sw=4:expandtab
 *
 * See LICENSE for licensing information
 *
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <security/pam_appl.h>

#include "fprint.h"


static int
fprint_conv_callback(int num_msg, const struct pam_message **msg,
                         struct pam_response **resp, void *appdata_ptr) {
    if (num_msg == 0)
        return PAM_SUCCESS;

    /* PAM expects an array of responses, one for each message */
    if ((*resp = calloc(num_msg, sizeof(struct pam_response))) == NULL) {
        perror("calloc");
        return PAM_BUF_ERR;
    }

    for (int i = 0; i < num_msg; i++)
        printf("CONV: %s\n", msg[i]->msg);
    fflush(stdout);

    /*
     * Use PAM_CONV_AGAIN here because the return value is ignored by the fprint
     * module but honored by the others. That way if the i3lock-fprint PAM
     * service is misconfigured we report that we're not ready.
     */
    return PAM_CONV_AGAIN;
}

int
fprint_main(const char *username)
{
    int ret, done;
    pam_handle_t *pam_handle;
    struct pam_conv conv = {fprint_conv_callback, NULL};

    /* Initialize PAM */
    if ((ret = pam_start("i3lock-fprint", username, &conv, &pam_handle)) != PAM_SUCCESS)
        errx(EXIT_FAILURE, "PAM: %s", pam_strerror(pam_handle, ret));

    if ((ret = pam_set_item(pam_handle, PAM_TTY, getenv("DISPLAY"))) != PAM_SUCCESS)
        errx(EXIT_FAILURE, "PAM: %s", pam_strerror(pam_handle, ret));

    done = 0;
    do {
        /* start the PAM authentication */
        printf("PAM_AUTH_STARTING\n");
        fflush(stdout);
        int ret = pam_authenticate(pam_handle, 0);
        switch (ret) {
            case PAM_SUCCESS:
                /* PAM credentials should be refreshed, this will for example update any kerberos tickets.
                 * Related to credentials pam_end() needs to be called to cleanup any temporary
                 * credentials like kerberos /tmp/krb5cc_pam_* files which may of been left behind if the
                 * refresh of the credentials failed. */
                pam_setcred(pam_handle, PAM_REFRESH_CRED);
                printf("PAM_AUTH_SUCCESS\n");
                done = 1;
                break;
            case PAM_ABORT:
                printf("PAM_ABORT\n");
                break;
            case PAM_AUTH_ERR:
                printf("PAM_AUTH_ERR\n");
                break;
            case PAM_CRED_INSUFFICIENT:
                printf("PAM_CRED_INSUFFICIENT\n");
                break;
            case PAM_AUTHINFO_UNAVAIL:
                printf("PAM_AUTHINFO_UNAVAIL\n");
                break;
            case PAM_MAXTRIES:
                printf("PAM_MAXTRIES\n");
                break;
            case PAM_USER_UNKNOWN:
                printf("PAM_USER_UNKNOWN\n");
                break;
            case PAM_PERM_DENIED:
                printf("PAM_PERM_DENIED\n");
                break;
            default:
                printf("unexpected PAM status %d\n", ret);
                break;
        }
        fflush(stdout);

        if (!done) {
            char buf[BUFSIZ] = { 0 };
            switch (read(STDIN_FILENO, buf, BUFSIZ - 1)) {
            case -1: /* handle read error */
            case 0: /* handle closed connection */
                done = 1;
                break;
            }
        }
    } while (!done);

    /* cleanup */
    pam_end(pam_handle, ret);
    return EXIT_SUCCESS;
}
