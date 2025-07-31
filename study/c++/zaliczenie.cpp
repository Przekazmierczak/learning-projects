#include <iostream>
#include <string>
#include <memory>
#include <array>
#include <algorithm>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                                         BLOK 1
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//============================================================
//           Kod do zadania 1.               
//============================================================

const int a_size = 7;

class A {
private:
    const int a;
    int b;
public:
    A() : a(1), b(2) {};

    int get_a() { return a; };
    int get_b() { return b; }; 
    void set_b(int input_b) { b = input_b; };
};

//============================================================
//           Kod do zadania 1.1.               
//============================================================
void pokaz(A& a) {
    std::cout << "Wartosc a: " << a.get_a() << ", wartosc b: " << a.get_b() << std::endl;
}

void pokaz(A* a) {
    std::cout << "Wartosc a: " << a->get_a() << ", wartosc b: " << a->get_b() << std::endl;
}

//============================================================
//           Kod do zadania 1.2.               
//============================================================

void pokaz(A a_tablica[], int size) {
    for (int i = 0; i < size; ++i) {
        std::cout << "Wartosc a: " << a_tablica[i].get_a() << ", wartosc b: " << a_tablica[i].get_b() << std::endl;
    }
}

//============================================================
//           Kod do zadania 1.3.               
//============================================================

void stworz(A*& ptr) {
    ptr = new A;
}
void usun(A*& ptr) {
    delete ptr;
}

//============================================================
//           Kod do zadania 1.4.               
//============================================================

void stworz(A**& ptr) {
    ptr = new A*[7];

    for (int i = 0; i < a_size; ++i) {
        ptr[i] = new A;
    }
}

void pokaz(A**& ptr) {
    for (int i = 0; i < a_size; ++i) {
        std::cout << "Wartosc a: " << ptr[i]->get_a() << ", wartosc b: " << ptr[i]->get_b() << std::endl;
    }
}

void usun(A**& ptr) {
    for (int i = 0; i < a_size; ++i) {
        delete ptr[i];
    }
    delete[] ptr;
}

#include <locale>
void foo_1() {
    //============================================================
    //           Kod do zadania 1.1.               
    //============================================================
    //  Stwórz lokalny obiekt typu A na stosie
    A a_stos;

    pokaz(a_stos);
    pokaz(&a_stos);

    //============================================================
    //           Kod do zadania 1.2.               
    //============================================================
    // Tu stwórz lokalną tablicę obiektów typu A na stosie
    A a_tablica_stos[a_size];

    // tu pokaż zawartość tablicy za pomocą pętli zakresowej
    for (auto& element : a_tablica_stos) {
        pokaz(element);
    }

    pokaz(a_tablica_stos, a_size); // pokaż równierz tablicę za pomocą funkcji
    //============================================================
    //           Kod do zadania 1.3.               
    //============================================================
    A* ptr = nullptr;
    stworz(ptr);
    pokaz(ptr);
    usun(ptr);

    //============================================================
    //           Kod do zadania 1.4.               
    //============================================================

    A** ptr_tablica = nullptr;
    stworz(ptr_tablica);
    pokaz(ptr_tablica);
    usun(ptr_tablica);
}

//============================================================
//           Kod do zadania 2.               
//============================================================
namespace D1 {
    const int size = 3;

    class Sprzedawca {
        std::string nazwa{ "Jestem sprzedawcą tego pojazdu" };
        public:
            std::string get_nazwa() { return nazwa; };
    };

    struct Silnik {
        virtual void pokaz() = 0;

        virtual ~Silnik() = default;
    };

    struct Tlokowy : public Silnik {
        void pokaz() override { std::cout << " z silnikiem tlokowym" << std::endl; }
    };

    struct Elektryczny : public Silnik {
        void pokaz() override { std::cout << " z silnikiem elektrycznym" << std::endl; }
    };

    struct Odrzutowy : public Silnik {
        void pokaz() override { std::cout << " z silnikiem odrzutowym" << std::endl; }
    };

    struct Pojazd {
        virtual void pokaz() = 0;
        inline static Sprzedawca sprzedawca;

        virtual ~Pojazd() = default;

        friend void pokaz(Pojazd* pojazdy[]);
    protected:
        Silnik* silnik;
    };

