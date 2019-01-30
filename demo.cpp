#include <iostream>
#include <chrono>

#include "./include/BigInteger.h"

using namespace std;

int main()
{
    BigInteger bigint_1(
        "878978478678327583947589347893275849768937583743089769342057832754057"
        "645789768394758932758932674839758932768937589433422344759438738974545"
        "586476577546565473563543545436893476892758942376892374896486473543535"
        "324325323343643542325453895875978904389054396789578439678976884096834"
        "656865775426252352095825304832099534758943758943243253253244768975389"
        "243434323643543643537589644375893476892758942376892374896486473543535"
        "723564636255343542545389582475978904389054396789578439678976884096834"
        "936357390689320834095830483267099534758943758943243253253244768975389"
        "754754654654436435375894375893889476892758942376892374896486473543535"
        "3225823852809390489032869028493189023840982390589028590223523098928283"
    );

    BigInteger bigint_2(
        "324325323343643542325453895875978904389054396789578439678976884096834"
        "656865775426252352095825304832099534758943758943243253253244768975389"
        "243434323643543643537589644375893476892758942376892374896486473543535"
        "723564636255343542545389582475978904389054396789578439678976884096834"
        "656865775426252352095825304832099534758943758943243253253244768975389"
    );


    // Testing basic arithmetic computation
    cout << "Testing basic arithmetic computation... " << endl;
    cout << "------------------" << endl;

    auto start_t = chrono::steady_clock::now();
    BigInteger add_r = bigint_1 + bigint_2;
    auto add_time = chrono::steady_clock::now() - start_t;
    cout << "(1) + (2) = " << add_r << endl;
    cout << "addition time = " << chrono::duration<double, nano> (add_time).count() << "ns" << endl << endl;

    start_t = chrono::steady_clock::now();
    BigInteger product = bigint_1 * bigint_2;
    auto mul_time = chrono::steady_clock::now() - start_t;
    cout << "(1) * (2) = " << product << endl;
    cout << "multiplication time = " << chrono::duration<double, milli>(mul_time).count() << "ms" << endl << endl;

    start_t = chrono::steady_clock::now();
    BigInteger quotient = bigint_1 / bigint_2;
    auto div_time = chrono::steady_clock::now() - start_t;
    cout << "(1) / (2) = " << quotient << endl;
    cout << "division time = " << chrono::duration<double, milli>(div_time).count() << "ms" << endl << endl;

    start_t = chrono::steady_clock::now();
    BigInteger remainder = bigint_1 % bigint_2;
    auto mod_time = chrono::steady_clock::now() - start_t;
    cout << "(1) % (2) = " << remainder << endl;
    cout << "modulo time = " << chrono::duration<double, milli>(mod_time).count() << "ms" << endl << endl;

    // Verifying the consistence of the results
    cout << "Verifying the consistence of the results... " << endl;
    if (bigint_1 == (quotient * bigint_2 + remainder))
        cout << "Yes, (1) == (2) * quotient + remainder." << endl;
    else
        cout << "NoNoNo, (1) != (2) * quotient + remainder." << endl;
    cout << endl;

    // Testing User's input data
    BigInteger dividend, divisor;
    cout << "Testing User's input data..." << endl;
    cout << "Enter a dividend integer number: "; cin >> dividend;
    cout << "Enter a divisor integer number: "; cin >> divisor;
    quotient = dividend / divisor;
    remainder = dividend % divisor;
    cout  << "quotient = " << quotient << endl;
    cout  << "remainder = " << remainder << endl;
    if (dividend == (quotient * divisor + remainder))
        cout << "The dividend is consistent with quotient * divisor + remainder" << endl;
    else
        cout << "The dividend is not consistent with quotient * divisor + remainder" << endl;

    system("pause");

    return 0;
}
