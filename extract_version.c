#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

char* find_version(const char* buffer, size_t length, const char* search_string) {
    const char* start = buffer;
    const char* end = buffer + length;

    while (start < end) {
        const char* version_start = strstr(start, search_string);
        if (version_start != NULL) {
            version_start += strlen(search_string);
            const char* version_end = strchr(version_start, ' ');
            if (version_end != NULL) {
                size_t version_length = version_end - version_start;
                char* version = malloc(version_length + 1);
                strncpy(version, version_start, version_length);
                version[version_length] = '\0';
                return version;
            }
        }
        start += strlen(start) + 1;  // Move to the next chunk
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No file path provided.\n");
        return 1;
    }

    const char* filename = argv[1];
    const size_t chunk_size = 256;
    char buffer[chunk_size];

    // Open the binary file
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Read the file in chunks
    size_t total_bytes_read = 0;
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), chunk_size - 1, file)) > 0) {
        buffer[bytes_read] = '\0';

        // Search for the version string
        const char* search_string = "ATCU_FIRMWARE_VER ";
        char* version = find_version(buffer, bytes_read, search_string);

        // Print the version
        if (version != NULL) {
            printf("Version: %s\n", version);
            free(version);
            break;  // Version found, exit the loop
        }

        total_bytes_read += bytes_read;
        fseek(file, total_bytes_read, SEEK_SET);  // Move to the next chunk
    }

    // Close the file
    fclose(file);

    if (total_bytes_read == 0) {
        printf("No version found.\n");
    }

    // Wait for user input before exiting
    printf("\nPress enter to exit...");
    getchar();

    return 0;
}