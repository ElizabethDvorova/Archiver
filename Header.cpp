#include "Header.h"

bool Value(const pair<char, float>& a, const pair<char, float>& b) 
{
    return a.second > b.second;
}

unordered_map<char, float> estimate_proba(const vector<char>& bytes)
{
    // ������������ ���������� ������ ������� �������
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

//������� �������. ��������� ������ �� ������������, ������ ����� ��� ��� ��������.
unordered_map<char, string> build_code(
    const vector<pair<char, float>>& sorted_freq,
    int start_pos,
    int end_pos,
    const string& prefix = "" // ���� � ������, ������� �� ������
) 
{
    unordered_map<char, string> code_map;

    // ������� ������: ���� ������� �������� �������, ��� ���� ������
    if (start_pos == end_pos) {
        code_map[sorted_freq[start_pos].first] = prefix;
        return code_map;
    }

    // ������� ����� ������ � ������������������
    float total_freq = 0;
    for (int i = start_pos; i <= end_pos; ++i) {
        total_freq += sorted_freq[i].second;
    }
    float half_freq = total_freq / 2;

    // ����� ��������
    float sum_freq = 0;
    int mid = start_pos;
    while (mid < end_pos && (sum_freq + sorted_freq[mid].second) < half_freq) {
        sum_freq += sorted_freq[mid].second;
        ++mid;
    }

    // �������� ����� ����� ������ � ����������� "1" � ��������
    auto left_codes = build_code(sorted_freq, start_pos, mid, prefix + "1");
    code_map.insert(left_codes.begin(), left_codes.end());

    // �������� ������ ����� ������ � ����������� "0" � ��������
    auto right_codes = build_code(sorted_freq, mid + 1, end_pos, prefix + "0");
    code_map.insert(right_codes.begin(), right_codes.end());

    return code_map;
}


//����������� ��������� ������
vector<bool> encode(const vector<char>& bytes, const unordered_map<char, vector<bool>>& table)
{
    vector<bool> encoded;
    for (auto iter = bytes.begin(); iter != bytes.end(); ++iter)
    {
        const auto& bits = table.at(*iter); // �������� ������� ������������� ������� �� �������
        encoded.insert(encoded.end(), bits.begin(), bits.end()); // ��������� ������� ������������� � ���������
    }
    return encoded;
}

//������������� ��������� ����� ������� � �������� �����
vector<char> decode(const vector<bool>& bits, const unordered_map<char, vector<bool>>& table)
{
    vector<char> decoded;// ����� ��������� �������������� �������

    //������� �������� ������� 
    unordered_map<vector<bool>, char> reverse_table;
    for (const auto& t : table) {
        reverse_table[t.second] = t.first;
    }
    //������ ������� �����
    vector<bool> current_bits; //����� ������� ������� ������������������ ����� �� ��������������� ������.
    for (bool b : bits) {
        current_bits.push_back(b);
        // ���������, ���� �� ������������ � �������� �������
        //���� ���� ���������� ����� ������ � ������� �������
        if (reverse_table.count(current_bits))
        {
            decoded.push_back(reverse_table[current_bits]);
            current_bits.clear();
        }
    }
    return decoded;
}

//������� �� ���������� 2///////////////////////////////////////////
inline void append(vector<bool>& body, const vector<bool>& tale)
{
    body.insert(body.end(), tale.begin(), tale.end());
}
inline void append(vector<bool>& body, char tale)
{
    // ���� �� ����� � ����������� �����.
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        // ���������� �������� ����.
        body.emplace_back((tale >> (7 - bit_idx)) & 0x01);
    }
}

inline vector<bool> table2bits(const unordered_map<char, vector<bool>>& table)
{
    // ����� ��� ������ ����������.
    vector<bool> bits;
    // ������� ���� ������� � ������� �������.
    for (auto item : table) 
    {
        // ���������� �������� �������.
        append(bits, item.first);
        // ���������� ����� �������������� ����.
        append(bits, (char)item.second.size());
        // ���������� ����.
        append(bits, item.second);
    }
    return bits;
}

