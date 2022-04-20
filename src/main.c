//#include <bits/stdc++.h>

#include    <stdio.h>
#include    <stdbool.h>
#include    <stdlib.h>
#include    <stdint.h>
#include    <string.h>
#include    <math.h>
#include    "fuzzy_logic.h"

FILE *input_f;
FILE *output_f;

#define PI 3.1415926535897932384626433832795

/*************************** Fuzzy logic rules start ***************************************/
// Дистанция до линии в 2-х сантиметрах (limit +-127 = +-254см)
#define D_VERY_HIGH  (100)      // 2,0м
#define D_HIGH       (75)       // 1,5м
#define D_L1_CEN     (50)       // 1,0м
#define D_L1_TOP     (25)       // 50см
#define D_L1_BTN     (50)       // 1,0м
#define D_ZERO       (0)
#define D_LOW        (-D_HIGH)
#define D_VERY_LOW   (-D_VERY_HIGH)
#define D_Z_TOP      (1)        // 2см
#define D_Z_BTN      (10)       // 20см
#define D_R1_CEN     (-D_L1_CEN)
#define D_R1_TOP     (D_L1_TOP)
#define D_R1_BTN     (D_L1_BTN)


// Угол направления (theta) в градусах
#define T_VERY_HIGH  (115)
#define T_HIGH2      (20)
#define T_HIGH       (15)
#define T_ZERO       (0)
#define T_LOW        (-T_HIGH)
#define T_LOW2       (-T_HIGH2)
#define T_VERY_LOW   (-T_VERY_HIGH)

#define T_Z_TOP      (5)
#define T_Z_BTN      (10)
#define T_SIGMA      (10)

// Воздействие: угол направления колёс в градусах
#define TURN_H_LEFT     (25)
#define TURN_LEFT       (4)
#define NO_TURN         (0)
#define TURN_RIGHT      (-TURN_LEFT)
#define TURN_H_RIGHT    (-TURN_H_LEFT)

#define NULL_PARAM      (0)
#define NULL_OUT        (0)

// правила фуззификации (функции принадлежности)
//          name,     ffunc,    [n],  a,            b,              c,              next        
MAKE_FFUNC (d_zero,   trapecia, 0,    D_ZERO,       D_Z_TOP,        D_Z_BTN,        d_r1);     
MAKE_FFUNC (d_r1,     trapecia, 0,    D_R1_CEN,     D_R1_TOP,       D_R1_BTN,       d_r2);  
MAKE_FFUNC (d_r2,     low,      0,    D_VERY_LOW,   D_LOW,          NULL_PARAM,     d_l1);   
MAKE_FFUNC (d_l1,     trapecia, 0,    D_L1_CEN,     D_L1_TOP,       D_L1_BTN,       d_l2);   
MAKE_FFUNC (d_l2,     high,     0,    D_HIGH,       D_VERY_HIGH,    NULL_PARAM,     t_zero);   
MAKE_FFUNC (t_zero,   trapecia, 1,    T_ZERO,       T_Z_TOP,        T_Z_BTN,        t_l1);     
MAKE_FFUNC (t_l1,     triangle, 1,    T_LOW,        T_SIGMA,        NULL_PARAM,     t_l2);    
MAKE_FFUNC (t_l2,     low,      1,    T_LOW2,       T_LOW,          NULL_PARAM,     t_r1);   
MAKE_FFUNC (t_r1,     triangle, 1,    T_HIGH,       T_SIGMA,        NULL_PARAM,     t_r2);   
MAKE_FFUNC (t_r2,     high,     1,    T_HIGH,       T_HIGH2,        NULL_PARAM,     d_zero);   



// Правила нечёткой логики по данным эксперта
//        name,          A,          OPER,     B,        fin,       output,         next rule
MAKE_RULE (rule_01,      d_r2,       F_AND,    t_l2,     true,      NO_TURN,        rule_02);   // на большой дистанции угол въезда 25 градусов
MAKE_RULE (rule_02,      d_r2,       F_AND,    t_l1,     true,      TURN_LEFT,      rule_03);
MAKE_RULE (rule_03,      t_zero,     F_OR,     t_r1,     false,     NO_TURN,        rule_04);
MAKE_RULE (rule_04,      rule_03,    F_OR,     t_r2,     false,     NO_TURN,        rule_05);
MAKE_RULE (rule_05,      d_r2,       F_AND,    rule_04,  true,      TURN_H_LEFT,    rule_06);
MAKE_RULE (rule_06,      t_r2,       F_OR,     t_r1,     false,     NO_TURN,        rule_07);
MAKE_RULE (rule_07,      d_r1,       F_AND,    rule_06,  true,      TURN_H_LEFT,    rule_08);
MAKE_RULE (rule_08,      d_r1,       F_AND,    t_zero,   true,      TURN_LEFT,      rule_09);
MAKE_RULE (rule_09,      d_r1,       F_AND,    t_l1,     true,      NO_TURN,        rule_10);
MAKE_RULE (rule_10,      d_r1,       F_AND,    t_l2,     true,      TURN_RIGHT,     rule_11);
MAKE_RULE (rule_11,      d_zero,     F_AND,    t_l2,     true,      TURN_RIGHT,     rule_12);
MAKE_RULE (rule_12,      d_zero,     F_AND,    t_l1,     true,      NO_TURN,        rule_13);
MAKE_RULE (rule_13,      d_zero,     F_AND,    t_zero,   true,      NO_TURN,        rule_14);
MAKE_RULE (rule_14,      d_zero,     F_AND,    t_r1,     true,      NO_TURN,        rule_15);
MAKE_RULE (rule_15,      d_zero,     F_AND,    t_r2,     true,      TURN_LEFT,      rule_16);
MAKE_RULE (rule_16,      t_l2,       F_OR,     t_l1,     false,     NO_TURN,        rule_17);
MAKE_RULE (rule_17,      d_l1,       F_AND,    rule_16,  true,      TURN_H_RIGHT,   rule_18);
MAKE_RULE (rule_18,      d_l1,       F_AND,    t_zero,   true,      TURN_RIGHT,     rule_19);
MAKE_RULE (rule_19,      d_l1,       F_AND,    t_r1,     true,      NO_TURN,        rule_20);
MAKE_RULE (rule_20,      d_l1,       F_AND,    t_r2,     true,      TURN_LEFT,      rule_21);
MAKE_RULE (rule_21,      t_l2,       F_OR,     t_l1,     false,     NO_TURN,        rule_22);
MAKE_RULE (rule_22,      t_zero,     F_OR,     rule_21,  false,     NO_TURN,        rule_23);
MAKE_RULE (rule_23,      d_l2,       F_AND,    rule_22,  true,      TURN_H_RIGHT,   rule_24);
MAKE_RULE (rule_24,      d_l2,       F_AND,    t_r1,     true,      TURN_RIGHT,     rule_25);
MAKE_RULE (rule_25,      d_l2,       F_AND,    t_r2,     true,      NO_TURN,        rule_01);



