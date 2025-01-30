# Тестовое задание

### Сборка проекта

* Скомпилировать весь проект:
    ```
    make all
    ```
* или просто
    ```
    make
    ```

* Отдельные таргеты Makefile:
    * `all` - весь проект
    * `mylib` - динамическая библиотека
    * `program1` - Програма №1
    * `program2` - Програма №2
    * `clean` - очистка временных файлов
    * `rebuild` - clean + all

### Запуск программы

* Исполняемые файлы:
    * ./app1 - Программа  №1
    * ./app2 - Программа  №2


### Описание

* Тестовое задание для стажера на позицию «Программист на языке C++»
* Проект состоит из трех частей:
    1. Библиотека `mylib`, содержащая три функции: `func_1` (обработка строки), `func_2` (сумма числовых элементов строки), `func_3` (проверка длины строки)
    2. Программа №1. Двупоточная программа. Принимает строку ввода пользователя, обрабатывает ее и отправляет в Программу №2. Выступает в роли клиента
    3. Программа №2. Однопоточная программа. Принимает данные от Программы №1 и проверяет их через `func_3`. Выступает в роли сервера 