inline char get_byte(const vector<bool>& bits, unsigned int cur_pos)
{
    // ����� ��� ������ ����������. �� ��������� �������� ���� ��� ��������������� � FALSE.
    char byte = 0x00;
    // ���� �� ����� � �����.
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        // ���� ������� ��� ����� TRUE, �� ��� ���� ��������� � TRUE.
        if (bits[cur_pos + bit_idx]) byte |= (0x01 << (7 - bit_idx));
    }
    return byte;
}

inline unordered_map<char, vector<bool>> bits2table(const vector<bool>& bits)
{
    // ����� ��� ������ ����������.
    std::unordered_map<char, vector<bool>> table;
    // ����� �������� ����.
    int cur_pos = 0;
    // ������� ���� ������� � ������� �������.
    while (cur_pos < bits.size()) {
        // ��������� �������.
        char character = (char) get_byte(bits, cur_pos);
        cur_pos += 8;
        // ��������� ����� ����, ���������������� �������.
        char code_length = get_byte(bits, cur_pos);
        cur_pos += 8;
        // ��������� ����, ���������������� �������.
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
    // ����� ��� ������ ����������.
    size_t byte_num = bits.size() / 8;
    if (bits.size() % 8 > 0) byte_num++;
    // ����� ��� ������ ����������. �� ��������� �������� ���� ��� ��������������� � FALSE.
    vector<char> bytes(byte_num, 0x00);
    // ���� �� ������ ������.
    for (int i = 0; i < bits.size(); i++) {
        // ������ �������� �����.
        int byte_idx = i / 8;
        // ������ �������� ���� � �����.
        int bit_idx = i % 8;
        // ���� ������� ��� ����� TRUE, �� ��� ���� ��������� � TRUE.
        if (bits[i]) bytes[byte_idx] |= (0x01 << (7 - bit_idx));
    }
    return bytes;
}

inline vector<bool> bytes2bits(const vector<char>& bytes)
{
    // ����� ��� ������ ����������.
    vector<bool> bits(bytes.size() * 8);
    // ���� �� ������ ������.
    for (int byte_idx = 0; byte_idx < bytes.size(); byte_idx++) {
        // ���� �� ����� �������� �����.
        for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
            // ��������� �������� ����.
            bits[byte_idx * 8 + bit_idx] = ((bytes[byte_idx] >> (7 - bit_idx)) & 0x01);
        }
    }
    return bits;
}

//��� ������ ��� � ����:
inline void write_bit_sequence(ostream& out, const vector<bool>& bits)
{
    // ������ ��� ������ ���������� ���, ���� � ������������������ ����.
    auto bytes = bits2bytes(bits);
    // ������ ������� ������� ������� � ����� ������� ������� � ����.
    size_t bit_num = bits.size();
    out.write((char*)&bit_num, sizeof(bit_num));
    out.write(bytes.data(), bytes.size());
}

inline vector<bool> read_bit_sequence(istream& in)
{
    // ������ ��� ������ ���������� ���, ���� � ������������������ ����.
    size_t bit_num, byte_num;
    vector<char> bytes;
    // ���������� ���������� ��� � ���������� ������������������ ����.
    in.read((char*)&bit_num, sizeof(bit_num));
    byte_num = bit_num / 8;
    if (bit_num % 8 > 0) byte_num++;
    bytes.resize(byte_num);
    in.read((char*)bytes.data(), byte_num);
    // �������������� ������������������ ���� � ������������������ ���.
    auto bits = bytes2bits(bytes);
    bits.resize(bit_num);
    return bits;
}
////////////////////
//��������//////////

