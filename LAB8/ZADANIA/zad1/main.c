#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_LINE_LEN 256

int **image;
int **negative_image;
int n, w, h;

long int calculate_microseconds(struct timeval start_time, struct timeval end_time){
    return 1000000 * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_usec - start_time.tv_usec;
}

void load_image(char *path){
    FILE *read_file = fopen(path, "r");
    if(read_file == NULL){
        printf("CANT OPEN READ IMAGE FILE\n");
        exit(1);
    }
    char *buff = calloc(MAX_LINE_LEN, sizeof(char));
    fgets(buff, MAX_LINE_LEN, read_file); // skip first line containing "P2"
    fgets(buff, MAX_LINE_LEN, read_file); // skip second line containing description
    fgets(buff, MAX_LINE_LEN, read_file); // get width and height of image
    sscanf(buff, "%d %d\n", &w, &h);
    fgets(buff, MAX_LINE_LEN, read_file); // get max pixel value in gray scale
    int max_pixel_value;
    sscanf(buff, "%d\n", &max_pixel_value);
    if(max_pixel_value != 255){
        printf("MAX GRAY VALUE MUST BE SET TO 255. CONSIDER CHANGING THE IMAGE\n");
        exit(1);
    }
    image = calloc(h, sizeof(int *));
    for(int i = 0; i < h; i++)
        image[i] = calloc(w, sizeof(int));
    int pixel;
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            fscanf(read_file, "%d", &pixel);
            image[i][j] = pixel;
        }
    }
    fclose(read_file);
}

void save_image(char *path){
    FILE *write_file = fopen(path, "w");
    if(write_file == NULL){
        printf("CANT OPEN WRITE IMAGE FILE\n");
        exit(1);
    }
    fprintf(write_file, "P2\n");
    fprintf(write_file, "%d %d\n", w, h);
    fprintf(write_file, "255\n");
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++)
            fprintf(write_file, "%d ", negative_image[i][j]);
        fprintf(write_file, "\n");
    }
    fclose(write_file);
}

void *numbers_thread(void *arg){
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    int index = *(int *)arg; // (int *) casts void pointer to int pointer and later *(int *) gets value from that address
    int start_pixel_value = index * 256 / n;
    int end_pixel_value = (index + 1) * 256 / n;
    if(index == n - 1)
        end_pixel_value = 256;
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            if(image[i][j] >= start_pixel_value && image[i][j] < end_pixel_value)
                negative_image[i][j] = 255 - image[i][j];
        }
    }
    gettimeofday(&end_time, NULL);
    long int *difference = malloc(sizeof(long int));
    *difference = calculate_microseconds(start_time, end_time);
    pthread_exit(difference);
}

void *block_thread(void *arg){
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    int index = *(int *)arg; // (int *) casts void pointer to int pointer and later *(int *) gets value from that address
    int start_pixel = index * w / n;
    int end_pixel = (index + 1) * w / n - 1;
    for(int i = 0; i < h; i++){
        for(int j = start_pixel; j <= end_pixel; j++)
            negative_image[i][j] = 255 - image[i][j];
    }
    gettimeofday(&end_time, NULL);
    long int *difference = malloc(sizeof(long int));
    *difference = calculate_microseconds(start_time, end_time);
    pthread_exit(difference);
}

int main(int argc, char *argv[]){
    if(argc != 5){
        printf("WRONG NUMBER OF ARGUMENTS\n");
        exit(1);
    }
    n = atoi(argv[1]);
    char *type = argv[2];
    char *file_in_path = argv[3];
    char *file_out_path = argv[4];
    if(strcmp(type, "numbers") != 0 && strcmp(type, "block") != 0){
        printf("WRONG TYPE ARGUMENT\n");
        exit(1);
    }
    load_image(file_in_path);
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    negative_image = calloc(h, sizeof(int *));
    for(int i = 0; i < h; i++)
        negative_image[i] = calloc(w, sizeof(int));
    pthread_t *threads = calloc(n, sizeof(pthread_t));
    int *indexes = calloc(n, sizeof(int));
    for(int i = 0; i < n; i++){
        indexes[i] = i;
        if(strcmp(type, "numbers") == 0)
            pthread_create(&threads[i], NULL, &numbers_thread, &indexes[i]);
        else
            pthread_create(&threads[i], NULL, &block_thread, &indexes[i]);
    }
    FILE *times_file = fopen("Times.txt", "a");
    if(times_file == NULL){
        printf("CANT OPEN TIMES FILE\n");
        exit(1);
    }
    printf("Type: %s, Threads: %d, Width: %d, Height: %d\n", type, n, w, h);
    fprintf(times_file, "Type: %s, Threads: %d, Width: %d, Height: %d\n", type, n, w, h);
    long int *val;
    for(int i = 0; i < n; i++){
        pthread_join(threads[i], (void **)&val);
        printf("Thread: %3d, Time: %6ld[us]\n", i, *val);
        fprintf(times_file, "Thread: %3d, Time: %6ld[us]\n", i, *val);
    }
    gettimeofday(&end_time, NULL);
    printf("TOTAL, %6ld[us]\n", calculate_microseconds(start_time, end_time));
    fprintf(times_file, "TOTAL, %6ld[us]\n\n", 1000000 * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_usec - start_time.tv_usec);
    fclose(times_file);
    save_image(file_out_path);
    for(int i = 0; i < h; i++){
        free(image[i]);
        free(negative_image[i]);
    }
    free(image);
    free(negative_image);
    free(threads);
    free(indexes);
    return 0;
}