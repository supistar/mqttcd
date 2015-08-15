/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include "mqttcd_arg.h"

int parse_arguments(mqttcd_context_t* context, int argc, char** argv) {
    struct option options[] = {
        { "host",        required_argument, NULL, 0 },
        { "port",        required_argument, NULL, 0 },
        { "version",     required_argument, NULL, 0 },
        { "client_id",   required_argument, NULL, 0 },
        { "username",    required_argument, NULL, 0 },
        { "password",    required_argument, NULL, 0 },
        { "topic",       required_argument, NULL, 0 },
        { "daemonize",   no_argument,       NULL, 0 },
        { "handler",     required_argument, NULL, 0 },
        { "handler_dir", required_argument, NULL, 0 },
        { "handler_name",required_argument, NULL, 0 },
        { 0,             0,                 0,    0 }
    };

    char** raw_options[] = {
        &context->raw_option.host,
        &context->raw_option.port,
        &context->raw_option.version,
        &context->raw_option.client_id,
        &context->raw_option.username,
        &context->raw_option.password,
        &context->raw_option.topic,
        &context->raw_option.daemonize,
        &context->raw_option.handler,
        &context->raw_option.handler_dir,
        &context->raw_option.handler_name
    };

    // initialize variables
    int i;
    for (i = 0; i < sizeof(raw_options) / sizeof(raw_options[0]); i++) {
        *(raw_options[i]) = NULL;
    }

    // parse command line arguments
    int result;
    int index;
    while ((result = getopt_long(argc, argv, "", options, &index)) != -1) {
        if (result != 0) {
            return MQTTCD_PARSE_ARG_FAILED;
        }

        if (options[index].has_arg != no_argument) {
            int length = strlen(optarg) + 1;
            *(raw_options[index]) = malloc(length);
            strncpy(*(raw_options[index]), optarg, length);
        } else {
            *(raw_options[index]) = malloc(1);
            (*(raw_options[index]))[0] = '\0';
        }
    }

    // check parsed arguments
    if (context->raw_option.host != NULL) {
        context->option.host = context->raw_option.host;
    } else {
        return MQTTCD_PARSE_ARG_FAILED;
    }

    if (context->raw_option.port != NULL) {
        context->option.port = atoi(context->raw_option.port);
    } else {
        context->option.port = 1883; // default is 1883.
    }

    if (context->raw_option.version != NULL) {
        context->option.version = atoi(context->raw_option.version);
    } else {
        context->option.version = 4; // default is 4. 3 or 4.
    }

    if (context->raw_option.client_id == NULL) {
        // default is process id and host name
        char client_id[24] = { 0 }; // 23byte(without null), http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html#_Toc398718031
        pid_t pid = getpid();
        char hostname[_POSIX_HOST_NAME_MAX] = { 0 };
        if (gethostname(hostname, _POSIX_HOST_NAME_MAX) != 0) {
            strcpy(hostname, "empty"); // fallback
        }

        if (snprintf(client_id, 24, "mqttcd/%d-%s", pid, hostname) < 0) {
            return MQTTCD_PARSE_ARG_FAILED;
        }

        int length = strlen(client_id) + 1;
        context->raw_option.client_id = malloc(length);
        strncpy(context->raw_option.client_id, client_id, length);
    }
    context->option.client_id = context->raw_option.client_id;

    if (context->raw_option.username == NULL) {
        // default is empty
        context->raw_option.username = malloc(1);
        context->raw_option.username[0] = '\0';
    }
    context->option.username = context->raw_option.username;

    if (context->raw_option.password == NULL) {
        // default is empty
        context->raw_option.password = malloc(1);
        context->raw_option.password[0] = '\0';
    }
    context->option.password = context->raw_option.password;

    if (context->raw_option.topic != NULL) {
        context->option.topic = context->raw_option.topic;
    } else {
        return MQTTCD_PARSE_ARG_FAILED;
    }

    if (context->raw_option.daemonize != NULL) {
        context->option.daemonize = 1;
    } else {
        context->option.daemonize = 0; // default is not daemonize
    }

    if (context->raw_option.handler != NULL) {
        if (strcmp(context->raw_option.handler, "nop") == 0) {
            context->option.handler = MQTTCD_HANDLER_NOP;
        }
        if (strcmp(context->raw_option.handler, "string") == 0) {
            context->option.handler = MQTTCD_HANDLER_STRING;
        }
    } else {
        context->option.handler = MQTTCD_HANDLER_NOP; // default is nop
    }

    if (context->raw_option.handler_dir == NULL) {
        // default is current directory
        context->raw_option.handler_dir = realpath("./handlers", NULL);
    }
    context->option.handler_dir = context->raw_option.handler_dir;

    if (context->raw_option.handler_name == NULL) {
        // Default handler name is "default"
        context->raw_option.handler_name = strdup("default");
    }
    context->option.handler_name = context->raw_option.handler_name;

    return MQTTCD_SUCCEEDED;
}

int free_arguments(mqttcd_context_t* context) {
    char** raw_options[] = {
        &context->raw_option.host,
        &context->raw_option.port,
        &context->raw_option.version,
        &context->raw_option.client_id,
        &context->raw_option.username,
        &context->raw_option.password,
        &context->raw_option.topic,
        &context->raw_option.daemonize,
        &context->raw_option.handler,
        &context->raw_option.handler_dir,
        &context->raw_option.handler_name
    };

    int i;
    for (i = 0; i < sizeof(raw_options) / sizeof(raw_options[0]); i++) {
        if (*(raw_options[i]) != NULL) {
            free(*(raw_options[i]));
        }
    }

    return MQTTCD_SUCCEEDED;
}

