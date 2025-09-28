/**
****************************************************************************************
* @file    gh3x_bia_impedance_dixer.c
* @author  GHealth Driver Team
* @brief   gh3x driver example
****************************************************************************************
* @attention
#####Copyright (c) 2024 GOODIX
All rights reserved.
****************************************************************************************
*/

#include <stdio.h>
#include <math.h>
#include "stdint.h"
#include "string.h"
#include "gh3x_bia_impedance_fixer.h"

#define pi (3.1415926)
#define BIA_FS_250K (250)

complex_double add(complex_double a, complex_double b) {
    complex_double res;
    res.real = a.real + b.real;
    res.imag = a.imag + b.imag;
    return res;
}

complex_double subtract(complex_double a, complex_double b) {
    complex_double res;
    res.real = a.real - b.real;
    res.imag = a.imag - b.imag;
    return res;
}

complex_double multiply(complex_double a, complex_double b) {
    complex_double res;
    res.real = a.real * b.real - a.imag * b.imag;
    res.imag = a.real * b.imag + a.imag * b.real;
    return res;
}

complex_double multiply3(complex_double a, complex_double b, complex_double c) {
    complex_double tmp = multiply(a, b);
    complex_double res = multiply(tmp, c);
    return res;
}

complex_double divide(complex_double a, complex_double b) {
    complex_double res;
    double denom = b.real * b.real + b.imag * b.imag;
    if (denom)
    {
        res.real = (a.real * b.real + a.imag * b.imag) / denom;
        res.imag = (a.imag * b.real - a.real * b.imag) / denom;
    }
    else
    {
        res = b;
    }
    return res;
}

complex_double ZcontactCal(complex_double a, complex_double b) {
    complex_double tmp = multiply(a, b);
    complex_double res = subtract(a, b);
    res = divide(tmp, res);
    return res;
}

double magnitude(complex_double a) {
    return sqrt(a.real * a.real + a.imag * a.imag);
}

complex_double add3(complex_double a, complex_double b, complex_double c)
{
    complex_double res = add(add(a, b), c);
    return res;
}

complex_double add4(complex_double a, complex_double b, complex_double c, complex_double d)
{
    complex_double res = add(add(add(a, b), c), d);
    return res;
}

complex_double add6(complex_double a, complex_double b, complex_double c, complex_double d, complex_double e, complex_double f)
{
    complex_double res = add4(a, b, c, d);
    res = add3(res, e, f);
    return res;
}

complex_double multiplynegtive1(complex_double a)
{
    complex_double negtive1;
    negtive1.real = -1;
    negtive1.imag = 0;
    complex_double res = multiply(a, negtive1);
    return res;
}

complex_double BiaImpedanceFixerZbodyFixExeMolecular(STGHxBIAInfo stghBIAInfo)
{
    complex_double temMolecular;
    complex_double w = stghBIAInfo.stBIAW;
    complex_double w_2 = multiply(w, w);
    complex_double w_3 = multiply(w_2, w);
    complex_double temMolecular_w0 = multiplynegtive1(multiply(stghBIAInfo.stBIAMeasure.V1B2A, stghBIAInfo.stBIAPara.k10));

    complex_double temDenominator_w1_part1 = add4(\
        multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAPara.k2), \
        multiply(stghBIAInfo.stBIAMeasure.ZEL1B, stghBIAInfo.stBIAPara.k3), \
        multiply(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAPara.k4), \
        multiply(stghBIAInfo.stBIAMeasure.ZEL2B, stghBIAInfo.stBIAPara.k5));
    complex_double temDenominator_w1_part2 = add4(\
        multiplynegtive1(multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), stghBIAInfo.stBIAPara.k6)), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL2A), stghBIAInfo.stBIAPara.k7), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1B, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k8), \
        multiplynegtive1(multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k9)));
    complex_double temDenominator_w1 = add(\
        multiplynegtive1(multiply(multiply(temDenominator_w1_part1, w), stghBIAInfo.stBIAMeasure.V1B2A)), \
        multiply(temDenominator_w1_part2, w));


    complex_double temDenominator_w2_part1 = add6(\
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), stghBIAInfo.stBIAPara.k1), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL2A), stghBIAInfo.stBIAPara.k11), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k12), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1B, stghBIAInfo.stBIAMeasure.ZEL2A), stghBIAInfo.stBIAPara.k13), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL1B, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k14), \
        multiply(multiply(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k15));
    complex_double temDenominator_w2_part2 = add(\
        multiply(add(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), multiply(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B)), \
        multiply(add(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B), multiply(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B)));

    complex_double temDenominator_w2 = add(\
        multiply(multiply(temDenominator_w2_part1, w_2), stghBIAInfo.stBIAMeasure.V1B2A), \
        multiplynegtive1(multiply(multiply(temDenominator_w2_part2, w_2), stghBIAInfo.stBIAPara.k16)));

    complex_double temDenominator_w3_part3 = temDenominator_w2_part2;

    complex_double temDenominator_w3 = multiply(multiply(multiply(temDenominator_w3_part3, w_3), stghBIAInfo.stBIAPara.k17), stghBIAInfo.stBIAMeasure.V1B2A);

    temMolecular = add4(temMolecular_w0, temDenominator_w1, temDenominator_w2, temDenominator_w3);
    return temMolecular;
}

