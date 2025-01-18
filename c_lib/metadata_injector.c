#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

// Helper function to write metadata to the beginning of the file (used for file types like JPEG)
void prepend_metadata(const char *file_path, const char *metadata) {
    FILE *original_file = fopen(file_path, "rb");
    if (original_file == NULL) {
        perror("Error opening original file");
        return;
    }

    // Create a temporary file to write new content
    char temp_file_path[256];
    snprintf(temp_file_path, sizeof(temp_file_path), "%s.tmp", file_path);
    FILE *temp_file = fopen(temp_file_path, "wb");
    if (temp_file == NULL) {
        perror("Error creating temporary file");
        fclose(original_file);
        return;
    }

    // Write metadata
    fwrite(metadata, sizeof(char), strlen(metadata), temp_file);
    fwrite("\n", sizeof(char), 1, temp_file);

    // Copy original file contents into the temp file
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, original_file)) > 0) {
        fwrite(buffer, 1, bytes_read, temp_file);
    }

    fclose(original_file);
    fclose(temp_file);

    // Replace the original file with the temporary file
    if (rename(temp_file_path, file_path) != 0) {
        perror("Error replacing the original file with the temporary file");
        remove(temp_file_path);
    }
}

// Helper function to append metadata to the file
void append_metadata(const char *file_path, const char *metadata) {
    FILE *file = fopen(file_path, "ab");
    if (file == NULL) {
        perror("Error opening file for appending");
        return;
    }

    const char *separator = "\n---- METADATA ----\n";
    fwrite(separator, sizeof(char), strlen(separator), file);
    fwrite(metadata, sizeof(char), strlen(metadata), file);
    fwrite("\n", sizeof(char), 1, file);

    fclose(file);
}

// Main function to handle metadata injection
void add_metadata(const char *file_path, const char *metadata) {
    // Check file extension to determine how to inject metadata
    const char *extension = strrchr(file_path, '.');
    if (extension != NULL) {
        if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
            printf("Prepending metadata to JPEG file...\n");
            prepend_metadata(file_path, metadata);
        } else {
            printf("Appending metadata to %s file...\n", extension);
            append_metadata(file_path, metadata);
        }
    } else {
        printf("No file extension found. Appending metadata by default...\n");
        append_metadata(file_path, metadata);
    }
}
