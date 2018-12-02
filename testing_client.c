#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "cache.h"

char *host = "localhost";
char *port = "8001";
int response_size = 1024;

struct cache_obj {
    int sockfd;
};

int get_response(cache_type cache, char *response, char *request_format, ...) {
    va_list ap;
    va_start(ap, request_format);
    int length = vsnprintf(NULL, 0, request_format, ap);
    va_start(ap, request_format);
    char *request = (char *) malloc(length+1);
    if (vsnprintf(request, length, request_format, ap) != length) {
        printf("Error forming request message\n");
        // some error code
    }
    printf("Request: %s\n", request);
    if (send(cache->sockfd, request, length, 0) != length) {
        printf("Error sending request message to server\n");
        return -1;
    }
    if (recv(cache->sockfd, response, response_size, 0) < 1) {
        printf("Error receiving response from server, server may have closed connection\n");
        return -1;
    }
    // printf("Response: %s\n", response);
    return 0;
}

cache_type create_cache(index_type maxmem, hash_func hasher) {
    int socket_fd;
    struct addrinfo hints;
    struct addrinfo *serverinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int success = getaddrinfo(host, port, &hints, &serverinfo);
    if (success != 0) {
        printf("Error getting address info of server\n");
        exit(1);
    }
    for (p = serverinfo; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            continue;
        }
        break;
    }
    if (p == NULL) {
        printf("Failed to connect to host\n");
        exit(1);
    }
    printf("Successfully connected to %s\n", host);
    freeaddrinfo(serverinfo);
    // Add code for creating cache with POST /memsize message
    cache_type cache = (cache_type) malloc(sizeof(cache_type));
    if (cache == NULL) {
        printf("Error in malloc for cache\n");
        exit(1);
    }
    cache->sockfd = socket_fd;
    return cache;
}

int cache_set(cache_type cache, key_type key, val_type val, index_type val_size) {
    char *request = "PUT /key/%s/%s HTTP/1.1\r\n";
    char *response = (char *) malloc(response_size*sizeof(char));
    if (get_response(cache, response, request, key, val) != 0) {
        printf("Error communicating with server\n");
        exit(-1);
    }
    char *start_line = strtok(response, "\r\n");
    strtok(start_line, " ");
    char *status = strtok(NULL, " ");
    if (strcmp(status, "200 OK") == 0) {
        free(response);
        return 0;
    }
    else {
        free(response);
        return 1;
    }
}

val_type cache_get(cache_type cache, key_type key, index_type *val_size) {
    char *request = "GET /key/%s HTTP/1.1\r\n";
    char *response = (char *) malloc(response_size*sizeof(char));
    if (get_response(cache, response, request, key) != 0) {
        printf("Error communicating with server\n");
        exit(-1);
    }
    printf("%s\n", response);

    // Cycle through headers to reach response body
    char *start_line = strtok(response, "\r\n");
    char *p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    strtok(start_line, " ");
    char *status = strtok(NULL, " "); // this only returns status code, not message

    // char *p;
    if (strcmp(status, "200") == 0) {
        if (p == NULL) {
            printf("response has no body\n");
            exit(-1);
        }
        printf("response body: %s\n", p);
        // Extract value from JSON formatted string
        p = strtok(p, " ");
        p = strtok(NULL, " ");
        p = strtok(NULL, " ");
        // p = strtok(NULL, " ");
        p = strtok(NULL, "}");
        printf("value: %s\n", p);
        *val_size = strlen(p) + 1;
        char *value = (char *) malloc((strlen(p)+1)*sizeof(char));
        strcpy(value, p);
        free(response);
        return value; // note: recipient has to free value
    }
    else { // add code for what to do if request is improperly formatted
        printf("cache_get not successful for key %s\n", key);
        printf("status: %s\n", status);
        *val_size = 0;
        free(response);
        return NULL;
    }
}

int cache_delete(cache_type cache, key_type key) {
    char *request = "DELETE /key/%s HTTP/1.1\r\n";
    char *response = (char *) malloc(response_size*sizeof(char));
    if (get_response(cache, response, request, key) != 0) {
        printf("Error communicating with server\n");
        exit(-1);
    }

    char *start_line = strtok(response, "\r\n");
    strtok(start_line, " ");
    char *status = strtok(NULL, " ");
    if (strcmp(status, "200 OK") == 0) {
        free(response);
        return 0;
    }
    else {
        free(response);
        return 1;
    }
}

index_type cache_space_used(cache_type cache) {
    char *request = "GET /memsize HTTP/1.1\r\n";
    char *response = (char *) malloc(response_size*sizeof(char));
    if (get_response(cache, response, request) != 0) {
        printf("Error communicating with server\n");
        exit(-1);
    }

    char *start_line = strtok(response, "\r\n");
    char *p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    p = strtok(NULL, "\r\n");
    strtok(start_line, " ");
    char *status = strtok(NULL, " "); // this only returns status code, not message

    // Cycle through headers to reach response body
    if (strcmp(status, "200") == 0) {
        while (strcmp(p, "") != 0) {
            p = strtok(NULL, "\r\n");
        }
        p = strtok(NULL, "\r\n");
        if (p != NULL) {
            // printf some error message
            exit(-1);
        }
        
        // Extract value from JSON formatted string
        p = strtok(p, " ");
        p = strtok(NULL, "}");
        index_type memused = atoi(p);
        free(response);
        return memused;
    }
    else { // add code for what to do if request is improperly formatted
        free(response);
        return 0; // arbitrary return value for now
    }
}

void destroy_cache(cache_type cache) {
    char *request = "POST /shutdown HTTP/1.1\r\n";
    char *response = (char *) malloc(response_size*sizeof(char));
    if (get_response(cache, response, request) != 0) {
        printf("Error communicating with server\n");
        exit(-1);
    }

    char *start_line = strtok(response, "\r\n");
    strtok(start_line, " ");
    char *status = strtok(NULL, " ");
    if (strcmp(status, "200 OK") == 0) {
        printf("Cache successfully destroyed\n");
        close(cache->sockfd);
        free(cache);
        free(response);
    }
    else {
        printf("Error, either incorrectly formatted message or server problem\n");
        free(response);
    }
}