complex_double BiaImpedanceFixerZbodyFixExeDenominator(STGHxBIAInfo stghBIAInfo)
{
    complex_double temDenominator;
    complex_double w = stghBIAInfo.stBIAW;
    complex_double w_2 = multiply(w, w);
    complex_double w_3 = multiply(w_2, w);
    complex_double temDenominator_w0 = stghBIAInfo.stBIAPara.k10;
    complex_double temDenominator_w1 = add3(\
        multiply(multiply(stghBIAInfo.stBIAMeasure.V1B2A, stghBIAInfo.stBIAPara.k18), w), \
        multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), stghBIAInfo.stBIAPara.k6), w), \
        multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k9), w));

    complex_double temDenominator_w2 = add3(\
        multiplynegtive1(multiply(multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), stghBIAInfo.stBIAPara.k1), stghBIAInfo.stBIAMeasure.V1B2A), w_2)), \
        multiplynegtive1(multiply(multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B), stghBIAInfo.stBIAPara.k15), stghBIAInfo.stBIAMeasure.V1B2A), w_2)), \
        multiply(multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), add(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B)), stghBIAInfo.stBIAPara.k16), w_2));

    complex_double temDenominator_w3 = multiplynegtive1(multiply(multiply(multiply(multiply(add(stghBIAInfo.stBIAMeasure.ZEL1A, stghBIAInfo.stBIAMeasure.ZEL1B), add(stghBIAInfo.stBIAMeasure.ZEL2A, stghBIAInfo.stBIAMeasure.ZEL2B)), stghBIAInfo.stBIAPara.k17), stghBIAInfo.stBIAMeasure.V1B2A), w_3));

    temDenominator = add4(temDenominator_w0, temDenominator_w1, temDenominator_w2, temDenominator_w3);
    return temDenominator;
}

complex_double BiaImpedanceFixerZbodyFixExe(STGHxBIAInfo stghBIAInfo)
{
    complex_double temDenominator;
    complex_double temMolecular;

    temMolecular = BiaImpedanceFixerZbodyFixExeMolecular(stghBIAInfo);
    temDenominator = BiaImpedanceFixerZbodyFixExeDenominator(stghBIAInfo);

    return divide(temMolecular, temDenominator);
}

complex_double BiaGetZbodyImpedanceFixerWeight(GU16 uchFreq)
{
    complex_double temW;
    temW.real = 2 * pi*uchFreq * 1000;
    temW.imag = 0;
    return temW;
}

