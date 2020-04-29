#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

const int dim = 40;

struct automat {
    int nrStari = 0, m = 0, q0 = 0, k = 0, stariFinale[dim] = {}, tranzitii[dim][dim][dim] = {{{}}}, nrTranzitii = 0;
    char alfabet[dim] = " ";
};

int cautCaracter(char c, char v[]) {
    for (int i = 0; i < strlen(v); ++i)
        if (v[i] == c)
            return i;
    return -1;
}

int cautStare(const int v[], int n, int x) {
    int i;
    for (i = 1; i <= n; ++i)
        if (v[i] == x)
            return 1;
    return -1;
}

void eliminStare(int v[], int &n, int p) {
    int i;
    for (i = p + 1; i <= n; ++i)
        v[i - 1] = v[i];
    n--;
}

void copieVector(int a[], const int b[], int n) {
    int i;
    for(i = 1; i <=n ; ++i)
        a[i] = b[i];
}

void eliminaLinie(int a[dim][dim][dim], int &nrLinii, int nrColoane, int p) {
    int i, j;
    for(i = p; i < nrLinii; ++i)
        for(j = 0; j < nrColoane; ++j) {
            a[i - 1][j][0] = a[i][j][0];
            copieVector(a[i-1][j], a[i][j], a[i][j][0]);
        }
    nrLinii--;
}

void citire(automat &a) {
    ifstream fin("automat.in");
    int i, j, stare1, stare2, poz;
    char tranzitie;
    fin >> a.nrStari >> a.m;
    for (i = 0; i < a.nrStari; ++i)
        for (j = 0; j <= a.m; ++j)
            a.tranzitii[i][j][0] = 0;
    for (i = 0; i < a.m; ++i)
        fin >> a.alfabet[i];
    fin >> a.q0 >> a.k;
    for (i = 1; i <= a.k; ++i)
        fin >> a.stariFinale[i];
    fin >> a.nrTranzitii;
    for (i = 1; i <= a.nrTranzitii; ++i) {
        fin >> stare1 >> tranzitie >> stare2;
        if (tranzitie == '$')
            a.tranzitii[stare1][a.m][++a.tranzitii[stare1][a.m][0]] = stare2;
        else {
            poz = cautCaracter(tranzitie, a.alfabet);
            a.tranzitii[stare1][poz][++a.tranzitii[stare1][poz][0]] = stare2;
        }
    }
    fin.close();
}

void afisare(automat a) {
    int i, j, k;
    cout << a.nrStari << "\n";
    cout << a.m << "\n";
    cout << a.alfabet << "\n";
    cout << a.q0 << "\n";
    cout << a.k << "\n";
    for (i = 1; i <= a.k; ++i)
        cout << a.stariFinale[i] << " ";
    cout << "\n";
    for (i = 0; i < a.nrStari; ++i) {
        for (j = 0; j < a.m; ++j) {
            if (a.tranzitii[i][j][0]) {
                cout << "[ ";
                a.nrTranzitii += a.tranzitii[i][j][0];
                for (k = 1; k <= a.tranzitii[i][j][0]; ++k)
                    cout << a.tranzitii[i][j][k] << " ";
                cout << "] ";
            } else cout << "[] ";
        }
        cout << endl;
    }
    cout << a.nrTranzitii << "\n";
    for (i = 0; i < a.nrStari; ++i) {
        for (j = 0; j < a.m; ++j) {
            if (a.tranzitii[i][j][0])
                for (k = 1; k <= a.tranzitii[i][j][0]; ++k)
                    cout << i << " " << a.alfabet[j] << " " << a.tranzitii[i][j][k] << "\n";
        }
    }
}

