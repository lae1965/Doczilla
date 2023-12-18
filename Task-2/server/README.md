### Использованные библиотеки
- `nlohmann-json`
- `sqlite3`

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
