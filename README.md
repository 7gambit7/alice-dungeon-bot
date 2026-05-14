# alice-dungeon-bot

Симуляция бота Алисы, обходящего подземелье, собирающего ресурсы и расходующего еду.

Идеи по улучшению алгоритма - в [`improvements.md`](improvements.md).

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Бинарник появится по пути `build/task`.

### Опции CMake

| Опция                 | Дефолт   | Что делает                                            |
|-----------------------|----------|-------------------------------------------------------|
| `CMAKE_BUILD_TYPE`    | `Release`| Тип сборки                                            |
| `WARNINGS_AS_ERRORS`  | `OFF`    | `-Werror` - варнинги становятся ошибками              |
| `ENABLE_SANITIZERS`   | `OFF`    | Сборка с AddressSanitizer + UndefinedBehaviorSanitizer|

Пример сборки с санитайзерами:

```bash
cmake -S . -B build-san -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build-san
```

## Запуск

```bash
./build/task path/to/input.txt
```

Бот пишет лог действий в `result.txt` в текущей директории.

## Тесты

```bash
ctest --test-dir build --output-on-failure
```

Или напрямую без CMake:

```bash
bash tests/run_tests.sh
```

Покрытие: пример из ТЗ, негативные кейсы парсера (10 шт.), ручные кейсы (минимум еды, цепь), стресс тест N=255.

## Структура

```
src/
  domain.h / domain.cpp        типы предметной области
  parser.h / parser.cpp        парсер входного файла + валидация
  bot.h                        интерфейс IBot (для подмены алгоритма)
  alice_bot.h / alice_bot.cpp  реализация бота Алисы
  main.cpp                     entry point
tests/
  *.in / *.expected            тестовые пары
  run_tests.sh                 раннер
  gen_stress.sh                генератор стресс-теста
.github/workflows/ci.yml       CI: build (gcc+clang), tests, sanitizers, clang-format
```

## CI

GitHub Actions прогоняет на каждый push/PR:
- сборку на gcc и clang с `-Werror`
- все тесты (`ctest`)
- сборку с ASan + UBSan и тесты
- проверку форматирования через `clang-format-22`

## Стиль кода

`clang-format-22` с базой `Google` (см. [`.clang-format`](.clang-format)). Перед коммитом:

```bash
clang-format-22 -i src/*.cpp src/*.h
```