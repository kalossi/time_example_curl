#include <stdio.h>
#include <time.h>
#include <curl/curl.h>
#include <string.h>

// Callback function to capture response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    // Save response to data buffer
    strcat(data, (char*)ptr);
    return size * nmemb;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <http_url>\n", argv[0]);
        return 1;
    }

    time_t now = time(NULL);
    if (now == -1) {
        perror("Failed to get time");
        return 1;
    }

    char json[64];
    snprintf(json, sizeof(json), "{\"epoch\":%ld}", now);

    CURL *curl = curl_easy_init();
    if (!curl) return 1;

    char response_data[1024] = "";  // Buffer to store response

    // Prepare the headers (Content-Type)
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Setting up the callback function to capture the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);

    long response_code = 0;
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        // Capture the response code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("HTTP Response Code: %ld\n", response_code);

        // Print out the response body
        printf("Response Body: %s\n", response_data);
    } else {
        fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
    }

    // Clean up the headers
    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);
    return res == CURLE_OK ? 0 : 1;
}
