#pragma once
#include <unordered_map>
#include <map>
#include <iostream>
#include <vector>
#include<fstream>
#include <algorithm>
#include <string>

using namespace std;
float seredina;
unordered_map<char, float> probabilities;
unordered_map<char, float> sortedprobabilities;

unordered_map<char, float> estimate_proba(const std::vector<char>& bytes);


unordered_map<char, string> build_code(
    const vector<pair<char, float>>& sorted_freq,
    int start_pos,
    int end_pos,
    const string& prefix); // Путь в дереве, который мы строим

vector<bool> encode(const vector<char>& bytes,
    const unordered_map<char, vector<bool>>& table);

//
//
//unordered_map<char, std::vector<bool>> build_code(const std::unordered_map<char, float>& proba);
//
//
//vector<bool> encode(const std::vector<char>& bytes,
//	const std::unordered_map<char, std::vector<bool>>& table);
//
//
//float estimate_compression(const std::unordered_map<char, std::vector<bool>>& table,
//	const std::unordered_map<char, float>& proba);
