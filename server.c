#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <netdb.h>

#include "cache.h"

index_type default_hasher(key_type key);

index_type DEFAULT_MAXMEM = 100;
char *DEFAULT_PORT = "8001";

// I borrowed the code to set up the socket server from here, after reading and understanding it:
// http://beej.us/guide/bgnet/html/single/bgnet.html.

void handle_message(int i, cache_type cache, char *buffer, int *exit_main) {
    int formatted = 0;
    int exit = 0;
    printf("Request: %s\n", buffer);
    char *start_line = strtok(buffer, "\r\n");
    printf("start_line: %s\n", start_line);
    char *method = strtok(start_line, " ");
    char *target = strtok(NULL, " ");
    printf("target: %s\n", target);
    char *http_version = strtok(NULL, " ");
    char *response_body = "";
    char *status = " 200 OK\r\n";
    if (start_line == NULL || method == NULL || target == NULL || http_version == NULL) {
        status = " 400 Bad Request\r\n";
        formatted = 1;
        if (http_version == NULL) {
            http_version = "HTTP/1.1";
        }
    }
    time_t t = time(NULL);
    struct tm *tmp;
    tmp = localtime(&t);
    char date[1000];
    int time_formatted = strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", tmp);
    if (time_formatted == 0) {
        printf("Error formatting date string\n");
        return;
        // send some error message
    }
    // strcat(response, date);
    char *headers = "Accept: text/html\r\nContent-Type: application/json\r\n\r\n";
    // strcat(response, "Accept: text/html\r\n");
    // strcat(response, "Content-Type: application/json\r\n\r\n");
    if (formatted == 0) {
        if (strcmp(method, "GET") == 0) {
            // char *target = strtok(NULL, " ");
            // http_version = strtok(NULL, " ");
            char *subsets = strtok(target, "/");
            if (strcmp(subsets, "key") == 0) {
                index_type *val_size = malloc(sizeof(index_type));
                key_type key = strtok(NULL, "/");
                if (key == NULL) {
                    printf("Proper usage: GET /key/k\n");
                    status = " 400 Bad Request\r\n";
                } else {
                    char *value = (char *) cache_get(cache, key, val_size);
                    if (value == NULL) {
                        printf("Cache does not contain a value for this key\n");
                        status = " 404 Not Found\r\n";
                    } else {
                        printf("value requested: %s\n", value);
                        int space_required = snprintf(NULL, 0, "{key: %s, value: %s}", key, value)+1;
                        response_body = malloc(space_required*sizeof(char));
                        snprintf(response_body, space_required, "{key: %s, value: %s}", key, value);
                        // char *response_body = "{ key: ";
                        // strcat(response_body, key);
                        // strcat(response_body, ", value: ");
                        // strcat(response_body, value);
                        // strcat(response_body, " }");
                        free(val_size);
                    }
                }
            }
            else {if (strcmp(subsets, "memsize") == 0) {
                index_type memused = cache_space_used(cache);
                printf("%d", memused);
                int space_required = snprintf(NULL, 0, "{memused: %d}", memused)+1;
                printf("Space required: %d\n", space_required);
                response_body = malloc(space_required*sizeof(char));
                if (response_body == NULL) {
                    printf("Error allocating memory\n");
                    return;
                }
                snprintf(response_body, space_required, "{memused: %d}", memused); // segfault here
                printf("%s\n", response_body);
                // strcat(response, response_body);
                // free(response_body);
            }
            else {
                printf("Proper usage: GET /key/k or GET /memsize\n");
                status = " 400 Bad Request\r\n";
            }}
        }
        else {if (strcmp(method, "PUT") == 0) {
            // char *target = strtok(NULL, " ");
            // char *http_version = strtok(NULL, " ");
            // strcat(http_version, response);
            // response = http_version;
            char *subsets = strtok(target, "/");
            if (strcmp(subsets, "key") != 0) {
                printf("Proper usage: PUT /key/k/v\n");
                status = " 400 Bad Request\r\n";
                // break out of switch statement
            } else {
                key_type key = strtok(NULL, "/");
                if (key == NULL) {
                    printf("Proper usage: PUT /key/k/v\n");
                    status = " 400 Bad Request\r\n";
                    // break out of switch statement
                } else {
                    key_type value = strtok(NULL, "/");
                    if (value == NULL) {
                        printf("Proper usage: PUT /key/k/v\n");
                        status = " 400 Bad Request\r\n";
                        // break out of switch statement
                    } else {
                        if (cache_set(cache, key, value, (strlen(value)+1)*sizeof(char)) != 0) {
                            printf("Error in setting value in cache\n");
                            status = " 500 Internal Server Error\r\n";
                            // break out of switch statement
                        }
                    }
                }
            }
        }
        else {if (strcmp(method, "DELETE") == 0) {
            // char *target = strtok(NULL, " ");
            // char *http_version = strtok(NULL, " ");
            // strcat(http_version, response);
            // response = http_version;
            char *subsets = strtok(target, "/");
            if (strcmp(subsets, "key") != 0) {
                printf("Proper usage: DELETE /key/k");
                status = " 400 Bad Request\r\n";
                // break out of switch statement
            } else {
                key_type key = strtok(NULL, "/");
                if (key == NULL) {
                    printf("Proper usage: DELETE /key/k");
                    status = " 400 Bad Request\r\n";
                    // break out of switch statement
                } else {
                    if (cache_delete(cache, key) != 0) {
                        printf("Error while deleting key-value pair from cache\n");
                        status = " 500 Internal Server Error\r\n";
                        // break out of switch statement
                    }
                }
            }
        }
        else {if (strcmp(method, "HEAD") == 0) {
            // placeholder for now
        }
        else {if (strcmp(method, "POST") == 0) {
            // char *target = strtok(NULL, " ");
            // char *http_version = strtok(NULL, " ");
            // strcat(http_version, response);
            // response = http_version;
            char *subsets = strtok(target, "/");
            if (strcmp(subsets, "shutdown") == 0) {
                if (shutdown(i, 0) != 0) { // The 0 passed to shutdown means no more remote writing to socket
                    printf("Error shutting down socket\n");
                    status = " 500 Internal Server Error\r\n";
                    // break out of switch statement
                } else {
                    destroy_cache(cache);
                    exit = 1;
                }
            } else {
                printf("Proper usage: POST /shutdown\n");
                status = " 400 Bad Request\r\n";
            }
            // else {if (strcmp(subsets, "memsize") == 0) {
            //     char *subsets = strtok(NULL, "/");
            //     if (subsets == NULL) {
            //         printf("Proper usage: POST /memsize/m\n");
            //         status = " 400 Bad Request\r\n";
            //     } else {
            //         index_type memsize = atoi(subsets);
            //         cache = create_cache(memsize, &default_hasher);
            //     }
            // } else {
            //     printf("Proper usage: POST /shutdown\nPOST /memsize/m\n");
            //     status = " 400 Bad Request\r\n";
            //     // break out of switch statement
            // }}
        }
        else {
            printf("Correct usage:\nGET /key/k\nGET /memsize\nPUT /key/k/v\nDELETE /key/k\nHEAD /key/k\nPOST /shutdown\n");
            status = " 400 Bad Request\r\n";
        }}}}}
    }
    
    int response_size = strlen(http_version) + strlen(status) + strlen(date) + strlen(headers) + strlen(response_body)+1;
    // printf("%d\n", response_size);
    char *response = malloc(response_size);
    snprintf(response, response_size, "%s%s%s%s%s", http_version, status, date, headers, response_body);
    // strcat(response, http_version);
    // strcat(response, status);
    // strcat(response, date);
    // strcat(response, headers);
    // strcat(response, response_body);
    // printf("Response body: %s\n", response_body);
    // printf("Response: %s\n", response);
    int response_length = strlen(response)+1;
    if (send(i, response, response_length, 0) != response_length) {
        printf("Error sending message to client\n");
        return;
    }
    if (exit == 1) {
        *exit_main = 1;
    }
}

