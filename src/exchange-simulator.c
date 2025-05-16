#include "../include/config.h"
#include "../include/sor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <ctype.h> 

// Structure to hold response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function for CURL
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Helper function to validate stock symbol
static int is_valid_symbol(const char *symbol) {
    if (!symbol || strlen(symbol) < 1 || strlen(symbol) > 5) {
        return 0;
    }
    
    // Check if all characters are alphabetic
    for (int i = 0; symbol[i] != '\0'; i++) {
        if (!isalpha(symbol[i])) {
            return 0;
        }
    }
    return 1;
}

// Function to get user input for stock symbol and quantity
void get_user_input(char *symbol, int *quantity) {
    char input[256];
    int valid_input = 0;
    
    // Get and validate (SYNTAX) stock symbol
    while (!valid_input) {
        printf("Enter stock symbol (e.g., AAPL): ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove newline if present
            input[strcspn(input, "\n")] = 0;
            
            if (is_valid_symbol(input)) {
                strncpy(symbol, input, 9); // Ensure we don't overflow symbol buffer
                symbol[9] = '\0'; // Ensure null termination
                valid_input = 1;
            } else {
                printf("❌ Invalid stock symbol. Please enter 1-5 alphabetic characters.\n");
            }
        }
    }
    
    // Get and validate (SYNTAX) quantity
    valid_input = 0;
    while (!valid_input) {
        printf("Enter quantity to buy (Integer): ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            char *endptr;
            long num = strtol(input, &endptr, 10);
            
            // Check if conversion was successful and number is positive
            if (*endptr == '\n' || *endptr == '\0') {
                if (num > 0 && num <= INT_MAX) {
                    *quantity = (int)num;
                    valid_input = 1;
                } else {
                    printf("❌ Invalid quantity. Please enter a positive integer");
                }
            } else {
                printf("❌ Invalid input. Please enter a valid integer.\n");
            }
        }
    }
    
    printf("\n✅ Input received: %d shares of %s\n\n", *quantity, symbol);
}

// Fetch stock data from Yahoo Finance
double fetch_stock_price(const char* symbol) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    double price = 0.0;
    
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Set up the response buffer
        chunk.memory = malloc(1);
        chunk.size = 0;

        // Construct Yahoo Finance API URL
        char url[256];
        snprintf(url, sizeof(url), 
                "https://query1.finance.yahoo.com/v8/finance/chart/%s?interval=1d&range=1d",
                symbol);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            // Parse JSON response
            struct json_object *parsed_json = json_tokener_parse(chunk.memory);
            struct json_object *chart, *result, *meta, *regularMarketPrice;
            
            if (json_object_object_get_ex(parsed_json, "chart", &chart) &&
                json_object_object_get_ex(chart, "result", &result) &&
                json_object_array_length(result) > 0) {
                
                struct json_object *first_result = json_object_array_get_idx(result, 0);
                if (json_object_object_get_ex(first_result, "meta", &meta) &&
                    json_object_object_get_ex(meta, "regularMarketPrice", &regularMarketPrice)) {
                    
                    price = json_object_get_double(regularMarketPrice);
                } else {
                    printf("Failed to fetch stock price, try again with a valid symbol\n");
                }
            } else {
                printf("Failed to fetch stock price, try again with a valid symbol\n");
            }
            json_object_put(parsed_json);
        } else {
            printf("Failed to fetch stock price, try again with a valid symbol\n");
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        free(chunk.memory);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return price;
}

// Fill exchange data with live price information
void fill_exchange_data(Exchange exchanges[], const char* symbol) {
    double base_price = fetch_stock_price(symbol);
    
    if (base_price <= 0.0) {
        // If price fetch fails, use default values
        strcpy(exchanges[0].name, "SGX");
        strcpy(exchanges[1].name, "HKEX");
        strcpy(exchanges[2].name, "TSE");
        
        exchanges[0].price = 101.5;
        exchanges[0].latency = 5.2;
        exchanges[0].volume = 700;
        
        exchanges[1].price = 100.2;
        exchanges[1].latency = 6.5;
        exchanges[1].volume = 800;
        
        exchanges[2].price = 99.9;
        exchanges[2].latency = 8.0;
        exchanges[2].volume = 400;
    } else {
        // Initialize exchange names
        strcpy(exchanges[0].name, "SGX");
        strcpy(exchanges[1].name, "HKEX");
        strcpy(exchanges[2].name, "TSE");
        
        // Fill with live data
        for (int i = 0; i < NUM_EXCHANGES; i++) {
            exchanges[i].price = base_price * (1.0 + (rand() % 100 - 50) / 1000.0); // ±5% variation
            exchanges[i].latency = (rand() % 100) / 1000.0; // 0-100ms latency
            exchanges[i].volume = (rand() % 1000) + 100; // 100-1100 volume
        }
    }

    // Print exchange data in a formatted table
    printf("\n📊 Current Stock Data:\n");
    printf("┌─────────┬──────────┬──────────┬──────────┬──────────┐\n");
    printf("│ Exchange│   Price  │ Latency  │ Volume   │ Score    │\n");
    printf("├─────────┼──────────┼──────────┼──────────┼──────────┤\n");
    
    for (int i = 0; i < NUM_EXCHANGES; i++) {
       exchanges[i].score = 0.0;
        printf("│ %-7s │ %8.2f │ %8.2f │ %8d │ %8.2f │\n",
               exchanges[i].name,
               exchanges[i].price,
               exchanges[i].latency,
               exchanges[i].volume,
               exchanges[i].score);
    }
    printf("└─────────┴──────────┴──────────┴──────────┴──────────┘\n\n");
}
