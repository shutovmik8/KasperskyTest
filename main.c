#include <libgen.h>
#include <fcntl.h>
#include <stdio.h>
#include <zip.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


int zip(int argc, char* argv[])
{
	if (argc < 2)  //Проверка на корректность запуска программы
	{
		fprintf(stderr, "File name is not provided!\n");
		return 1;
	}
	int pass = 0;
	if (strcmp(argv[1], "-p") == 0) pass = 1; //Определение режима работы
	if ((pass) && (argc < 3)) //Проверка на корректность запуска программы
	{
		fprintf(stderr, "File name is not provided!\n");
		return 1;
	}
	int N;
	int fd;
	if (pass)
	{
		if ((fd = open(argv[2], O_RDONLY)) == -1)  //Открываем файл, который хотим заархивировать, проверяем на успешность открытия файла
		{
			fprintf(stderr, "No such file!\n");
			return 1;
		}
	}
	else
	{
		if ((fd = open(argv[1], O_RDONLY)) == -1) 
		{
			fprintf(stderr, "No such file!\n");
			return 1;
		}
	}
	long size = lseek(fd, 0, SEEK_END); 
	void *p = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0); //Маппируем файл в память в соответствии с его размером
	zip_error_t err;
	zip_source_t *src = zip_source_buffer_create(0, 0, 0, &err);
	zip_t *archive = zip_open_from_source(src, ZIP_CREATE | ZIP_TRUNCATE, &err); //Создаем архив
	zip_source_t *src_file = zip_source_buffer_create(p, size, 0, &err); 
	if (pass)
	{
		zip_file_add(archive, argv[2], src_file, ZIP_FL_OVERWRITE); //Добавляем файл в архив с паролем
		char pass[256];
		printf("Enter password: "); 
		fgets(pass, 256, stdin);
		if(strlen(pass) >= 255) return 1;
		pass[strlen(pass) - 1] = '\0';
		zip_file_set_encryption(archive, 0, ZIP_EM_AES_256, pass); //Устанавливаем пароль
		for (int i = 0; i < 256; i++) pass[i] = '0';
	}
	else zip_file_add(archive, argv[1], src_file, ZIP_FL_OVERWRITE); //Добавляем файл в архив
	zip_source_keep(src);
	zip_source_keep(src_file);
	zip_close(archive); //Закрываем архив
	close(fd);
	char *buf = (char *)malloc(1024);
	zip_source_open(src);
	if ((pass) && (argc > 3)) fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);//Создаем файл на диске и записываем его чанками
	else if ((!pass) && (argc > 2)) fd = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		else fd = open("archive.zip", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
	while ((N = zip_source_read(src, buf, 1024)) != 0) write(fd, buf, N); 
	munmap(p, size);
	close(fd);
	free(buf);
}

int unzip(int argc, char* argv[])
{
	if (argc < 2) //Проверка на корректность запуска программы
	{
		fprintf(stderr, "Archive name is not provided!\n");
		return 1;
	}
	int fd;
	if ((fd = open(argv[1], O_RDONLY)) == -1) //Открываем архив
	{
		fprintf(stderr, "No such archive!\n");
		return 1;
	}
	zip_error_t err;
	long size = lseek(fd, 0, SEEK_END);
	long N;
	int rename = 0;
	lseek(fd, 0, SEEK_SET); 
	const void *p = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0); //Маппируем архив в память
	zip_source_t *src = zip_source_buffer_create(p, size, 0, &err);
	zip_t *archive = zip_open_from_source(src, 0, &err); //Открываем архив
	if (zip_get_num_entries(archive, ZIP_FL_UNCHANGED) != 1) //Проверка, что в архиве один файл
	{
		fprintf(stderr, "Only single-file archives are allowed!\n");
		return 1;
	}
	const char *filename = zip_get_name(archive, 0, 0);
	zip_stat_t sb;
	zip_stat_index(archive, 0, 0, &sb);
	int encr = sb.encryption_method;
	zip_file_t *my_file;
	if (encr == ZIP_EM_NONE) my_file = zip_fopen(archive, filename, 0); //Проверка наличия пароля на архиве
	else
	{
		char pass[256];
		printf("Enter password: "); 
		fgets(pass, 256, stdin);
		if(strlen(pass) >= 255) return 1;
		pass[strlen(pass) - 1] = '\0';
		printf("%s\n", pass);
		my_file = zip_fopen_encrypted(archive, filename, 0, pass); //Разархивация
		if (!my_file)
		{
			fprintf(stderr, "Password is incorrect!\n");
			return 1;
		}
	}
	if (access(filename, F_OK) == 0) //Проверка на наличие файла с совпадающим именем
	{
		rename = 1;
		printf("Overwrite the file? (y/N): "); 
		char ans[256];
		fgets(ans, 256, stdin);
		if(strlen(ans) >= 255) return 1;
		if (ans[0] != 'y')
		return 0;
	}
	char *buf = (char *)malloc(1024);
	int fd1 = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //Запись файла на диск
	while ((N = zip_fread(my_file, buf, 1024)) != 0) write(fd1, buf, N);
	close(fd1);
	close(fd);
	free(buf);
}

int main(int argc, char* argv[])
{
	if (strcmp(basename(argv[0]), "zip") == 0) return zip(argc, argv);
	else if (strcmp(basename(argv[0]), "unzip") == 0) return unzip(argc, argv);
	return 1;
}
