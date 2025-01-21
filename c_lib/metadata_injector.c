#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>
#include <hpdf.h>

// Function prototypes
void add_metadata_to_jpeg(const char *file_path, const char *metadata);
void add_metadata_to_pdf(const char *file_path, const char *metadata);

// Helper function to determine file extension
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot ? dot + 1 : "";
}

// Main function to add metadata
void add_metadata(const char *file_path, const char *metadata) {
    const char *ext = get_file_extension(file_path);
    
    if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0) {
        add_metadata_to_jpeg(file_path, metadata);
    } else if (strcasecmp(ext, "pdf") == 0) {
        add_metadata_to_pdf(file_path, metadata);
    } else {
        fprintf(stderr, "Unsupported file format: %s\n", ext);
    }
}

void add_metadata_to_jpeg(const char *file_path, const char *metadata) {
    // Open the JPEG file
    FILE *file = fopen(file_path, "rb+");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", file_path);
        //return 1;
    }

    // Load the EXIF data from the file (or create new EXIF data)
    ExifData *exif_data = exif_data_new();
    if (!exif_data) {
        fprintf(stderr, "Error: Failed to create EXIF data object.\n");
        fclose(file);
        //return 1;
    }

    // Ensure the EXIF IFD for metadata exists
    ExifContent *ifd_exif = exif_data->ifd[EXIF_IFD_EXIF];
    if (!ifd_exif) {
        fprintf(stderr, "Error: Failed to initialize EXIF IFD.\n");
        exif_data_unref(exif_data);
        fclose(file);
        //return 1;
    }

    // Find or create the EXIF entry for the UserComment tag
    ExifEntry *entry = exif_content_get_entry(ifd_exif, EXIF_TAG_USER_COMMENT);
    if (!entry) {
        entry = exif_entry_new();
        if (!entry) {
            fprintf(stderr, "Error: Failed to create EXIF entry.\n");
            exif_data_unref(exif_data);
            fclose(file);
         //   return 1;
        }

        // Initialize the new entry with the UserComment tag
        exif_entry_initialize(entry, EXIF_TAG_USER_COMMENT);
        exif_content_add_entry(ifd_exif, entry);
        exif_entry_unref(entry); // Unreference after adding to the content
    }

    // Ensure the entry has enough space to store metadata
    size_t metadata_length = strlen(metadata);
    if (metadata_length + 8 > entry->size) { // 8 bytes reserved for encoding info
        exif_entry_fix(entry);
        free(entry->data);
        entry->data = malloc(metadata_length + 8);
        entry->size = metadata_length + 8;
    }

    // Set the UserComment data (prefix with ASCII encoding info)
    memcpy(entry->data, "\x41\x53\x43\x49\x49\x00\x00\x00", 8); // ASCII encoding header
    memcpy(entry->data + 8, metadata, metadata_length);

    // Save the modified EXIF data back to the file
    unsigned char *output_data = NULL;
    unsigned int output_size = 0;
    exif_data_save_data(exif_data, &output_data, &output_size);

    if (output_data) {
        // Rewrite the file with updated EXIF metadata
        rewind(file); // Go to the start of the file
        fwrite(output_data, 1, output_size, file);
        free(output_data);
    } else {
        fprintf(stderr, "Error: Failed to save EXIF data to file.\n");
        exif_data_unref(exif_data);
        fclose(file);
        //return 1;
    }

    // Cleanup
    exif_data_unref(exif_data);
    fclose(file);

    printf("Metadata successfully injected into %s\n", file_path);
    //return 0;
}

// Function to add metadata to a new PDF file
void add_metadata_to_pdf(const char *file_path, const char *metadata) {
    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    if (!pdf) {
        fprintf(stderr, "Error: Failed to create PDF document.\n");
        return;
    }

    // Set PDF metadata
    HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, "Your Name");
    HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, "Injected Metadata");
    HPDF_SetInfoAttr(pdf, HPDF_INFO_SUBJECT, metadata);

    // Add a page to the PDF
    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), 12);
    HPDF_Page_TextOut(page, 50, 800, "This is a sample PDF with metadata.");
    HPDF_Page_EndText(page);

    // Save the PDF to a file
    HPDF_SaveToFile(pdf, file_path);

    // Clean up
    HPDF_Free(pdf);

    printf("Metadata successfully added to PDF file: %s\n", file_path);
}