bool compar(const int a[], const int b[], int n) {
    int i;
    for (i = 0; i < n; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}

bool stariIdentice(automat a, int stare1, int stare2) {
    if (cautStare(a.stariFinale, a.k, stare1) != cautStare(a.stariFinale, a.k, stare2))
        return false;
    int i, c;
    int f1[a.nrStari], f2[a.nrStari];
    for (i = 0; i < a.nrStari; ++i)
        f1[i] = f2[i] = 0;
    for (c = 0; c < a.m; ++c) {
        if (a.tranzitii[stare1][c][0] != a.tranzitii[stare2][c][0])
            return false;
        for (i = 1; i <= a.tranzitii[stare1][c][0]; ++i)
            f1[a.tranzitii[stare1][c][i]] = 1;
        for (i = 1; i <= a.tranzitii[stare2][c][0]; ++i)
            f2[a.tranzitii[stare2][c][i]] = 1;
        if (!compar(f1, f2, a.nrStari))
            return false;
    }
    return true;
}

void replace(automat &a, int stare1, int stare2) {
    int i, j, k;
    for (i = 0; i < a.nrStari; ++i)
        for (j = 0; j < a.m; ++j)
            for (k = 1; k <= a.tranzitii[i][j][0]; ++k)
                if (a.tranzitii[i][j][k] == stare1) {
                    if (cautStare(a.tranzitii[i][j], a.tranzitii[i][j][0], stare2) == 1)
                        eliminStare(a.tranzitii[i][j], a.tranzitii[i][j][0], k);
                    else
                        a.tranzitii[i][j][k] = stare2;
                }
}

void LNFAtoNFA(automat a, automat &b) {
    int inchideri[dim][dim], stare, i, j, p, stareCurenta, c, k;
    bool adaug;
    b.nrStari = a.nrStari;
    strcpy(b.alfabet, a.alfabet);
    b.m = a.m;
    for (i = 0; i < b.nrStari; ++i)
        for (j = 0; j <= b.m; ++j)
            b.tranzitii[i][j][0] = 0;
    for (i = 0; i < a.nrStari; ++i)
        inchideri[i][0] = 0;
    for (stare = 0; stare < a.nrStari; ++stare) { //PASUL 1: Calcularea lambda-inchiderii
        p = 1;
        stareCurenta = stare;
        inchideri[stare][++inchideri[stare][0]] = stare;
        do {
            adaug = false;
            for (i = 1; i <= a.tranzitii[stareCurenta][a.m][0]; ++i) {
                if (cautStare(inchideri[stare], inchideri[stare][0], a.tranzitii[stareCurenta][a.m][i]) == -1) {
                    inchideri[stare][++inchideri[stare][0]] = a.tranzitii[stareCurenta][a.m][i];
                    adaug = true;
                }
            }
            p++;
            stareCurenta = inchideri[stare][p];
        } while (adaug);
    }
    for (i = 0; i < a.nrStari; ++i) {   //PASUL 2: Calcularea functiei de tranzitie
        for (j = 1; j <= inchideri[i][0]; ++j) {
            for (c = 0; c < a.m; ++c) {
                for (p = 1; p <= a.tranzitii[inchideri[i][j]][c][0]; ++p) {
                    stare = a.tranzitii[inchideri[i][j]][c][p];
                    for (k = 1; k <= inchideri[stare][0]; ++k) {
                        if (cautStare(b.tranzitii[i][c], b.tranzitii[i][c][0], inchideri[stare][k]) == -1) {
                            b.tranzitii[i][c][++b.tranzitii[i][c][0]] = inchideri[stare][k];
                        }
                    }
                }
            }
        }
    }
    b.q0 = a.q0;    //PASUL 3: Calcularea starilor initiale si finale
    b.k = 0;
    for (i = 0; i < b.nrStari; ++i)
        for (j = 1; j <= inchideri[i][0]; ++j)
            if (cautStare(a.stariFinale, a.k, inchideri[i][j]) == 1) {
                b.stariFinale[++b.k] = i;
                j = inchideri[i][0] + 2;
            }
    int deInlocuit[a.nrStari];  //PASUL 4: Eliminarea starilor identice
    for (i = 0; i < a.nrStari; ++i)
        deInlocuit[i] = i;
    for (i = 0; i < a.nrStari - 1; ++i)
        for (j = i + 1; j < a.nrStari; ++j)
            if (stariIdentice(b, i, j)) {
                if (i < deInlocuit[i])
                    deInlocuit[j] = i;
                else
                    deInlocuit[j] = deInlocuit[i];
            }
    int aux = a.nrStari;
    for (i = 0; i < aux; ++i)
        if (deInlocuit[i] != i) {
            eliminaLinie(b.tranzitii, b.nrStari, b.m, i);
            replace(b, i, deInlocuit[i]);
        }
}

int main() {
    automat a, b;
    citire(a);
    LNFAtoNFA(a, b);
    afisare(b);
    return 0;
}