STBIAParameter BiaGetZbodyImpedanceFixerParameter(GU16 uchFreq)
{
    STBIAParameter stBIAZbodyFixParameter;
#ifdef  GR5526_SK  ////EVK 
    stBIAZbodyFixParameter.k1 = (complex_double){ 2985873534865, 0 };
    stBIAZbodyFixParameter.k2 = (complex_double){ 0, 3.80716150000000e+23 };
    stBIAZbodyFixParameter.k3 = (complex_double){ 0, 3.87938175000000e+23 };
    stBIAZbodyFixParameter.k4 = (complex_double){ 0, 3.98749125000000e+23 };
    stBIAZbodyFixParameter.k5 = (complex_double){ 0, 3.78971800000000e+23 };
    stBIAZbodyFixParameter.k6 = (complex_double){ 0, 1.24777500000000e+23 };
    stBIAZbodyFixParameter.k7 = (complex_double){ 0, 1.47569700000000e+23 };
    stBIAZbodyFixParameter.k8 = (complex_double){ 0, 1.41292050000000e+23 };
    stBIAZbodyFixParameter.k9 = (complex_double){ 0, 1.29310800000000e+23 };
    stBIAZbodyFixParameter.k10 = (complex_double){ 4.42500000000000e+34, 0 };
    stBIAZbodyFixParameter.k11 = (complex_double){ 2938608255995, 0 };
    stBIAZbodyFixParameter.k12 = (complex_double){ 2995183171310, 0 };
    stBIAZbodyFixParameter.k13 = (complex_double){ 3160181832225, 0 };
    stBIAZbodyFixParameter.k14 = (complex_double){ 2871281018650, 0 };
    stBIAZbodyFixParameter.k15 = (complex_double){ 3037138771820, 0 };
    stBIAZbodyFixParameter.k16 = (complex_double){ 106561516020, 0 };
    stBIAZbodyFixParameter.k17 = (complex_double){ 0, 17.412224357403487 };
    stBIAZbodyFixParameter.k18 = (complex_double){ 0, 5.19099325000000e+23 };
#else  
    if (BIA_FS_250K <= uchFreq)
    {   // para k   demo f6f9
//        stBIAZbodyFixParameter.k1 =  (complex_double)  {679964659.5,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double)  {0,5.87840000000e+19};
//        stBIAZbodyFixParameter.k3 =  (complex_double)  {0,6.31082187500e+19};
//        stBIAZbodyFixParameter.k4 =  (complex_double)  {0,6.70719062500e+19};
//        stBIAZbodyFixParameter.k5 =  (complex_double)  {0,7.37320625000e+19}; 
//        stBIAZbodyFixParameter.k6 =  (complex_double)  {0,1.69906250000e+19};
//        stBIAZbodyFixParameter.k7 =  (complex_double)  {0,2.14406250000e+19};
//        stBIAZbodyFixParameter.k8 =  (complex_double)  {0,2.69328125000e+19};
//        stBIAZbodyFixParameter.k9 =  (complex_double)  {0,2.64465000000e+19};
//        stBIAZbodyFixParameter.k10 = (complex_double)  {5.031250e+30,0};
//        stBIAZbodyFixParameter.k11 = (complex_double)  {692284131.5,0};
//        stBIAZbodyFixParameter.k12 = (complex_double)  {779136622.0,0};
//        stBIAZbodyFixParameter.k13 = (complex_double)  {768145629.8125,0};
//        stBIAZbodyFixParameter.k14 = (complex_double)  {780665389.3125,0};
//        stBIAZbodyFixParameter.k15 = (complex_double)  {843912073.8125,0};
//        stBIAZbodyFixParameter.k16 = (complex_double)  {25463606.25,0};
//        stBIAZbodyFixParameter.k17 = (complex_double)  {0,0.006511690808701};
//        stBIAZbodyFixParameter.k18 = (complex_double)  {0,8.7910968750000e+19};
    }
    else
    {     // para k   demo f6f9
//        stBIAZbodyFixParameter.k1 =  (complex_double)  {17128101510270.0,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double)  {0,6.49341000e+23};
//        stBIAZbodyFixParameter.k3 =  (complex_double)  {0,6.79191810e+23};
//        stBIAZbodyFixParameter.k4 =  (complex_double)  {0,5.74818910e+23};
//        stBIAZbodyFixParameter.k5 =  (complex_double)  {0,7.30368100e+23}; 
//        stBIAZbodyFixParameter.k6 =  (complex_double)  {0,2.16981000e+23};
//        stBIAZbodyFixParameter.k7 =  (complex_double)  {0,2.01350000e+23};
//        stBIAZbodyFixParameter.k8 =  (complex_double)  {0,2.94050000e+23};
//        stBIAZbodyFixParameter.k9 =  (complex_double)  {0,2.05308100e+23};
//        stBIAZbodyFixParameter.k10 = (complex_double)  {2.3e34,0};
//        stBIAZbodyFixParameter.k11 = (complex_double)  {14465724492970.0,0};
//        stBIAZbodyFixParameter.k12 = (complex_double)  {18299666622700.0,0};
//        stBIAZbodyFixParameter.k13 = (complex_double)  {15744966864577.0,0};
//        stBIAZbodyFixParameter.k14 = (complex_double)  {17808462143707.0,0};
//        stBIAZbodyFixParameter.k15 = (complex_double)  {16420781705007.0,0};
//        stBIAZbodyFixParameter.k16 = (complex_double)  {637348289300.0,0};
//        stBIAZbodyFixParameter.k17 = (complex_double)  {0,313.528221884016669};
//        stBIAZbodyFixParameter.k18 = (complex_double)  {0,8.9457081e+23};
    }
    
//        // para k   demo F73D
//        stBIAZbodyFixParameter.k1 =  (complex_double){2774490.85,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double){0,313512500000000000.0};
//        stBIAZbodyFixParameter.k3 =  (complex_double){0,258511500000000000.0};
//        stBIAZbodyFixParameter.k4 =  (complex_double){0,219284000000000000.0};
//        stBIAZbodyFixParameter.k5 =  (complex_double){0,376635000000000000.0};
//        stBIAZbodyFixParameter.k6 =  (complex_double){0,82637500000000000.0};
//        stBIAZbodyFixParameter.k7 =  (complex_double){0,72325000000000000.0};
//        stBIAZbodyFixParameter.k8 =  (complex_double){0,123500000000000000.0};
//        stBIAZbodyFixParameter.k9 =  (complex_double){0,94585000000000000.0};
//        stBIAZbodyFixParameter.k10 = (complex_double){26750000000000000000000000000.0,0};
//        stBIAZbodyFixParameter.k11 = (complex_double){2374481.1,0};
//        stBIAZbodyFixParameter.k12 = (complex_double){3474455.25,0};
//        stBIAZbodyFixParameter.k13 = (complex_double){2016613.08,0};
//        stBIAZbodyFixParameter.k14 = (complex_double){3069616.03,0};
//        stBIAZbodyFixParameter.k15 = (complex_double){2753035.78,0};
//        stBIAZbodyFixParameter.k16 = (complex_double){41714.75,0};
//        stBIAZbodyFixParameter.k17 = (complex_double){0,0.000018777479497};
//        stBIAZbodyFixParameter.k18 = (complex_double){0,406749000000000000.0};
        
        // para k   demo F570
        stBIAZbodyFixParameter.k1 =  (complex_double){1347637970637698696116362436.5,0};
        stBIAZbodyFixParameter.k2 =  (complex_double){0,1.86116341698339838462355e38};
        stBIAZbodyFixParameter.k3 =  (complex_double){0,1.236791758992391012352e38};
        stBIAZbodyFixParameter.k4 =  (complex_double){0,1.43754903764840929134995e38};
        stBIAZbodyFixParameter.k5 =  (complex_double){0,2.100806503078490537984e38};
        stBIAZbodyFixParameter.k6 =  (complex_double){0,4.476645020650730160128e37};
        stBIAZbodyFixParameter.k7 =  (complex_double){0,5.1054632992895724521875e37};
        stBIAZbodyFixParameter.k8 =  (complex_double){0,5.299892336484941824e37};
        stBIAZbodyFixParameter.k9 =  (complex_double){0,6.67864684440628232192e37};
        stBIAZbodyFixParameter.k10 = (complex_double){1.55937137097703424e49,0};
        stBIAZbodyFixParameter.k11 = (complex_double){1548608733578286757614922786.5,0};
        stBIAZbodyFixParameter.k12 = (complex_double){1984531008166817169372804330.0,0};
        stBIAZbodyFixParameter.k13 = (complex_double){1097106385075050032560266436.5,0};
        stBIAZbodyFixParameter.k14 = (complex_double){1486093451241071504995123200.0,0};
        stBIAZbodyFixParameter.k15 = (complex_double){1650645367722947930967438570.0,0};
        stBIAZbodyFixParameter.k16 = (complex_double){-18209423229355817763122490.0,0};
        stBIAZbodyFixParameter.k17 = (complex_double){0,10096682195008006.883939333288259};
        stBIAZbodyFixParameter.k18 = (complex_double){0,2.20262617184564336494995e38};
        
//        // para k   demo F898
//	    stBIAZbodyFixParameter.k1 =  (complex_double){114876249051.5,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double){0,12685356750000000000000.0};
//        stBIAZbodyFixParameter.k3 =  (complex_double){0,9233456250000000000000.0};
//        stBIAZbodyFixParameter.k4 =  (complex_double){0,10783693750000000000000.0};
//        stBIAZbodyFixParameter.k5 =  (complex_double){0,14275594250000000000000.0};
//        stBIAZbodyFixParameter.k6 =  (complex_double){0,2952156250000000000000.0};
//        stBIAZbodyFixParameter.k7 =  (complex_double){0,3791718750000000000000.0};
//        stBIAZbodyFixParameter.k8 =  (complex_double){0,3811718750000000000000.0};
//        stBIAZbodyFixParameter.k9 =  (complex_double){0,4522393750000000000000.0};
//        stBIAZbodyFixParameter.k10 = (complex_double){943750000000000000000000000000000.0,0};
//        stBIAZbodyFixParameter.k11 = (complex_double){129714300633.0,0};
//        stBIAZbodyFixParameter.k12 = (complex_double){154479258801.5,0};
//        stBIAZbodyFixParameter.k13 = (complex_double){99043107712.5,0};
//        stBIAZbodyFixParameter.k14 = (complex_double){124274304764.0,0};
//        stBIAZbodyFixParameter.k15 = (complex_double){141448043720.5,0};
//        stBIAZbodyFixParameter.k16 = (complex_double){1167843687.5,0};
//        stBIAZbodyFixParameter.k17 = (complex_double){0,1.014640847125367};
//        stBIAZbodyFixParameter.k18 = (complex_double){0,16014500500000000000000.0};

////        // para k   demo F6DA
//	    stBIAZbodyFixParameter.k1 =  (complex_double){119025.030625,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double){0,16140125000000000.0};
//        stBIAZbodyFixParameter.k3 =  (complex_double){0,18017000000000000.0};
//        stBIAZbodyFixParameter.k4 =  (complex_double){0,17086250000000000.0};
//        stBIAZbodyFixParameter.k5 =  (complex_double){0,18279000000000000.0};
//        stBIAZbodyFixParameter.k6 =  (complex_double){0,5448437500000000.0};
//        stBIAZbodyFixParameter.k7 =  (complex_double){0,5540312500000000.0};
//        stBIAZbodyFixParameter.k8 =  (complex_double){0,7075125000000000.0};
//        stBIAZbodyFixParameter.k9 =  (complex_double){0,6052500000000000.0};
//        stBIAZbodyFixParameter.k10 = (complex_double){2193750000000000000000000000.0,0};
//        stBIAZbodyFixParameter.k11 = (complex_double){111716.990625,0};
//        stBIAZbodyFixParameter.k12 = (complex_double){122388.0025,0};
//        stBIAZbodyFixParameter.k13 = (complex_double){126570.990625,0};
//        stBIAZbodyFixParameter.k14 = (complex_double){127305.0025,0};
//        stBIAZbodyFixParameter.k15 = (complex_double){125669.2,0};
//        stBIAZbodyFixParameter.k16 = (complex_double){2836.11875,0};
//        stBIAZbodyFixParameter.k17 = (complex_double){0,0.000000642000597};
//        stBIAZbodyFixParameter.k18 = (complex_double){0,23260250000000000.0};

        // para k   demo F679 /F6E3 / F7B8
//        stBIAZbodyFixParameter.k1 =  (complex_double){11052407.65,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double){0,1400875000000000000.0};
//        stBIAZbodyFixParameter.k3 =  (complex_double){0,1554275000000000000.0};
//        stBIAZbodyFixParameter.k4 =  (complex_double){0,1618480000000000000.0};
//        stBIAZbodyFixParameter.k5 =  (complex_double){0,1583930000000000000.0};
//        stBIAZbodyFixParameter.k6 =  (complex_double){0,476020000000000000.0};
//        stBIAZbodyFixParameter.k7 =  (complex_double){0,557325000000000000.0};
//        stBIAZbodyFixParameter.k8 =  (complex_double){0,616750000000000000.0};
//        stBIAZbodyFixParameter.k9 =  (complex_double){0,599650000000000000.0};
//        stBIAZbodyFixParameter.k10 = (complex_double){176500000000000000000000000000.0,0};
//        stBIAZbodyFixParameter.k11 = (complex_double){11085988.75,0};
//        stBIAZbodyFixParameter.k12 = (complex_double){11806286.9,0};
//        stBIAZbodyFixParameter.k13 = (complex_double){13045757.15,0};
//        stBIAZbodyFixParameter.k14 = (complex_double){11793100.5,0};
//        stBIAZbodyFixParameter.k15 = (complex_double){12487132.6,0};
//        stBIAZbodyFixParameter.k16 = (complex_double){330225.5,0};
//        stBIAZbodyFixParameter.k17 = (complex_double){0,0.000068413488133};
//        stBIAZbodyFixParameter.k18 = (complex_double){0,2003110000000000000.0};

////        // para k   demo f5e4
//	    stBIAZbodyFixParameter.k1 =  (complex_double) {1824930682.5,0};
//        stBIAZbodyFixParameter.k2 =  (complex_double) {0,246785875000000000000.0};
//        stBIAZbodyFixParameter.k3 =  (complex_double) {0,188584375000000000000.0};
//        stBIAZbodyFixParameter.k4 =  (complex_double) {0,158348690000000000000.0};
//        stBIAZbodyFixParameter.k5 =  (complex_double) {0,267715190000000000000.0};
//        stBIAZbodyFixParameter.k6 =  (complex_double) {0,66555875000000000000.0};
//        stBIAZbodyFixParameter.k7 =  (complex_double) {0,60360000000000000000.0};
//        stBIAZbodyFixParameter.k8 =  (complex_double) {0,85942500000000000000.0};
//        stBIAZbodyFixParameter.k9 =  (complex_double) {0,61902690000000000000.0};
//        stBIAZbodyFixParameter.k10 = (complex_double) {23075000000000000000000000000000.0,0};
//        stBIAZbodyFixParameter.k11 = (complex_double) {1535639888.05,0};
//        stBIAZbodyFixParameter.k12 = (complex_double) {2240498830.55,0};
//        stBIAZbodyFixParameter.k13 = (complex_double) {1237698346.25,0};
//        stBIAZbodyFixParameter.k14 = (complex_double) {1867856488.75,0};
//        stBIAZbodyFixParameter.k15 = (complex_double) {1671090210.2,0};
//        stBIAZbodyFixParameter.k16 = (complex_double) {46262257.95,0};
//        stBIAZbodyFixParameter.k17 = (complex_double) {0,0.010084840444319};
//        stBIAZbodyFixParameter.k18 = (complex_double) {0,302258500000000000000.0};
    
#endif
    return stBIAZbodyFixParameter;
}

