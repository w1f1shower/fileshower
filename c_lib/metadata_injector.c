#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libexif/exif-data.h>
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

// Function to add EXIF metadata to a JPEG file
void add_metadata_to_jpeg(const char *file_path, const char *metadata) {
    ExifData *exif_data = exif_data_new();
    if (!exif_data) {
        fprintf(stderr, "Error: Failed to create EXIF data.\n");
        return;
    }

    // Add metadata to the EXIF comment tag
    ExifEntry *entry = exif_data_get_entry(exif_data, EXIF_TAG_USER_COMMENT);
    if (!entry) {
        entry = exif_content_get_entry(exif_data->ifd[EXIF_IFD_0], EXIF_TAG_USER_COMMENT);
        if (!entry) {
            exif_data_free(exif_data);
            fprintf(stderr, "Error: Failed to create EXIF entry.\n");
            return;
        }
    }

    // Write the metadata
    strncpy((char *)entry->data, metadata, entry->size);

    // Save EXIF data to buffer
    unsigned char *exif_buffer = NULL;
    unsigned int exif_buffer_size = 0;
    exif_data_save_data(exif_data, &exif_buffer, &exif_buffer_size);

    // Write buffer to file
    FILE *file = fopen(file_path, "wb");
    if (file) {
        fwrite(exif_buffer, 1, exif_buffer_size, file);
        fclose(file);
    } else {
        fprintf(stderr, "Error: Unable to write to file %s\n", file_path);
    }

    // Free resources
    free(exif_buffer);
    exif_data_free(exif_data);

    printf("Metadata successfully added to JPEG file: %s\n", file_path);
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
