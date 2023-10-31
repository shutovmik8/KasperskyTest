Разработано две программы zip, unzip - для архивации и разархивации. При создании использовалась open source библиотека libzip.  
Архиватор работает в двух режимах: с установкой пароля на архив и без установки.
# Запуск архивации
```bash
./zip [-p] filename [archive.zip]
```
Ключи:
-p: установить пароль на архив

Аргументы:
filename: имя файла 
archive.zip: имя архива

# Запуск разархивации 
```bash
./unzip archive.zip
```
Аргументы:
archive.zip: имя архива

# Сборка
Для сборки выполнить:
```bash
make build
```

# Тесты
Для запуска тестов выполнить:
```bash
make test
```