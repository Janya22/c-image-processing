#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
    /* TODO: Question 1*/
struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
// structure of a single pixel in an image.
// includes 8-bit unsigned integer (ranging from 0 to 255) of red,green and blue components of the pixel's color.

/* image loaded from file. */
struct Image {
    int width;
    int height;
    struct Pixel *pixel;/*pointer to dynamically allocated array of pixel structure*/
    char *output_file; 

/*Structure of an image includes its
 dimensions (height and width), the pixel data and the filname of the output image.*/
    
};

/* Free a struct Image */
void free_image(struct Image *img)
{
    /* TODO: Question 2a */
    // dynamically allocated array of pixels need to be freed
    free(img->pixel);
    // image structure itself is freed
    free(img);
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }


    /* Allocate the Image object, and read the image from the file */
    /* TODO: Question 2b */
    struct Image *img = NULL;/*initialising*/
    //dynamically allocating memory for image object
    img =(struct Image*) malloc(sizeof(struct Image));
      
    //error condition
    if (img == NULL) {
        fprintf(stderr, "File %s could not be read.\n", filename);
        return NULL;//if memory allocation fails.
    }

    //check if the file format is correct
    char format_check[6];//declares array 

    if (fscanf(f, "%5s", format_check ) != 1 || strncmp(format_check, "HSDEC", 5) != 0) { 
    // checks if the first 5 characters from the file is HSDEC or not using the strncmp function.
        fprintf(stderr, "Invalid file format.\n");//prints error message if either fcanf fails to read 1 element or if first 5 char doesnt match HSDEC.
        free(img); //free stops dynamic allocation
        fclose(f);
        return NULL;//if file format is inavlid
    }


    /* Read height and width */
    int width, height;
    if (fscanf(f, "%d %d", &width, &height) != 2) { //we take the memory address. so wheneve we use scanf we use &
        fprintf(stderr, "Error reading height and width from the file.\n");
        fclose(f);
        return NULL;
    }

    //storing width and height in the image object
    img->width =width;
    img->height = height;

    //memory allocation for pixel data
    img->pixel = (struct Pixel *)malloc(width * height * sizeof(struct Pixel)); 

    if (img->pixel == NULL) {
        fprintf(stderr, "File %s could not be read.\n", filename);
        free(img);
        fclose(f);
        return NULL;//if memory allocation fails.
    }

    //loop for interation over pixel data in the file (width — integer number of columns ,height — integer number of rows)
    for (int i = 0; i < width * height; ++i) { 
        if (fscanf(f, "%hhu %hhu %hhu", &(img->pixel[i].red), &(img->pixel[i].green), &(img->pixel[i].blue)) != 3) { 
            // %hhu is used to read an unsigned char (8-bit) value.
            //checks if all 3 values have been read from the file f 
            fprintf(stderr, "Error reading pixel data from the file.\n"); // if there r no 3 inputs ->error
            free(img->pixel);
            free(img);
            fclose(f);
            return NULL;//if error in reading pizel data.
        }
    }
    /* Close the file */
    fclose(f);

    return img;//on success returns pointer to image structure containing image data.
}

/* Write img to file filename. Return true on success, false on error. */
bool save_image(const struct Image *img, const char *filename)
{
    /* TODO: Question 2c */
    FILE *f = fopen(filename, "w");//Opens new file for writing.
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened for writing.\n", filename);
        return false;//if file cannot be opened
    }

    

    /* Writes header containing height and width to the new file */
    fprintf(f, "HSDEC %d %d ", img->width, img->height);

    /* Writes pixel data to the new file */
    //loop for interation over pixel data in the file
    for (int i = 0; i < img->width * img->height; ++i) {
        fprintf(f, "%d %d %d ", img->pixel[i].red, img->pixel[i].green, img->pixel[i].blue);
    }
    
    /* Close the file */
    fclose(f);

    printf("Image is successfully saved to %s.\n", filename);
    return true;//returns true on success.
}

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    /* TODO: Question 2d */

    //memory allocation for new image structure
    struct Image *new =(struct Image *) malloc(sizeof(struct Image));

    if (new == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;//if memory allocation fails
    }

    new->width= source->width;  // Copying width from original image.
    new->height = source->height;    // Copying height from original image.

    //memory allocation for pixel data of copy file.
    new->pixel = (struct Pixel*)malloc(new->height * new->width * sizeof(struct Pixel)); 

    if (new->pixel == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(new);//free new image structure allocated
        return NULL;//if memory allocation fails
    }