int main(int argc, char **argv) {
    
    // Parse command-line input
    char opt;
    index_type maxmem = DEFAULT_MAXMEM;
    char *portnum = DEFAULT_PORT;
    char *options = "m:t:";
    while ((opt = getopt(argc, argv, options)) != -1) {
        if (opt == 'm') {
            maxmem = atoi(optarg);
        }
        else {if (opt == 't') {
            portnum = optarg;
        }
        else {
            printf("Usage: %s [-m maxmem] [-t portnum]\n", argv[0]);
            return 1;
        }}
    }

    cache_type cache = create_cache(maxmem, &default_hasher);

    fd_set master;
    fd_set read_fds;
    int fdmax;

    int server_fd;
    int new_socket;
    struct sockaddr_storage client_address;
    socklen_t addr_size;

    int valread;
    char buffer[1024];
    int exit_main = 0;

    // Set up TCP server socket
    struct addrinfo hints;
    struct addrinfo *serverinfo, *p;
    int yes = 1;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int success = getaddrinfo(NULL, portnum, &hints, &serverinfo);
    if (success != 0) {
        printf("Error setting up server struct addrinfo\n");
        exit(1);
    }

    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            printf("Error in setsockopt\n");
            close(server_fd);
            continue;
        }
        if (bind(server_fd, p->ai_addr, p->ai_addrlen) == -1) {
            printf("Error in bind\n");
            close(server_fd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        printf("Failed to bind to a port\n");
        exit(2);
    }

    freeaddrinfo(serverinfo);

    if (listen(server_fd, 10) < 0) {
        printf("Error in listening to port\n");
        return 4;
    }

    FD_SET(server_fd, &master);

    fdmax = server_fd;

    // Loop infinitely, waiting for and acting on requests
    while (1) {
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            printf("Error in select\n");
            exit(4);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) {
                    addr_size = sizeof(client_address);
                    if ((new_socket = accept(server_fd, (struct sockaddr *) &client_address, &addr_size)) < 0) {
                        printf("Error in accepting connection request\n");
                        return 5;
                    }
                    else {
                        FD_SET(new_socket, &master);
                        if (new_socket > fdmax) {
                            fdmax = new_socket;
                        }
                        printf("New connection on socket %d\n", new_socket);
                    }
                }
                else {
                    if ((valread = recv(i, buffer, 1024, 0)) <= 0) {
                        if (valread == 0) {
                            printf("Connection closed by client\n");
                        }
                        else {
                            printf("Error receiving message from client\n");
                        }
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else {
                        printf("valread: %d\n", valread);
                        printf("buffer: %s\n", buffer);
                        handle_message(i, cache, buffer, &exit_main);
                        if (exit_main == 1) {
                            for (int k = 0; k <= fdmax; k++) {
                                close(k);
                            }
                            FD_ZERO(&master);
                            break;
                        }
                    }
                }
            }
        }
    }
}