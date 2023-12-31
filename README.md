В этом репозитории находятся три задачи, выполненные в рвмках тестового задания.

Первое задание выполнено на языке С++. Должно работать кроссплатформенно, но проверял только на Linux.
Применил алгоритм топологической сортировки обхода графа в глубину (DFS).
Для проверки задания создал несколько катологов и текстовых файлов.
Сложнее всего было отследить циклические ссылки, и литературы по этой теме очень мало. Но вроде бы все отслеживается и цепочка с циклом выводится в консоль.

### Необходимо для сборки

- `CMake 3.20+`

### Сборка

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Запуск

- `-o <выходной файл>` по умолчанию: task1-result.txt
- `-d <директория>` по умолчанию: текущая директория

```
cd build
./txtparser -d ../testbed
```

Второе задание состоит из клиентской и серверной части. Клиентская часть написана на ванильном JavaScript, серверная - на С++. Сборкой клиентской части я не стал заниматься, просто подключил js к html-файлу и применил type=module. Запускал с помощью плагина Live Server. По-хорошему надо бы было webpack развернуть, но я просто не успевал, задание довольно объемное, многое пришлось изучать (это здорово), но и многое хотелось бы усовершенствовать.
Для выполнения серверной части использовал библиотеки `nlohmann-json` и `sqlite3`.

### Необходимо для сборки

- `CMake 3.20+`
- `pkg-config`
- `nlohmann-json`
- `sqlite3`

### Сборка

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Запуск

- `-p <порт>` по умолчанию: 8080
- `-d <путь к базе данных>` по умолчанию: students.db

```
cd build
./students
```

Третья задача, как и вторая, состоит из клиентской и серверной части. Клиентская часть написана на js и не требует сборки. Серверная часть используется исключительно для преодаления защиты cors. Пользуясь разрешением использовать для сервера любой язык програмирования, сделал серверную часть на node.js с применением express.js.

### Необходимо для сборки

- `node 18`
- `npm 9`

### Сборка

```
cd server
npm i
```

### Запуск

```
npm start
```
