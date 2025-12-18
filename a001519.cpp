// Compile : g++ -O2 a001519.cpp -o a001519 `pkg-config --cflags --libs flint`
#include <iostream>
#include <string>
#include <flint/fmpz.h>
#include <flint/arb.h>

using namespace std;

/* précision binaire: 10000 chiffres décimaux ≈ 10000 * log2(10) ≈ 33219 bits + marge de sécurité */
#define PRECISION_BITS 35000

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <n-as-decimal-string>" << endl;
        return 1;
    }

    const slong prec = PRECISION_BITS;
    fmpz_t n, n_2;
    fmpz_init(n);
    if (fmpz_set_str(n, argv[1], 10) != 0) {
        cerr << "Error: invalid value for n" << endl;
        return 2;
    }

    arb_t a, t1, t2, t3, t4, x1, x2, x3, x4, r;
    arb_init(a);
    arb_init(t1);
    arb_init(t2);
    arb_init(t3);
    arb_init(t4);
    arb_init(x1);
    arb_init(x2);
    arb_init(x3);
    arb_init(x4);
    arb_init(r);

    fmpz_init(n_2);
    fmpz_sub_ui(n_2, n, 2);

    arb_set_si(a, 5);
    arb_sqrt(a, a, prec);

    /* x1 = (5 + a) / 10 */
    arb_set_si(x1, 5);
    arb_add(x1, x1, a, prec);
    arb_div_ui(x1, x1, 10, prec);

    /* x2 = (5 - a) / 10 */
    arb_set_si(x2, 5);
    arb_sub(x2, x2, a, prec);
    arb_div_ui(x2, x2, 10, prec);

    /* x3 = (3 + a) / 2 */
    arb_set_si(x3, 3);
    arb_add(x3, x3, a, prec);
    arb_div_ui(x3, x3, 2, prec);

    /* x4 = (3 - a) */
    arb_set_si(x4, 3);
    arb_sub(x4, x4, a, prec);
    arb_div_ui(x4, x4, 2, prec);

    /* t1 = x3^(n-2) */
    arb_pow_fmpz(t1, x3, n_2, prec);

    /* t2 = x4^(n-2) */
    arb_pow_fmpz(t2, x4, n_2, prec);

    /* t3 = x1*t1 */
    arb_mul(t3, x1, t1, prec);

    /* t4 = x2*t2 */
    arb_mul(t4, x2, t2, prec);

    /* r = (t3 + t4) */
    arb_add(r, t3, t4, prec);

    /* Affichage haute précision */
    cout << "raw = ";
    arb_printn(r, prec, 0);
    cout << endl;

    /* Arrondi contrôlé */
    arf_t mid;
    arf_init(mid);

    /* mid = milieu exact de r */
    arf_set(mid, arb_midref(r));

    fmpz_t u;
    fmpz_init(u);

    /* arrondi au plus proche */
    arf_get_fmpz(u, mid, ARF_RND_NEAR);

    /* Vérification de proximité */
    arb_t diff, tmp;
    arb_init(diff);
    arb_init(tmp);

    arb_set_fmpz(tmp, u);
    arb_sub(diff, r, tmp, prec);
    arb_abs(diff, diff);

    /* tolérance ≈ 10^{-300} */
    arb_t tol;
    arb_init(tol);
    arb_set_ui(tol, 1);
    arb_mul_2exp_si(tol, tol, -1000);

    if (!arb_lt(diff, tol)) {
        cerr << "Warning: raw value may not be close enough to an integer or precision too low" << endl;
    }

    cout << "a(";
    fmpz_print(n);
    cout << ") = ";
    fmpz_print(u);
    cout << endl;

    arf_clear(mid);
    arb_clear(diff);
    arb_clear(tmp);
    arb_clear(tol);
    fmpz_clear(u);

    flint_cleanup();
    return 0;
}
