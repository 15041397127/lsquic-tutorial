/* Copyright (c) 2020 LiteSpeed Technologies */
/*
 * tut.c is the example program to illustrate lsquic API usage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lsquic.h"


static int
tut_log_buf (void *ctx, const char *buf, size_t len)
{
    FILE *out = ctx;
    fwrite(buf, 1, len, out);
    fflush(out);
    return 0;
}
static const struct lsquic_logger_if logger_if = { tut_log_buf, };


static int
tut_packets_out (void *packets_out_ctx,
    const struct lsquic_out_spec *out_spec,
    unsigned n_packets_out)
{
    /* TODO */  return -1;
}


static void
tut_usage (const char *argv0)
{
    const char *name;

    name = strchr(argv0, '/');
    if (name)
        ++name;
    else
        name = argv0;

    fprintf(stdout,
"Usage: %s [options]\n"
"\n"
"   -f log.file     Log message to this log file.  If not specified, the\n"
"                     are printed to stderr.\n"
"   -L level        Set library-wide log level.  Defaults to 'warn'.\n"
"   -l module=level Set log level of specific module.  Several of these\n"
"                     can be specified via multiple -l flags or by combining\n"
"                     these with comma, e.g. -l event=debug,conn=info.\n"
"   -h              Print this help screen and exit.\n"
    , name);
}


static lsquic_conn_ctx_t *
tut_client_on_new_conn (void *stream_if_ctx, struct lsquic_conn *conn)
{
    return NULL;
}


static void
tut_client_on_conn_closed (struct lsquic_conn *conn)
{
}


static lsquic_stream_ctx_t *
tut_client_on_new_stream (void *stream_if_ctx, struct lsquic_stream *stream)
{
    return NULL;
}


static void
tut_client_on_read (struct lsquic_stream *stream, lsquic_stream_ctx_t *h)
{
}


static void
tut_client_on_write (struct lsquic_stream *stream, lsquic_stream_ctx_t *h)
{
}


static void
tut_client_on_close (struct lsquic_stream *stream, lsquic_stream_ctx_t *h)
{
}


static const struct lsquic_stream_if tut_client_callbacks =
{
    .on_new_conn        = tut_client_on_new_conn,
    .on_conn_closed     = tut_client_on_conn_closed,
    .on_new_stream      = tut_client_on_new_stream,
    .on_read            = tut_client_on_read,
    .on_write           = tut_client_on_write,
    .on_close           = tut_client_on_close,
};


int
main (int argc, char **argv)
{
    struct lsquic_engine_api eapi;
    struct lsquic_engine *engine;
    FILE *log_fh = stderr;
    int opt;

    if (0 != lsquic_global_init(LSQUIC_GLOBAL_SERVER|LSQUIC_GLOBAL_CLIENT))
    {
        fprintf(stderr, "global initialization failed\n");
        exit(EXIT_FAILURE);
    }

    while (opt = getopt(argc, argv, "f:l:L:h"), opt != -1)
    {
        switch (opt)
        {
        case 'f':
            log_fh = fopen(optarg, "ab");
            if (!log_fh)
            {
                perror("cannot open log file for writing");
                exit(EXIT_FAILURE);
            }
            break;
        case 'l':
            if (0 != lsquic_logger_lopt(optarg))
            {
                fprintf(stderr, "error processing -l option\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'L':
            if (0 != lsquic_set_log_level(optarg))
            {
                fprintf(stderr, "error processing -L option\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            tut_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
    }

    /* Initialize logging */
    lsquic_logger_init(&logger_if, log_fh, LLTS_NONE);

    /* Initialize callbacks */
    memset(&eapi, 0, sizeof(eapi));
    eapi.ea_packets_out = tut_packets_out;
    eapi.ea_stream_if   = &tut_client_callbacks;

    engine = lsquic_engine_new(LSENG_SERVER, &eapi);
    if (!engine)
    {
        fprintf(stderr, "cannot create engine\n");
        exit(EXIT_FAILURE);
    }

    lsquic_engine_destroy(engine);
    lsquic_global_cleanup();
    exit(EXIT_SUCCESS);
}