//������� ������������� ����������� ������ (����� ������������ ���� (8) � ������� ����� ������������).
float estimate_compression(const unordered_map<char, vector<bool>>& table, const unordered_map<char, float>& proba)
{
    float Lavg = 0.0; // ����� ������� ������� ����� ������������ ����
    for (const auto& code : table)   //���� �� ���� ��������� ������� �������   
        //table - ��� ������������� ������, ��� ������� �������� �������, � ���������� - ������� ���, �������������� �� ���

    {
        const auto pr = proba.at(code.first);  //proba.at(code.first) ���������� ���-�� ��������� ������� code.first
        Lavg += (code.second).size() * pr;  //(code.second).size() ���������� ����� ���� ��� ������� code.first.
        //�� �������� ��� ����� �� ����������� ��������� ������� � ��������� ��������� � Lavg. 
    }
    //������������� ����������� ������
    return 8 / Lavg;
}




//������� ������������ ����������� ������ (�������� ���������� ���� � ���������� ���� ����� ������)
float estimate_compression_prak(size_t bytes_before, size_t bytes_after)
{
    return (float)bytes_before / bytes_after;;
}




//�������� ������������� ����  
bool is_optimal(const unordered_map<char, vector<bool>>& table, const unordered_map<char, float>& proba)
{
    float Lavg = 0.0; //������� ����� ����
    for (const auto& code : table)
    {
        const auto pr = proba.at(code.first);
        Lavg += (code.second).size() * pr;
    }
    float Lopt = 0.0; //������� ����� ������������
    for (const auto& p : proba)
    {
        Lopt -= p.second * log2(p.second);
    }
    if (Lopt <= Lavg <= Lopt + 1) return 1;
    else return 0;
}



//�������� ������������ ����
bool is_prefix(const unordered_map<char, vector<bool>>& table)
{
    for (auto entry1 : table) //����� ������� unordered_map
    {
        for (auto entry2 : table) //���������� � ����������
        {
            if (entry1.first == entry2.first)
            {
                continue; // ���������� ��������� � ����� �����
            }
            auto code1 = entry1.second; //��� entry1
            auto code2 = entry2.second; //��� entry2
            if (code1.size() > code2.size())
            {
                if (equal(code2.begin(), code2.end(), code1.begin()))
                {
                    return false; // code1 �������� ��������� code2
                }
            }
        }
    }
    return true;
}

size_t getFileSize(const string& filename) 
{
    ifstream file(filename, ios::binary | ios::ate);           //������� ��� ��������� ������� ������ �����
    return file.tellg();
}


