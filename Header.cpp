#include "Header.h"

bool Value(const pair<char, float>& a, const pair<char, float>& b) 
{
    return a.second > b.second;
}

unordered_map<char, float> estimate_proba(const vector<char>& bytes)
{
    // Подсчитываем количество встреч каждого символа
    for (auto iter = bytes.begin(); iter != bytes.end(); iter++)
    {
        char byte = *iter;
        probabilities[byte]++;
    }
    for (auto iter = probabilities.begin(); iter != probabilities.end(); iter++)
    {
        iter->second /= bytes.size();
    }
    return probabilities;
}

//кодовая таблица. принимает массив из вероятностей, выдает набор бит для символов.
unordered_map<char, string> build_code(
    const vector<pair<char, float>>& sorted_freq,
    int start_pos,
    int end_pos,
    const string& prefix = "" // Путь в дереве, который мы строим
) 
{
    unordered_map<char, string> code_map;

    // Базовый случай: если позиции символов совпали, это лист дерева
    if (start_pos == end_pos) {
        code_map[sorted_freq[start_pos].first] = prefix;
        return code_map;
    }

    // Подсчет суммы частот в последовательности
    float total_freq = 0;
    for (int i = start_pos; i <= end_pos; ++i) {
        total_freq += sorted_freq[i].second;
    }
    float half_freq = total_freq / 2;

    // Поиск середины
    float sum_freq = 0;
    int mid = start_pos;
    while (mid < end_pos && (sum_freq + sorted_freq[mid].second) < half_freq) {
        sum_freq += sorted_freq[mid].second;
        ++mid;
    }

    // Рекурсия левая ветка дерева с добавлением "1" к префиксу
    auto left_codes = build_code(sorted_freq, start_pos, mid, prefix + "1");
    code_map.insert(left_codes.begin(), left_codes.end());

    // Рекурсия правая ветка дерева с добавлением "0" к префиксу
    auto right_codes = build_code(sorted_freq, mid + 1, end_pos, prefix + "0");
    code_map.insert(right_codes.begin(), right_codes.end());

    return code_map;
}


//кодирование исходного текста
vector<bool> encode(const vector<char>& bytes, const unordered_map<char, vector<bool>>& table)
{
    vector<bool> encoded;
    for (auto iter = bytes.begin(); iter != bytes.end(); ++iter)
    {
        const auto& bits = table.at(*iter); // Получаем битовое представление символа из таблицы
        encoded.insert(encoded.end(), bits.begin(), bits.end()); // Добавляем битовое представление в результат
    }
    return encoded;
}

//декодирование бинарного файла обратно в исходный текст
vector<char> decode(const vector<bool>& bits, const unordered_map<char, vector<bool>>& table)
{
    vector<char> decoded;// будет содержать декодированные символы

    //Создаем обратную таблицу 
    unordered_map<vector<bool>, char> reverse_table;
    for (const auto& t : table) {
        reverse_table[t.second] = t.first;
    }
    //вектор текущих битов
    vector<bool> current_bits; //будет хранить текущую последовательность битов из закодированного текста.
    for (bool b : bits) {
        current_bits.push_back(b);
        // Проверяем, есть ли соответствие в обратной таблице
        //если есть записываем соотв символ и очищаем текущие
        if (reverse_table.count(current_bits))
        {
            decoded.push_back(reverse_table[current_bits]);
            current_bits.clear();
        }
    }
    return decoded;
}

//функции из приложения 2///////////////////////////////////////////
inline void append(vector<bool>& body, const vector<bool>& tale)
{
    body.insert(body.end(), tale.begin(), tale.end());
}
inline void append(vector<bool>& body, char tale)
{
    // Цикл по битам в добавляемым байте.
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        // Добавление текущего бита.
        body.emplace_back((tale >> (7 - bit_idx)) & 0x01);
    }
}

inline vector<bool> table2bits(const unordered_map<char, vector<bool>>& table)
{
    // Буфер для записи результата.
    vector<bool> bits;
    // Перебор всех записей в кодовой таблице.
    for (auto item : table) 
    {
        // Добавление значения символа.
        append(bits, item.first);
        // Добавление длины неравномерного кода.
        append(bits, (char)item.second.size());
        // Добавление кода.
        append(bits, item.second);
    }
    return bits;
}