/**
* @brief fix Zobody value
*
* @param pstImpedanceNetValue: point of bia Z value
* @param uchFreqIndex: fs
* @param pstFixedZbody: point of fixed Z body value
* @return BID_OFFSET_IMP_FIXED_E code
*/
BID_OFFSET_IMP_FIXED_E BiaImpedanceFixerZbodyFix(complex_double *pstImpedanceNetValue, GU16 uchFreq, complex_double *pstFixedZbody)
{
    if (NULL == pstImpedanceNetValue || uchFreq <= 0 || NULL == pstFixedZbody)
    {
        return BIA_OFFSET_IMP_FIXED_FAILED;
    }

    STGHxBIAInfo stghBIAInfo;
    stghBIAInfo.stBIAW = BiaGetZbodyImpedanceFixerWeight(uchFreq);
    ////  The daughter board hardware connections of EVK and demo are different, and contact impedance input calibration model is different
    // #ifdef  GR5526_SK   //The contact impedance of the EVK input calibration model is measured
    stghBIAInfo.stBIAMeasure.ZEL2A = pstImpedanceNetValue[ZNewValueTxpZ4Index];
    stghBIAInfo.stBIAMeasure.ZEL1B = pstImpedanceNetValue[ZNewValueRxnZ3Index];
    stghBIAInfo.stBIAMeasure.ZEL2B = pstImpedanceNetValue[ZNewValueTxnZ2Index];
    stghBIAInfo.stBIAMeasure.ZEL1A = pstImpedanceNetValue[ZNewValueRxpZ1Index];
    // #else //// The demo exchanges the measured 1A and 2A and then inputs them into the calibration model
    // stghBIAInfo.stBIAMeasure.ZEL2A = pstImpedanceNetValue[ZNewValueRxpZ1Index];
    // stghBIAInfo.stBIAMeasure.ZEL1B = pstImpedanceNetValue[ZNewValueTxnZ2Index];
    // stghBIAInfo.stBIAMeasure.ZEL2B = pstImpedanceNetValue[ZNewValueRxnZ3Index];
    // stghBIAInfo.stBIAMeasure.ZEL1A = pstImpedanceNetValue[ZNewValueTxpZ4Index];
    // #endif
    stghBIAInfo.stBIAMeasure.V1B2A = pstImpedanceNetValue[ZNewValueZbodyIndex];
    
    STBIAParameter stBIAZbodyFixParameter = BiaGetZbodyImpedanceFixerParameter(uchFreq);
    GH3X_Memcpy((void *)&stghBIAInfo.stBIAPara, (void *)&stBIAZbodyFixParameter, sizeof(STBIAParameter));

    *pstFixedZbody = BiaImpedanceFixerZbodyFixExe(stghBIAInfo);

    return BIA_OFFSET_IMP_FIXED_OK;;
}