#define MIN_0   (-250)      // min parameter #1 (delta in cm)
#define MAX_0   (250)       // max parameter #1

#define COUNT_0  (50)
#define MULT_0   ((MAX_0 - MIN_0) / COUNT_0)

#define MIN_1    (-30)      // min parameter #2 (course angle in degree)
#define MAX_1    (30)       // max parameter #2

#define COUNT_1    (60)
#define MULT_1    ((MAX_1 - MIN_1) / COUNT_1)


/*************************************************************************
 * @brief user scaling input 1 value to the limits +-127
 * input - distance in cm +-10000, limitation to +-254
 * @param in1 
 * @return int8_t 
*************************************************************************/
int8_t in0_scaling (int16_t in1)
{
    int16_t ret = in1 / 2;

    return (lim_s8 (ret));
}

/*************************************************************************
 * @brief user scaling input 2 value to the limits +-127
 * input - cource in degree +-180, limitation to +-127 degree
 * @param in2 
 * @return int8_t 
*************************************************************************/
int8_t in1_scaling (int16_t in2)
{
    int16_t ret = in2;

    return (lim_s8 (ret));
}

/*************************************************************************
 * @brief user scaling output value +-127 to the control value +-30 valve degree 
 * no scaling need
 * @param out       +-127 max
 * @return int16_t 
*************************************************************************/
int16_t out_scaling (int8_t out)
{
    return out;
}


int main()
{
    int8_t in[2], out, u;
    int16_t n, k, t;
    uint8_t z;
    (void)rule_01;  // avoid warnings
    (void)d_zero;

    fuzzy_param fuzzy = 
    {
        .in_array    = in,
        .start_ffunc = &d_zero,
        .start_rule  = &rule_01,
    };

    //input_f = fopen ("input.txt","r");
     // if (input_f == NULL)
    // {
    //     printf ("Error input!");   
    //     return (1);             
    // }
    output_f = fopen ("../output_int.txt","w");
    if (output_f == NULL)
    {
        printf ("Error output!");   
        return (1);             
    }

/// Validation fuzzification functions
/*
    /// cube
    printf ("Test fuzzy function cube with m=0, d=10\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = cube (k, 0, 10, 0);
        printf("%d\t%d\n", k, z);
    }

    /// triangle
    printf ("Test fuzzy function triangle with m=0, d=100\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = triangle (k, 0, 100, 0);
        printf("%d\t%d\n", k, z);
    }

    /// a_triangle
    printf ("Test fuzzy function a_triangle with m=20, d1=100, d2=50\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = a_triangle (k, 20, 100, 50);
        printf("%d\t%d\n", k, z);
    }

    /// square
    printf ("Test fuzzy function square with m=-10, d1=50\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = square (k, -10, 50, 0);
        printf("%d\t%d\n", k, z);
    }

    /// trapecia
    printf ("Test fuzzy function trapecia with m=-15, d1=10, d2=40\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = trapecia (k, -15, 10, 40);
        printf("%d\t%d\n", k, z);
    }

    /// low
    printf ("Test fuzzy function low with min=-25, max=25\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = low (k, -25, 25, 0);
        printf("%d\t%d\n", k, z);
    }

    /// high
    printf ("Test fuzzy function high with min=-5, max=55\n");
    printf ("in\tout\n");
    for (k = -127; k < 128; k++)
    {
        z = high (k, -5, 55, 0);
        printf("%d\t%d\n", k, z);
    }
*/

    printf ("Test fuzzy logic controller\n");
    fprintf (output_f, "\t");
    for (k = 0; k <= COUNT_0; k++)
    {
      // масштабированиение входных величин
        t = MIN_0 + (k * MULT_0);
        in[0] = in0_scaling (t);       // 2cm in the 1lsb
        fprintf (output_f, "%d\t", t);
    }
    fprintf (output_f, "\n");

    for (n = 0; n <= COUNT_1; n++)
    {
        t = MIN_1 + (n * MULT_1);
        in[1] = in1_scaling (t); 
        fprintf (output_f, "%d\t", t);

        for (k = 0; k <= COUNT_0; k++)
        {
            t = MIN_0 + (k * MULT_0);
            in[0] = in0_scaling (t);
            u = process_fuzzy_logic (&fuzzy);
            out = out_scaling (u);
            fprintf (output_f, "%d\t", out);
        }

        fprintf (output_f, "\n");
    }


    printf ("Test fuzzy logic controller end\n");

    // fclose (input_f);
    fclose (output_f);
}