int main()
{
    setlocale(LC_ALL, "ru");

    ifstream rFile;
    rFile.open("The_Picture_of_Dorian_Gray.txt", ios::binary); // ��������� ���� ��� ������ � �������� ������
    if (!rFile.is_open())
        throw runtime_error("�� ������� ������� ���� ��� ������");

    // ���������� ������ �����
    rFile.seekg(0, ios::end);
    streampos fileSize = rFile.tellg();
    rFile.seekg(0, ios::beg);

    // �������� ����� ��� �������� ����������� �����
    vector<char> bytes(fileSize);
    seredina = bytes.size() / 2.;

    // ������ ���������� ����� � �����
    rFile.read(bytes.data(), fileSize);

    // ��������� ����
    rFile.close();

    cout << endl;
    probabilities = estimate_proba(bytes);
    for (auto iter = probabilities.begin(); iter != probabilities.end(); iter++) {
        const auto& pair = *iter;
        cout << pair.first << " - " << pair.second << endl;
    }
    cout << endl;
    cout << "��������������� ������� ������������: \n";
    //���������� �������������� �������:
    vector<pair<char, float>> tempVec(probabilities.begin(), probabilities.end());
    sort(tempVec.begin(), tempVec.end(), Value);

    for (auto iter = tempVec.begin(); iter != tempVec.end(); iter++)
    {
        sortedprobabilities[iter->first] = iter->second;
        const auto& pair = *iter;
        cout << pair.first << " - " << pair.second << endl;
    }

    cout << endl;

   
    // ����� ���� �������-����
    unordered_map<char, string> codes = build_code(tempVec, 0, tempVec.size() - 1);
    unordered_map<char, vector<bool>> codesvectbl;

    cout << "������� �������: \n";
    // ����� ���� ��� ������� �������
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

    //������ ���.������� � ����
    string nameforcodtab = "";
    cout << "������� ��� ��� �����, � ������� ����� �������� ������� �������: "; 
    cin >> nameforcodtab;

    nameforcodtab = nameforcodtab + ".bin";
    ofstream wFile;
    wFile.open(nameforcodtab, ios::binary);

    if (!wFile.is_open())
        throw runtime_error("�� ������� ������� ���� ��� ������ ������� �������:((");
    write_bit_sequence(wFile, tableinbits);
    wFile.close();
    cout << "������� ������� �������� � ����";
    cout << endl;
    cout << endl;
    vector<bool> encode_bits = encode(bytes, codesvectbl);
    //������ �������������� �������� � ����
    string nameforcodfail = "";
    cout << "������� ��� ��� �����, � ������� ����� ������� �������������� �������: ";
    cin >> nameforcodfail;
    nameforcodfail = nameforcodfail + ".bin";
    

    wFile.open(nameforcodfail, ios::binary);
    if (!wFile.is_open())
        throw runtime_error("�� ������� ������� ���� ��� ������ �����:((");
    write_bit_sequence(wFile, encode_bits);
    wFile.close();
    cout << "����� ������� � ����"<<endl; 
    cout << endl;


    vector<char> encoded_bytes = bits2bytes(encode_bits);

    size_t size1 = getFileSize("The_Picture_of_Dorian_Gray.txt");
    size_t size2 = getFileSize(nameforcodfail);
    size_t size3 = getFileSize(nameforcodtab);

    float compressionRatio = estimate_compression(codesvectbl, probabilities);
    float compressionRatio1 = estimate_compression_prak(size1, (size2 + size3));
    cout << "������������� �����������: " << compressionRatio << endl;
    cout << "������������ �����������: " << compressionRatio1 << endl;


    if (is_optimal(codesvectbl, probabilities)) cout << "\n��� �������� �����������." << endl;
    else cout << "\n��� �� �������� �����������." << endl;
    //�������� ������������ ����
    if (is_prefix(codesvectbl)) cout << "\n��� �������� ����������." << endl;
    else cout << "\n��� �� �������� ����������." << endl;


    /////////////////////////////////////////////�������������//////////////////////////////////////////////////////////////////////

    string nameoffile = "text.bin", nameoftable = "table.bin";
    //cout << "������� ��� ��� �����, ������� ���� �������������: ";
    //cin >> nameoffile;


    rFile.open(nameoffile, ios::binary); // ��������� ���� ��� ������ � �������� ������
    if (!rFile.is_open())
    {
        throw runtime_error("�� ������� ������� ���� ��� ������");
        return 1;
    }
   
    vector<bool> bitsdecoder = read_bit_sequence(rFile);
    vector<char> bytesdecoder = bits2bytes(bitsdecoder);
    rFile.close();


   // cout << "������� ��� ��� �����, � ������� �������� ������� �������: ";
    //cin >> nameoftable;

    rFile.open(nameoftable, ios::binary);
    if (!rFile.is_open())
    {
        throw runtime_error("�� ������� ������� ���� ��� ������");
        return 1;
    }

    vector<bool> bitsdecodtable = read_bit_sequence(rFile);
    unordered_map<char, vector <bool>> decodtable = bits2table(bitsdecodtable);
    rFile.close();
    
    //���������� ����� ���
    vector<char> decoding = decode(bitsdecoder, decodtable);
   

    //������ ���������������� ������ � ����
    string namefornewfile = "";
    cout << "������� ��� ��� �����, � ������� ����� ������� ��������������� �����: ";
    cin >> namefornewfile;
    namefornewfile = namefornewfile + ".txt";
    wFile.open(namefornewfile, ios::binary);

    if (!wFile.is_open())
    {
        throw runtime_error("�� ������� ������� ���� ��� ������");
        return 1;
    }


    for (char simv : decoding)
    {
        wFile << simv;
    }

    wFile.close();
    cout << "��������������� ����� ������� � ����";

    return 0;
}