inline char get_byte(const vector<bool>& bits, unsigned int cur_pos)
{
    // Буфер для записи результата. По умолчанию значения всех бит устанавливаются в FALSE.
    char byte = 0x00;
    // Цикл по битам в байте.
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        // Если текущий бит равен TRUE, то его надо выставить в TRUE.
        if (bits[cur_pos + bit_idx]) byte |= (0x01 << (7 - bit_idx));
    }
    return byte;
}

inline unordered_map<char, vector<bool>> bits2table(const vector<bool>& bits)
{
    // Буфер для записи результата.
    std::unordered_map<char, vector<bool>> table;
    // Номер текущего бита.
    int cur_pos = 0;
    // Перебор всех записей в кодовой таблице.
    while (cur_pos < bits.size()) {
        // Получение символа.
        char character = (char) get_byte(bits, cur_pos);
        cur_pos += 8;
        // Получение длины кода, соответствующего символу.
        char code_length = get_byte(bits, cur_pos);
        cur_pos += 8;
        // Получение кода, соответствующего символу.
        std::vector<bool> code;
        for (int bit_idx = 0; bit_idx < code_length; bit_idx++) {
            code.push_back(bits[cur_pos++]);
        }
        table[character] = code;
    }
    return table;
}

inline vector<char> bits2bytes(const vector<bool>& bits)
{
    // Буфер для записи результата.
    size_t byte_num = bits.size() / 8;
    if (bits.size() % 8 > 0) byte_num++;
    // Буфер для записи результата. По умолчанию значения всех бит устанавливаются в FALSE.
    vector<char> bytes(byte_num, 0x00);
    // Цикл по байтам данных.
    for (int i = 0; i < bits.size(); i++) {
        // Индекс текущего байта.
        int byte_idx = i / 8;
        // Индекс текущего бита в байте.
        int bit_idx = i % 8;
        // Если текущий бит равен TRUE, то его надо выставить в TRUE.
        if (bits[i]) bytes[byte_idx] |= (0x01 << (7 - bit_idx));
    }
    return bytes;
}

inline vector<bool> bytes2bits(const vector<char>& bytes)
{
    // Буфер для записи результата.
    vector<bool> bits(bytes.size() * 8);
    // Цикл по байтам данных.
    for (int byte_idx = 0; byte_idx < bytes.size(); byte_idx++) {
        // Цикл по битам текущего байта.
        for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
            // Установка текущего бита.
            bits[byte_idx * 8 + bit_idx] = ((bytes[byte_idx] >> (7 - bit_idx)) & 0x01);
        }
    }
    return bits;
}

//для записи бит в файл:
inline void write_bit_sequence(ostream& out, const vector<bool>& bits)
{
    // Буферы для записи количества бит, байт и последовательности байт.
    auto bytes = bits2bytes(bits);
    // Запись размера кодовой таблицы и самой кодовой таблицы в файл.
    size_t bit_num = bits.size();
    out.write((char*)&bit_num, sizeof(bit_num));
    out.write(bytes.data(), bytes.size());
}

inline vector<bool> read_bit_sequence(istream& in)
{
    // Буферы для записи количества бит, байт и последовательности байт.
    size_t bit_num, byte_num;
    vector<char> bytes;
    // Считывание количества бит и считывание последовательности байт.
    in.read((char*)&bit_num, sizeof(bit_num));
    byte_num = bit_num / 8;
    if (bit_num % 8 > 0) byte_num++;
    bytes.resize(byte_num);
    in.read((char*)bytes.data(), byte_num);
    // Преобразование последовательности байт в последовательность бит.
    auto bits = bytes2bits(bytes);
    bits.resize(bit_num);
    return bits;
}
////////////////////
//проверки//////////

