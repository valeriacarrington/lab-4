#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

class NFA {
public:
    set<int> states;  // множина всіх станів
    set<char> alphabet;  // алфавіт
    int start_state;  // початковий стан
    set<int> final_states;  // фінальні стани
    map<pair<int, char>, set<int>> transitions;  // функція переходів

    void read_from_file(const string& filename) {
        ifstream file(filename);
        string line;
        
        int A, S, F;
        // Читання кількості елементів алфавіту та станів
        file >> A >> S;
        
        // Читання початкового стану
        file >> start_state;
        
        // Читання фінальних станів
        file >> F;
        for (int i = 0; i < F; ++i) {
            int f;
            file >> f;
            final_states.insert(f);
        }
        
        // Читання функції переходів
        while (getline(file, line)) {
            stringstream ss(line);
            int from, to;
            char symbol;
            if (ss >> from >> symbol >> to) {
                transitions[{from, symbol}].insert(to);
            }
        }
    }
};

class DFA {
public:
    set<int> states;  // множина станів
    set<char> alphabet;  // алфавіт
    set<int> final_states;  // фінальні стани
    map<pair<set<int>, char>, set<int>> transitions;  // функція переходів
    set<int> start_state;  // початковий стан

    void print_dfa() {
        cout << "States: ";
        for (const auto& state : states) {
            cout << state << " ";
        }
        cout << endl;

        cout << "Final States: ";
        for (const auto& state : final_states) {
            cout << state << " ";
        }
        cout << endl;

        cout << "Transitions: " << endl;
        for (const auto& transition : transitions) {
            cout << "{";
            for (const auto& state : transition.first.first) {
                cout << state << " ";
            }
            cout << "} --" << transition.first.second << "--> {";
            for (const auto& state : transition.second) {
                cout << state << " ";
            }
            cout << "}" << endl;
        }
    }
};

// Метод перетворення НКА в ДКА
DFA convert_to_dfa(const NFA& nfa) {
    DFA dfa;
    map<set<int>, int> state_map;  // відображення множин станів у номери станів ДКА
    vector<set<int>> state_list;
    
    // Початковий стан
    set<int> start_set = {nfa.start_state};
    state_list.push_back(start_set);
    state_map[start_set] = 0;
    dfa.states.insert(0);
    if (nfa.final_states.count(nfa.start_state)) {
        dfa.final_states.insert(0);
    }

    // Алфавіт
    dfa.alphabet = nfa.alphabet;

    // Створення нових станів ДКА
    for (int i = 0; i < state_list.size(); ++i) {
        for (char symbol : nfa.alphabet) {
            set<int> next_set;
            for (int state : state_list[i]) {
                auto it = nfa.transitions.find({state, symbol});
                if (it != nfa.transitions.end()) {
                    next_set.insert(it->second.begin(), it->second.end());
                }
            }
            
            if (!next_set.empty()) {
                if (state_map.find(next_set) == state_map.end()) {
                    state_map[next_set] = state_list.size();
                    state_list.push_back(next_set);
                    dfa.states.insert(state_list.size() - 1);
                    if (next_set.count(nfa.start_state)) {
                        dfa.final_states.insert(state_list.size() - 1);
                    }
                }
                dfa.transitions[{state_list[i], symbol}] = next_set;
            }
        }
    }

    // Встановлення початкового стану ДКА
    dfa.start_state = start_set;

    return dfa;
}

int main() {
    NFA nfa;
    nfa.read_from_file("nfa_input.txt");  // Читання даних НКА з файлу

    DFA dfa = convert_to_dfa(nfa);  // Перетворення в ДКА

    dfa.print_dfa();  // Виведення результату

    return 0;
}