// loop iterates over each pixel in the source image and copies its pixel data into the new image.
  for (int i = 0; i < new->width * new->height; ++i) {
    new->pixel[i].red = source->pixel[i].red;
    new->pixel[i].green = source->pixel[i].green;
    new->pixel[i].blue = source->pixel[i].blue;
}

    return new;//return new image structure

}

/* Perform your first task.
 * (TODO: take source image as input and creates a new greyscale version of it.
 * Returns a new struct Image containing the result, or NULL on error. */
struct Image *apply_MONO(const struct Image *source)
{
    // Create a copy of the source image
    struct Image *output_img = copy_image(source);

    //error condition
    if (output_img == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    //for loop to iterate over each pixel in the new copied image to apply grey scale conversion formula to each pixel.
     for (int i = 0; i < output_img->width * output_img->height; i++) {
        
        // calculating greyscale value
        float grey = 0.299 * output_img->pixel[i].red + 0.587 * output_img->pixel[i].green + 0.114 * output_img->pixel[i].blue;

        // Set red, green, and blue components of the pixel to the greyscale value calculated above,
        output_img->pixel[i].red = grey;
        output_img->pixel[i].green = grey;
        output_img->pixel[i].blue = grey;
    }

    return output_img;//returns new struct image containing result.

}

/* Perform your second task.
 * (TODO: the function computes the histogram of the red, green, and blue values used in the output image. )
 * Returns true on success, or false on error. */

bool apply_HIST(const struct Image *source)
{
    if (source == NULL) {
        fprintf(stderr, "Source image is NULL.\n");
        return false;
    }

    // Initializes an array to store the count of each pixel value (0-255).
    int histogram[256] = {0};

    // loop iterates over each pixel and update the histogram counts.
    for (int i = 0; i < source->width * source->height; ++i) {
        histogram[source->pixel[i].red]++;
        histogram[source->pixel[i].green]++;
        histogram[source->pixel[i].blue]++;

    }

    // prints the histogram data.
    for (int i = 0; i < 256; ++i) {
        if (histogram[i] > 0) { // prints values only if it has a count more than 1.
        printf("Value %d: %u pixels\n", i, histogram[i]);
        }
    }

    return true;//on success
}

int main(int argc, char *argv[])
{
    /* Check command-line arguments */
    //if there is an even number of arguments or if it is less than 3 prints an error
    if (argc < 3 || argc % 2 != 1) {
        fprintf(stderr, "Usage: process INPUTFILE1 OUTPUTFILE1 [INPUTFILE2 OUTPUTFILE2 ...]\n");
        return 1;
    }

    //counts number of source files
    int img_count = (argc - 1) / 2;

    //allocates memory for an array of pointers to images
    struct Image **images = malloc(img_count * sizeof(struct Image*));
    if (images == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;//if allocations fails
    }

    // Load all input images into memory and set output filenames.
    for (int i = 0; i < img_count; ++i) {
        images[i] = load_image(argv[1 + i * 2]);
        if (images[i] == NULL) {
            // Free already loaded images and exit if any image fails to load
            for (int j = 0; j < i; ++j) {
                free_image(images[j]);
            }
            free(images);
            return 1;
        }
        // Allocate memory and save the output filename in the struct
        images[i]->output_file = argv[2 + i * 2];
    }
//processing each image
for (int i = 0; i < img_count; ++i) {
    //apply MONO to covert the images to greyscale
    struct Image *out_img = apply_MONO(images[i]);

    if (out_img == NULL) {
        fprintf(stderr, "First process failed.\n");
        for (int j = 0; j < img_count; ++j) {
            free_image(images[j]);
        }
        free(images);
        return 1;//return1 on error
    }

    if (!apply_HIST(out_img)) {
        fprintf(stderr, "Second process failed.\n");
        free_image(out_img);
        continue; // Continue  to process the next image.
    }

    //save processed image
    if (!save_image(out_img, images[i]->output_file)) {
        fprintf(stderr, "Saving image to %s failed.\n", argv[2]);
        free_image(out_img);
        continue; 
    }
    free_image(out_img); // Free the processed image after saving.
}



    // Cleanup
    for (int i = 0; i < img_count; ++i) {
        free_image(images[i]);//free memory for each images
    }
    free(images);//free memory for the array of images

    return 0;//return 0 on sucess
}
