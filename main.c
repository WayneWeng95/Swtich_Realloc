#include "Comparison.c"

int main(int argc, char *argv[]) // 4GB
{

    pthread_t tid;

    char *buffer1, *buffer2, *buffer3;

    char *first_para = argv[1];
    looping = atoi(first_para);

    char *second_para = argv[2];
    int thread_counter = atoi(second_para);

    char *third_para = argv[3];
    int mode_selection = atoi(third_para);

    pFile = fopen(filePath, "r");
    fseek(pFile, 0, SEEK_END);
    fSize = ftell(pFile);
    rewind(pFile);
    buffer = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer, 1, fSize, pFile);

    buffer1 = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer1, 1, fSize, pFile);
    buffer2 = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer2, 1, fSize, pFile);
    buffer3 = (char *)malloc(sizeof(char) * fSize);
    testBuff = fread(buffer3, 1, fSize, pFile);

    sleep(3);


    for (int i = 0; i < thread_counter; i++)
    {

        if (mode_selection == 0)
        {
            pthread_create(&tid, NULL, malloc_thread, NULL); // mode 0 for malloc
        }
        // else if (mode_selection == 1)
        // {
        //     pthread_create(&tid, NULL, mmaprelease_thread, NULL); // mode 1 for mmap release
        // }
        else if (mode_selection == 1)
        {
            pthread_create(&tid, NULL, mmap_thread, NULL); // mode 1 for mmap
        }
        else if (mode_selection == 2)
        {
            pthread_create(&tid, NULL, _malloc_thread, NULL); // mode 2 for hybird
        }
        else
        {
            printf("Wrong selection number");
        }
    }
    // https://stackoverflow.com/questions/58804022/how-to-resize-dev-shm
    pthread_exit(NULL);
}