/*
 * vim:ts=4:sw=4:expandtab
 *
 * See LICENSE for licensing information
 *
 */
#include <err.h>
#include <stdlib.h>

#include <security/pam_appl.h>

#include "fprint.h"
#include "compat.h"


static int
fprint_conv_callback(int num_msg, const struct pam_message **msg,
                         struct pam_response **resp, void *appdata_ptr) {
    struct imsgbuf *ibuf = appdata_ptr;
    if (num_msg == 0)
        return PAM_SUCCESS;

    /* message the main i3lock process, effectively "calling" imsg_recv_cb */
    imsg_compose(ibuf, FPRINT_PAM_CONV, 0, 0, -1, NULL, 0);
    (void)imsg_flush(ibuf);

    /* PAM expects an array of responses, one for each message */
    if ((*resp = calloc(num_msg, sizeof(struct pam_response))) == NULL) {
        perror("calloc");
        return PAM_BUF_ERR;
    }
    return PAM_CONV_AGAIN;
}

int
fprint_main(struct imsgbuf *ibuf, const char *username)
{
    int ret;
    pam_handle_t *pam_handle;
    struct pam_conv conv = {fprint_conv_callback, ibuf};

    /* Initialize PAM */
    if ((ret = pam_start("i3lock-fprint", username, &conv, &pam_handle)) != PAM_SUCCESS)
        errx(EXIT_FAILURE, "PAM: %s", pam_strerror(pam_handle, ret));

    if ((ret = pam_set_item(pam_handle, PAM_TTY, getenv("DISPLAY"))) != PAM_SUCCESS)
        errx(EXIT_FAILURE, "PAM: %s", pam_strerror(pam_handle, ret));

    /* start the PAM authentication */
    ret = pam_authenticate(pam_handle, 0);
    if (ret == PAM_SUCCESS) {
        /* PAM credentials should be refreshed, this will for example update any kerberos tickets.
         * Related to credentials pam_end() needs to be called to cleanup any temporary
         * credentials like kerberos /tmp/krb5cc_pam_* files which may of been left behind if the
         * refresh of the credentials failed. */
        pam_setcred(pam_handle, PAM_REFRESH_CRED);
    }

    /* message the main i3lock process, effectively "calling" imsg_recv_cb */
    imsg_compose(ibuf, ret, 0, 0, -1, NULL, 0);
    if (imsg_flush(ibuf) == -1)
        return EXIT_FAILURE;

    /* cleanup */
    pam_end(pam_handle, ret);
    return EXIT_SUCCESS;
}