/**
* @brief fix Z open value
*
* @param pstIQ: point of bia IQ
* @param stImpedanceOpenParam: bia open para
* @param uchFreqIndex: fs
* @param pstContactImpedance: point of fixed Z value
* @return BID_OFFSET_IMP_FIXED_E code
* @note
* 1.pstIQ should be the average value
* 2. pstImpedanceOpenParam should multiply times outside,
*       float times = 250.0/uchSlt0Fsl;
*    Z_open.imag = Z_open.imag*times;
*/
BID_OFFSET_IMP_FIXED_E BiaImpedanceFixerContactFixViaOpenParam(complex_double *pstIQ, complex_double stImpedanceOpenParam, GU16 uchFreq, complex_double *pstContactImpedance)
{
    if ( NULL == pstIQ || NULL == pstContactImpedance)
    {
        return BIA_OFFSET_IMP_FIXED_FAILED;
    }

    float usCapValue = 44.27;

	if( uchFreq < BIA_FS_250K)
	{
	  float times = (float)(BIA_FS_250K)/uchFreq;
	  stImpedanceOpenParam.imag = stImpedanceOpenParam.imag*times;
	  stImpedanceOpenParam.real = 0;
	} 
			  
    complex_double Z_rxp0 = { (float)(pstIQ->real), (float)(pstIQ->imag) + 159115 / (uchFreq*usCapValue) };

    *pstContactImpedance = ZcontactCal(stImpedanceOpenParam, Z_rxp0);

    return BIA_OFFSET_IMP_FIXED_OK;
}
