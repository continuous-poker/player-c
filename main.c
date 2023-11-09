#include <signal.h>
#include "mongoose.h"

void decide(struct mg_str* pBody, int *pBet) {
    (void ) pBody;
    //TODO: Add Poker Logic Here... :)
    *pBet = 0;
}

/*
 *  #########################################
 *  #               SERVER                  #
 *  #########################################
 */

// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) {
    s_signo = signo;
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        // Log Request
        struct mg_http_message *hm = (struct mg_http_message *) ev_data, tmp = {0};
        struct mg_str unknown = mg_str_n("?", 1), *cl;
        cl = mg_http_get_header(&tmp, "Content-Length");
        if (cl == NULL) cl = &unknown;
        MG_INFO(("%.*s %.*s %.*s %.*s", (int) hm->method.len, hm->method.ptr, (int) hm->uri.len, hm->uri.ptr, (int) tmp.uri.len, tmp.uri.ptr, (int) cl->len, cl->ptr));
        (void) fn_data;

        // Router
        if (mg_http_match_uri(hm, "/") && strncmp(hm->method.ptr, "GET", hm->method.len) == 0) {
            mg_http_reply(c, 200, NULL, "Poker C Mongoose Player");
        } else if (mg_http_match_uri(hm, "/") && strncmp(hm->method.ptr, "POST", hm->method.len) == 0) {
            int bet;
            decide(&hm->body, &bet);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m:%d}\n", mg_print_esc, 0, "bet", bet);
        } else {
            mg_http_reply(c, 500, NULL, "\n");
        }
    }
}


int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    MG_INFO(("%s", "Run Server on: http://0.0.0.0:8000"));
    mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, &mgr);
    while (s_signo == 0) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    MG_INFO(("Exiting on signal %d", s_signo));
    return 0;
}