//Оценить теоретический коэффициент сжатия (длина равномерного кода (8) к средней длине оптимального).
float estimate_compression(const unordered_map<char, vector<bool>>& table, const unordered_map<char, float>& proba)
{
    float Lavg = 0.0; // будет хранить среднюю длину оптимального кода
    for (const auto& code : table)   //Цикл по всем элементам кодовой таблицы   
        //table - это ассоциативный массив, где ключами являются символы, а значениями - векторы бит, представляющие их код

    {
        const auto pr = proba.at(code.first);  //proba.at(code.first) возвращает кол-во появлений символа code.first
        Lavg += (code.second).size() * pr;  //(code.second).size() возвращает длину кода для символа code.first.
        //Мы умножаем эту длину на вероятность появления символа и добавляем результат к Lavg. 
    }
    //теоретический коэффициент сжатия
    return 8 / Lavg;
}




//Оценить практический коэффициент сжатия (исходное количество байт к количеству байт после сжатия)
float estimate_compression_prak(size_t bytes_before, size_t bytes_after)
{
    return (float)bytes_before / bytes_after;;
}




//проверка оптимальности кода  
bool is_optimal(const unordered_map<char, vector<bool>>& table, const unordered_map<char, float>& proba)
{
    float Lavg = 0.0; //средняя длина кода
    for (const auto& code : table)
    {
        const auto pr = proba.at(code.first);
        Lavg += (code.second).size() * pr;
    }
    float Lopt = 0.0; //средняя длина оптимального
    for (const auto& p : proba)
    {
        Lopt -= p.second * log2(p.second);
    }
    if (Lopt <= Lavg <= Lopt + 1) return 1;
    else return 0;
}



//Проверка префиксности кода
bool is_prefix(const unordered_map<char, vector<bool>>& table)
{
    for (auto entry1 : table) //берем элемент unordered_map
    {
        for (auto entry2 : table) //сравниваем с остальными
        {
            if (entry1.first == entry2.first)
            {
                continue; // Пропускаем сравнение с самим собой
            }
            auto code1 = entry1.second; //код entry1
            auto code2 = entry2.second; //код entry2
            if (code1.size() > code2.size())
            {
                if (equal(code2.begin(), code2.end(), code1.begin()))
                {
                    return false; // code1 является префиксом code2
                }
            }
        }
    }
    return true;
}

size_t getFileSize(const string& filename) 
{
    ifstream file(filename, ios::binary | ios::ate);           //функция для узнавания размера любого файла
    return file.tellg();
}


