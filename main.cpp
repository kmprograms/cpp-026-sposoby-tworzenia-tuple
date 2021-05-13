#include <iostream>
#include <string>
#include <tuple>

template<typename... T>
class A {
public:
    // tuple referencji - przechowuje referencje do obiektow ktore zostaly przekazane jako
    // argument konstruktora
    std::tuple<T const &...> items;

    explicit A(T const &... items) : items{std::make_tuple(items...)} {};
};

template<typename... T>
class B {
public:
    // tuple referencji - przechowuje referencje do obiektow ktore zostaly przekazane jako
    // argument konstruktora
    std::tuple<T const &...> items;

    explicit B(T const &... items) : items{std::tie(items...)} {};
};

std::string message() {
    return "Message";
}

struct S {};

void f(S &s) { std::cout << "LV\n"; }

void f(S &&s) { std::cout << "RV\n"; }

template<class T>
void wrap(T &&t) {
    f(t);
}

template<class T>
void wrap2(T &&t) {
    f(std::forward<T>(t));
}

void print_tuple(std::tuple<int &, std::string &&> t) {
    std::cout << std::get<0>(t) << ", " << std::get<1>(t) << '\n';
}

int main() {

    std::cout << "-------------------------------- std::make_tuple ----------------------------------\n";
    // Niestety ponizsze wywolania prowadza do wypisania w konsoli bardzo dziwnych wynikow
    // odbiegajacych od tego co chcielismy otrzymac
    // Jest to pewne niezdefiniowane zachowanie

    int v1 = 10;
    std::string v2 = "sth";
    auto t1 = A<int, std::string>(v1, v2);
    auto[e1, e2] = t1.items;
    std::cout << e1 << std::endl;
    std::cout << e2 << std::endl;

    // Pod spodem std::make_tuple analizuje kazdy kolejny typ ktory podano.
    // Dla kazdego podanego typu wywolana jest instancja struktury std::decay
    // ktora wnioskuje jaki faktycznie typ przekazano
    // std::decay usuwa atrybut const oraz & z przekazanych typow.
    // To powoduje ze w powyzszym przypadku po prostu tworzy sie std::tuple<int, std::string>
    // Skladnik klasy A o nazwie items inicjalizuje wszystkie jego referencje wartosciami
    // utworzonej wlasnie tymczasowej std::tuple<int, std::string>
    // Jednak ta tymczasowa std::tuple tworzona przez std::make_tuple jest niszczona na koniec
    // pracy konstruktora powodujac ze items ktore wskazywaly na jej elementy teraz wskazuja
    // na pewne niekoreslone obszary

    std::cout << "------------------------------------ std::tie -------------------------------------\n";
    // Rozwiazaniem tego problemu jest zastosowanie std::tie
    // std::tie nie analizuje kolejnych przekazanych typow za pomoca std::decay
    // std::tie przechowuje referencje do przekazanych mu parametrow
    // Zastosowano std::tie w klasie B
    // Dzieki temu std::tie po prostu ustawia referencje items w instancji klasy B
    // na tych przygotowanych wczesniej referencjach i wszystko dziala poprawnie

    /*
    int v1 = 10;
    std::string v2 = "sth";
    auto t1 = B<int, std::string>(v1, v2);
    auto[e1, e2] = t1.items;
    std::cout << e1 << std::endl;
    std::cout << e2 << std::endl;
    */

    std::cout << "------------------------------ std::forward_as_tuple ------------------------------\n";
    /*
    // std::forward - przypomnienie
    S s;
    f(s);
    f(S());

    wrap(s);
    wrap(S());

    wrap2(s);
    wrap2(S());


    // Mamy jeszcze 3 sposob tworzenia std::tuple za pomoca std::forward_as_tuple
    // Rozpoznaje typy na podobnej zasadzie jak std::forward czyli:
    // -> kiedy dostaje lvalue wtedy traktuje to jak referencje do lvalue
    // -> kiedy dostaje rvalue wtedy traktuje to jak referencje do rvalue

    int v1 = 10;
    print_tuple(std::forward_as_tuple(v1, "A: " + message()));
    */

    // PODSUMOWANIE
    // std::make_tuple kiedy w zwracanej tuple potrzebujesz przechowywac wartosci,
    // std::tie kiedy w zwracanej tuple potrzebuejsz przechowywac referencje do l-wartosci,
    // std::forward_as_tuple kiedy chcesz podtrzymac typy referencji elementow z ktorych budujesz tuple

    return 0;
}
