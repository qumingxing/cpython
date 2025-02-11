#include "decrypt_source_file.h"

static unsigned char *str2hex(char *str);
static void encrypt_buf(char *raw_buf, char **encrpy_buf, int len);
static void decrypt_buf(char *raw_buf, char **encrpy_buf, int len);
static int decrypt_open(char *filename);
#define BLOCK_SIZE 16

FILE *d_open(char *filename, const char *modes)
{
    // qumingxing
    printf("ppython d_open ...%s\n", filename);
    fflush(stdout); // 确保输出立即显示

    if (strcmp(modes, "rb") != 0)
    {

        return fopen(filename, modes);
    }

    FILE *ret = NULL;
    int fd;
    fd = decrypt_open(filename);
    if (fd < 0)
    {
        // perror("error");
        return ret;
    }
    ret = fdopen(fd, modes);

    return ret;
}

int dopen(const char *pathname, int flags, mode_t mode)
{
#ifdef O_CLOEXEC
    if (flags != (O_RDONLY | O_CLOEXEC))
#else
    if (flags != O_RDONLY)
#endif
        return open(pathname, flags, mode);
    else
        return decrypt_open(pathname);
}

static int decrypt_open(char *filename)
{

    int ret = -1;
    int original_file_fd = open(filename, O_RDONLY);

    int size = 0;
    char buf[64] = {0};
    char filehead[512] = {0};
    // char *de_buf = (char *)malloc(64);

    char template[] = "decrypt-file-XXXXXX";
    ret = mkstemp(template);

    if (ret == -1)
    {
        close(original_file_fd);
        return -1;
    }

#ifdef _NO_DECRYPT_FILE_OUTPUT
    unlink(template);
#endif

    // 读取文件头
    // size = read (original_file_fd, filehead, sizeof(HEADINFO)-1);

    if (size < 0)
    {
        close(original_file_fd);
        close(ret);
        return -1;
    }
    filehead[size] = 0;

    //char *suffix = "a_enc.py";
    const char *suffixes[] = {"app_enc.py", "x_enc.py","transformers_test_enc.py","safetensors/torch.py","safetensor_test_enc.py"}; 
    size_t num_suffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    size_t filename_len = strlen(filename);
    //size_t suffix_len = strlen(suffix);
    int has_valid_suffix = 0;
    for (size_t i = 0; i < num_suffixes; i++) {
        size_t suffix_len = strlen(suffixes[i]);
        if (filename_len >= suffix_len && 
            strcmp(filename + filename_len - suffix_len, suffixes[i]) == 0) {
            has_valid_suffix = 1;
            break;
        }
    }
    ///

    // 普通文件直接打开
    // if(strcmp(filehead, HEADINFO) != 0) {
    //if (strcmp(filename + filename_len - suffix_len, suffix) != 0)
    if(!has_valid_suffix)
    {
        close(ret);
        ret = original_file_fd;
        lseek(ret, 0, SEEK_SET);
        return ret;
    }

    printf("ppython 加密文件开始打开 ...%s\n", filename);
    fflush(stdout); // 确保输出立即显示

    long file_size = lseek(original_file_fd, 0, SEEK_END); // 将文件指针移动到文件末尾

    lseek(original_file_fd, 0, SEEK_SET); // 将文件指针移动回文件开头

    char *buffer = (char *)malloc(file_size);
    //char *buffer = (char *)malloc(file_size + 1);
    char *de_buf = (char *)malloc(file_size + 1);

    size_t bytes_length = read(original_file_fd, buffer, file_size);
    decrypt_buf(buffer, &de_buf, bytes_length);

    //int actual_size = strlen(de_buf);
    int actual_size = bytes_length;


    //unsigned char padding_length = de_buf[actual_size - 1];
    unsigned char padding_length = de_buf[file_size-1];
    actual_size -= padding_length;

    // qumingxing
    printf("ppython 解密内容 ...%s<<<<<<\n", de_buf);
    printf("ppython 解密前内容长度 ...%d...解密后内容长度 ...%d...pkcs7最后一位长度...%d...实际长度 ...%d...<<<<<<\n", bytes_length, file_size-1, padding_length, actual_size);
    fflush(stdout); // 确保输出立即显示

    if (write(ret, de_buf, actual_size) == -1)
    {
        close(original_file_fd);
        close(ret);
        return -1;
    }
    free(de_buf);
    free(buffer);
    lseek(ret, 0, SEEK_SET);
    // free(de_buf);
    close(original_file_fd);

    // qumingxing
    printf("解密结束 ...<<<<<<\n");
    fflush(stdout); // 确保输出立即显示
    return ret;
}

static unsigned char *str2hex(char *str)
{
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (char *)malloc(str_len / 2);
    for (i = 0; i < str_len; i = i + 2)
    {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}

static void decrypt_buf(char *raw_buf, char **encrpy_buf, int len)
{
    AES_KEY aes;
    unsigned char *key = str2hex(KEY);
    unsigned char *iv = str2hex(IV);
    AES_set_decrypt_key(key, 128, &aes);
    // AES_cbc_encrypt (raw_buf, *encrpy_buf, len, &aes, iv, AES_DECRYPT);
    AES_cbc_encrypt(raw_buf, (unsigned char *)*encrpy_buf, len, &aes, iv, AES_DECRYPT);

    free(key);
    free(iv);
}