int main()
{
    setlocale(LC_ALL, "ru");

    ifstream rFile;
    rFile.open("The_Picture_of_Dorian_Gray.txt", ios::binary); // Открываем файл для чтения в бинарном режиме
    if (!rFile.is_open())
        throw runtime_error("Не удалось открыть файл для чтения");

    // Определяем размер файла
    rFile.seekg(0, ios::end);
    streampos fileSize = rFile.tellg();
    rFile.seekg(0, ios::beg);

    // Выделяем буфер для хранения содержимого файла
    vector<char> bytes(fileSize);
    seredina = bytes.size() / 2.;

    // Читаем содержимое файла в буфер
    rFile.read(bytes.data(), fileSize);

    // Закрываем файл
    rFile.close();

    cout << endl;
    probabilities = estimate_proba(bytes);
    for (auto iter = probabilities.begin(); iter != probabilities.end(); iter++) {
        const auto& pair = *iter;
        cout << pair.first << " - " << pair.second << endl;
    }
    cout << endl;
    cout << "отсортированная таблица вероятностей: \n";
    //сортировка ассоциативного массива:
    vector<pair<char, float>> tempVec(probabilities.begin(), probabilities.end());
    sort(tempVec.begin(), tempVec.end(), Value);

    for (auto iter = tempVec.begin(); iter != tempVec.end(); iter++)
    {
        sortedprobabilities[iter->first] = iter->second;
        const auto& pair = *iter;
        cout << pair.first << " - " << pair.second << endl;
    }

    cout << endl;

   
    // Поиск кода Шеннона-Фано
    unordered_map<char, string> codes = build_code(tempVec, 0, tempVec.size() - 1);
    unordered_map<char, vector<bool>> codesvectbl;

    cout << "Кодовая таблица: \n";
    // Вывод кода для каждого символа
    for (const auto& pair : codes)
    {
        vector<bool> bits;
        for (char c : pair.second)
        {
            bits.push_back(c == '1');
        }
        codesvectbl[pair.first] = bits;

        cout << pair.first << " = " << pair.second << endl;
    }

    vector<bool> tableinbits = table2bits(codesvectbl);

    //запись код.таблицы в файл
    string nameforcodtab = "";
    cout << "Введите имя для файла, в который будет записана кодовая таблица: "; 
    cin >> nameforcodtab;

    nameforcodtab = nameforcodtab + ".bin";
    ofstream wFile;
    wFile.open(nameforcodtab, ios::binary);

    if (!wFile.is_open())
        throw runtime_error("Не удалось открыть файл для записи кодовой таблицы:((");
    write_bit_sequence(wFile, tableinbits);
    wFile.close();
    cout << "кодовая таблица записана в файл";
    cout << endl;
    cout << endl;
    vector<bool> encode_bits = encode(bytes, codesvectbl);
    //запись закодированных символов в файл
    string nameforcodfail = "";
    cout << "Введите имя для файла, в который будет записан закодированные символы: ";
    cin >> nameforcodfail;
    nameforcodfail = nameforcodfail + ".bin";
    

    wFile.open(nameforcodfail, ios::binary);
    if (!wFile.is_open())
        throw runtime_error("Не удалось открыть файл для записи текст:((");
    write_bit_sequence(wFile, encode_bits);
    wFile.close();
    cout << "Текст записан в файл"<<endl; 
    cout << endl;


    vector<char> encoded_bytes = bits2bytes(encode_bits);

    size_t size1 = getFileSize("The_Picture_of_Dorian_Gray.txt");
    size_t size2 = getFileSize(nameforcodfail);
    size_t size3 = getFileSize(nameforcodtab);

    float compressionRatio = estimate_compression(codesvectbl, probabilities);
    float compressionRatio1 = estimate_compression_prak(size1, (size2 + size3));
    cout << "Теоретический коэффициент: " << compressionRatio << endl;
    cout << "Практический коэффициент: " << compressionRatio1 << endl;


    if (is_optimal(codesvectbl, probabilities)) cout << "\nКод является оптимальным." << endl;
    else cout << "\nКод НЕ является оптимальным." << endl;
    //проверка префиксности кода
    if (is_prefix(codesvectbl)) cout << "\nКод является префиксным." << endl;
    else cout << "\nКод НЕ является префиксным." << endl;


    /////////////////////////////////////////////декодирование//////////////////////////////////////////////////////////////////////

    string nameoffile = "text.bin", nameoftable = "table.bin";
    //cout << "Введите имя для файла, который надо раскодировать: ";
    //cin >> nameoffile;


    rFile.open(nameoffile, ios::binary); // Открываем файл для чтения в бинарном режиме
    if (!rFile.is_open())
    {
        throw runtime_error("Не удалось открыть файл для чтения");
        return 1;
    }
   
    vector<bool> bitsdecoder = read_bit_sequence(rFile);
    vector<char> bytesdecoder = bits2bytes(bitsdecoder);
    rFile.close();


   // cout << "Введите имя для файла, в который записана кодовая таблица: ";
    //cin >> nameoftable;

    rFile.open(nameoftable, ios::binary);
    if (!rFile.is_open())
    {
        throw runtime_error("Не удалось открыть файл для чтения");
        return 1;
    }

    vector<bool> bitsdecodtable = read_bit_sequence(rFile);
    unordered_map<char, vector <bool>> decodtable = bits2table(bitsdecodtable);
    rFile.close();
    
    //декодируем набор бит
    vector<char> decoding = decode(bitsdecoder, decodtable);
   

    //запись раскодированного текста в файл
    string namefornewfile = "";
    cout << "Введите имя для файла, в который будет записан раскодированный текст: ";
    cin >> namefornewfile;
    namefornewfile = namefornewfile + ".txt";
    wFile.open(namefornewfile, ios::binary);

    if (!wFile.is_open())
    {
        throw runtime_error("Не удалось открыть файл для чтения");
        return 1;
    }


    for (char simv : decoding)
    {
        wFile << simv;
    }

    wFile.close();
    cout << "раскодированный текст записан в файл";

    return 0;
}



