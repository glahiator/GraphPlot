# GraphPlot
Программа для визуального контроля и диагностики технических модулей, работа которых оценивается различного рода датчиками (давления, усилия, температуры или расхода). Датчики могут быть как собственными разработками, так существующими и широко распространнеными программируемыми логическими контроллерами (например Siemens). В первом случае протокол взаимодействия необходимо разрабатывать как для передающего устройства так и для GraphPlot. Во втором случае оптимальным является интеграция в уже существующие протоколы отправки данных с датчиков, описанных в [руководствах пользователя](https://www.simecs.ru/e-catalog/).


## Возможности
- [X] Построение в реалтайм графиков входных данных;
- [X] Взаимодействие с графиками в графическом интерфейсе пользователя: скрытие ненужных графиков, линий на них, корректировка диапазонов выводимой величины;
- [X] Данные принимаются через сокеты в виде UDP-пакетов;
- [X] Предполагается, что порядок данных при отправке и приеме является согласованным;
- [X] Если необходимо принимать данные по протоколу S7 с ПЛК фирмы Siemens, то в директории lib/ добавлена библиотека [s7lib](https://snap7.sourceforge.net/).

### Демо
Входные данные генерируются генератором случайных чисел. Скрывать можно как сам график в окне, так и отдельно взятые линии на графике.
<div align="center">
  <img src="https://user-images.githubusercontent.com/25270845/211928312-7ed022a1-edd3-4ada-9b1c-e173797a1ebf.gif" width="600"></img>
</div>

## ToDo 
- [ ] Обработка исключительных ситуаций.
- [ ] Подробная документация.
- [ ] Поддержка логгирования.
- [ ] Добавление дополнительных протоколов приема данных с датчиков.

### Требования
- Qt5
- установленный модуль QtCharts

### Сборка проекта
В проекте используется система сборки qmake, что должно избавить от каких-либо трудностей при сборке проекта.
- Клонировать репозиторий.
```sh 
git clone https://github.com/glahiator/GraphPlot.git
```
- Открыть GraphPlot.pro в редакторе QtCreator
- Добавить каталог build в репозитории в качестве каталога сборки проекта 

## Copyright & License
- Copyright (©) 2022 by [glahiator](https://github.com/glahiator)
- Licensed under the terms of the [GNU Lesser General Public License v3.0](./LICENSE.txt)