    struct Samochod : public Pojazd {
        Samochod() {
            silnik = new Elektryczny;
        }
        void pokaz() {
            std::cout << "Jest to samochod";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    struct Samolot : public Pojazd  {
        Samolot() {
            silnik = new Tlokowy;
        }
        void pokaz() {
            std::cout << "Jest to samolot";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    struct Rakieta : public Pojazd {
        Rakieta() {
            silnik = new Odrzutowy;
        }
        void pokaz() {
            std::cout << "Jest to rakieta";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    void stworz(Pojazd* pojazdy[]) {
        pojazdy[0] = new Rakieta;
        pojazdy[1] = new Samochod;
        pojazdy[2] = new Samolot;
    }

    void usun(Pojazd* pojazdy[]) {
        for (int i = 0; i < size; ++i) {
            delete pojazdy[i];
        }
    }

    void pokaz(Pojazd* pojazdy[]) {
        for (int i = 0; i < size; ++i) {
            pojazdy[i]->pokaz();
        }
    }

    void foo_2() {
        // W tym miejscu zdefiniować tablicę na stosie, która mogłaby posłużyć do przechowywania 3 rożnych typów pojazdów
        Pojazd* pojazdy[size];

        stworz(pojazdy);
        pokaz(pojazdy);
        usun(pojazdy);
    }
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                                         BLOK 2
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//============================================================
//           Kod do zadania 3.               
//============================================================
namespace D2 {
    const int size = 3;

    class Sprzedawca {
        std::string nazwa{ "Jestem sprzedawcą tego pojazdu" };
        public:
            std::string get_nazwa() { return nazwa; };
    };

    struct Silnik {
        virtual void pokaz() = 0;

        virtual ~Silnik() = default;
    };

    struct Tlokowy : public Silnik {
        void pokaz() override { std::cout << " z silnikiem tlokowym" << std::endl; }
    };

    struct Elektryczny : public Silnik {
        void pokaz() override { std::cout << " z silnikiem elektrycznym" << std::endl; }
    };

    struct Odrzutowy : public Silnik {
        void pokaz() override { std::cout << " z silnikiem odrzutowym" << std::endl; }
    };

    struct Pojazd {
        virtual void pokaz() = 0;
        inline static Sprzedawca sprzedawca;

        virtual ~Pojazd() = default;

        friend void pokaz(Pojazd* pojazdy[]);
    protected:
        std::unique_ptr<Silnik> silnik;
    };

    struct Samochod : public Pojazd {
        Samochod() {
            silnik = std::make_unique<Elektryczny>();
        }
        void pokaz() {
            std::cout << "Jest to samochod";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    struct Samolot : public Pojazd  {
        Samolot() {
            silnik = std::make_unique<Tlokowy>();
        }
        void pokaz() {
            std::cout << "Jest to samolot";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    struct Rakieta : public Pojazd {
        Rakieta() {
            silnik = std::make_unique<Odrzutowy>();
        }
        void pokaz() {
            std::cout << "Jest to rakieta";
            silnik->pokaz();
            std::cout << sprzedawca.get_nazwa() << std::endl;
        }
    };

    void stworz(std::array<std::unique_ptr<Pojazd>, size>& pojazdy) {
        pojazdy[0] = std::make_unique<Rakieta>();
        pojazdy[1] = std::make_unique<Samochod>();
        pojazdy[2] = std::make_unique<Samolot>();
    }

    void pokaz(std::array<std::unique_ptr<Pojazd>, size>& pojazdy) {
        for (auto& pojazd : pojazdy) {
            pojazd->pokaz();
        }
    }

    void foo_3() {
        // W tym miejscu zdefiniować tablicę na stosie, która mogłaby posłużyć do przechowywania 3 rożnych typów pojazdów
        std::array<std::unique_ptr<Pojazd>, size> pojazdy;

        stworz(pojazdy);
        pokaz(pojazdy);

        //============================================================
        //           Kod do zadania 4.               
        //============================================================
        std::for_each(pojazdy.begin(), pojazdy.end(), [](auto& element) {
            element->pokaz();
        });
    }
}

//============================================================
//           Kod do zadania 5.               
//============================================================

struct Pracownik {
};

class Firma {
    size_t liczba{ 10 };
    Pracownik* pracownicy{ nullptr };
public:
    // tu zdefiniuj przeciążone metody
    Firma() {
        pracownicy = new Pracownik[liczba];
    }

    ~Firma() {
        delete[] pracownicy;
        pracownicy = nullptr;
    }

    Firma(const Firma& other) : liczba(other.liczba) {
        pracownicy = new Pracownik[liczba];
        for (int i = 0; i < liczba; ++i) {
            pracownicy[i] = other.pracownicy[i];
        }
    }

    Firma& operator=(const Firma& other) {
        if (this != &other) {
            delete[] pracownicy;
    
            liczba = other.liczba;
            pracownicy = new Pracownik[liczba];
    
            for (int i = 0; i < liczba; ++i) {
                pracownicy[i] = other.pracownicy[i];
            }
    
            return *this;
        }
    }

    Firma(Firma&& other) noexcept : liczba(other.liczba), pracownicy(other.pracownicy) {
        other.pracownicy = nullptr;
        other.liczba = 0;
    }

    Firma& operator=(Firma&& other) noexcept {
        if (this != &other) {
            delete[] pracownicy;
    
            liczba = other.liczba;
            pracownicy = other.pracownicy;
    
            other.liczba = 0;
            other.pracownicy = nullptr;
    
            return *this;
        }
    }
}; 


void egzamin() { 
    // foo_1();
    // D1::foo_2();
    D2::foo_3();
}
int main() { 
    egzamin();
}