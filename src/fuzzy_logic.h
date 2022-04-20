/*******************************************************************************
* \file     fuzzy_logic.h
* \author   Ilya Petrukhin (ilya.petrukhin@gmail.com)
* \brief 
* \version  2.0
* \date     2022-04-14
*******************************************************************************/

#ifndef _FUZZY_LOGIC_H_
#define _FUZZY_LOGIC_H_

/*******************************************************************************
* Rules to using fuzzy functions library
*******************************************************************************/
// 1. Define input values in the range +-127 (scale +-1.0f)
//  Need scaling input values to the range +-127
//  --------- Parameter #1 - input error --------------------------------
//  #define IN_VERY_HIGH  127             // 1.0f
//  #define IN_HIGH       32              // 0.25f
//  #define IN_LOW        -IN_HIGH        // -0.25f
//  #define IN_VERY_LOW   -IN_VERY_HIGH   // -1.0f
//  #define IN_ZERO       0               // 0.0f
//  #define IN_Z_TOP      25              // 0.2f
//  #define IN_Z_BTN      32              // 0.25f
//  --------- Parameter #2 - input delta error ---------------------------
//  #define DI_ZERO       0               // 0.0f
//  #define DI_ZERO_D     25              // 0.2f
//  #define DI_VERY_HIGH  63              // 0.5f
//  #define DI_VERY_LOW   -DI_VERY_HIGH   // -0.5f
   
// 2. Define output values in the range +-127 (after scaling if need) 
//  #define OUT_VERY_LOW  -127    // -100.0f
//  #define OUT_LOW       -13     // -10.0f
//  #define OUT_ZERO      0       // 0.0f
//  #define OUT_HIGH      13      // 10.0f
//  #define OUT_VERY_HIGH 127     // 100.0f

// 3. Define fuzzy input functions and register it into library
// each function have a link to the next, last function have a link to the first
// parameter "n" correspond to input parameters array number
// функции фуззификации 
//            name,     ffunc,    n,    a,            b,              c,              next        
//MAKE_FFUNC (mu_zero,  trapecia, 0,    IN_ZERO,      IN_Z_TOP,       IN_Z_BTN,       mu_low);      // нулевое значение отклонения ZERO
//MAKE_FFUNC (mu_low,   left,     0,    IN_VERY_LOW,  IN_VERY_HIGH,   NULL_PARAM,     mu_high);     // отклонение ниже нормы       LOW
//MAKE_FFUNC (mu_high,  right,    0,    IN_VERY_LOW,  IN_VERY_HIGH,   NULL_PARAM,     d_zero);      // отклонение выше нормы       HIGH
//MAKE_FFUNC (d_zero,   cube,     1,    DI_ZERO,      DI_ZERO_D,      NULL_PARAM,     d_low);       // нулевое значение скорости   ZERO
//MAKE_FFUNC (d_low,    left,     1,    DI_VERY_LOW,  DI_VERY_HIGH,   NULL_PARAM,     d_high);      // отклонение снижается        LOW
//MAKE_FFUNC (d_high,   right,    1,    DI_VERY_LOW,  DI_VERY_HIGH,   NULL_PARAM,     mu_zero);     // отклонение повышается       HIGH
   
// 4. Define fuzzy rules, maybe with some logic functions
// each rule have a link to the next, last rule have a link to the first
// complex rules have a some rows, and have links to the previous partially results
// Правила нечёткой логики по данным эксперта
//          name,          A,          OPER,     B,         fin,       output,        next rule
//MAKE_RULE (rule_zero,    mu_zero,    F_AND,    d_zero,    true,      OUT_ZERO,      rule_low);
//MAKE_RULE (rule_low,     mu_low,     F_OR,     d_zero,    true,      OUT_VERY_LOW,  rule_plow);
//MAKE_RULE (rule_plow,    mu_low,     F_OR,     d_low,     false,     OUT_ZERO,      rule_plow2);
//MAKE_RULE (rule_plow2,   rule_plow,  F_AND,    d_zero,    true,      OUT_LOW,       rule_high);
//MAKE_RULE (rule_high,    mu_high,    F_OR,     d_zero,    true,      OUT_VERY_HIGH, rule_zero);
  
// 5. Scale input values and put it into array and start process from the first rule
// put all values into fuzzy param structure
//  int8_t in[2];
//  fuzzy_param fuzzy = 
// {
//   .in_array    = in,
//   .start_ffunc = &mu_zero,
//   .start_rule  = &rule_zero,
// };
//
// input variables need to scale within +-127 range
//  in[0] = in0_scaling (in_err);
//  in[1] = in1_scaling (delta_err);
// 
// out variable also need to scale to output control value:
//  int8_t temp = process_fuzzy_logic (&fuzzy)
//  int16_t out = out_scaling (temp);
//
// ***************** end of the brief *****************************************
   
/// Fuzzy logic operators
typedef enum 
{
  F_AND  = 0,   ///< Fuzzy AND: a AND b = min(a,b)
  F_OR,         ///< Fuzzy OR: a OR b = max(a,b)
  F_NOT,        ///< Fuzzy NOT: NOT a = 1-a
  F_IMP,        ///< Fuzzy IMP: a -> b = b/a 
  F_A,          ///< Out is A
  F_B,          ///< Out is B
  F_FALSE       ///< Out is 0
} fuzzy_op;

typedef uint8_t (*fuzzy) (int8_t x,  int8_t p1, int8_t p2, int8_t p3);

/// Fuzzy logic function 
typedef struct 
{
  fuzzy     func;     ///< fuzzification function pointer
  uint8_t   xn;       ///< input parameter number
  int8_t    a;        ///< first function parameter
  int8_t    b;        ///< second function parameter
  int8_t    c;        ///< third function parameter
  uint8_t   y;        ///< fuzzification output value 
  void      *next;    ///< next fuzzy function pointer
} fuzzy_funct;  

#define MAKE_FFUNC(name, ffunc, x, a, b, c, next) \
  extern fuzzy_funct next;   \
	fuzzy_funct name = {ffunc, x, a, b, c, 0, &next}

/// Fuzzy rule control structure
typedef struct 
{
  uint8_t       *a;       ///< operand a pointer
  fuzzy_op      op;       ///< logic operator between a and b
  uint8_t       *b;       ///< operand b pointer
  bool          fin;      ///< flag final complex logic function
  uint8_t       y;        ///< fuzzy function result value
  int8_t        out;      ///< output fuzzy value
  void          *next;    ///< next rule pointer
} fuzzy_rules; 

/// Fuzzy parameters control structure
typedef struct 
{
  int8_t        *in_array;    ///< pointer input values array
  fuzzy_rules   *start_rule;  ///< pointer to the first fuzzy rule
  fuzzy_funct   *start_ffunc; ///< pointer to the first fuzzy function
} fuzzy_param;
   
#define MAKE_RULE(name, a, op, b, fin, out, next) \
  extern fuzzy_rules next;   \
	fuzzy_rules name = {&((a).y), op, &((b).y), fin, 0, out, &next}


/// Prototypes fuzzyfication input functions parameter x
uint8_t cube      (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< Gauss cubic approximation, p1=M, p2=D_0.5 +-Delta 0.5 (n = 1 / D_0.5^3)
uint8_t triangle  (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< symmetric triangle function p1=center, p1-p2=min, p1+p2=max _/\_
uint8_t a_triangle (int8_t x, int8_t p1, int8_t p2, int8_t p3); ///< asymmetric triangle function p1=center, p1-p2=min, p1+p3=max _/\_
uint8_t square    (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< symmetric square function p1=center, p1-p2=min, p1+p2=max _|~|_
uint8_t trapecia  (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< symmetric trapecial function p1=center, p1+p2=top, p1+p3=bottom _/~\_
uint8_t low       (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< asymmetric low function p1=min p2=max ~\_
uint8_t high      (int8_t x, int8_t p1, int8_t p2, int8_t p3);  ///< asymmetric high function p1=min p2=max _/~

int8_t  in0_scaling (int16_t in1);  ///< user scaling input 1 value to the limits +-127
int8_t  in1_scaling (int16_t in2);  ///< user scaling input 2 value to the limits +-127
int16_t out_scaling (int8_t out);   ///< user scaling output value +-127 to the control value

int8_t process_fuzzy_logic (fuzzy_param *fuzzy);


inline static uint8_t   lim_u8  (int16_t x);
inline static int8_t    lim_s8  (int16_t x);

/*************************************************************************
 * \brief Limitation fuzzy result value by 0..1 e.g. 0..255
 *        for fuzzyfication functions
 * \param x         input
 * \return uint8_t  limitation result
*************************************************************************/
inline static uint8_t lim_u8 (int16_t x)
{
  if (x < 0)
  {
    x = 0;
  }
  else if (x > 255)
  {
    x = 255;
  }
  return (uint8_t)x;
}

/*************************************************************************
 * \brief Limitation in/out value by +-1 e.g. +-127
 *        for Fuzzyfication and Defuzzification function
 * \param x         input
 * \return int8_t   limitation output
*************************************************************************/
inline static int8_t lim_s8 (int16_t x)
{
  if (x < -127) // so
  {
    x = -127;
  }
  else if (x > 127)
  {
    x = 127;
  }
  return (int8_t)x;
}



#endif  // _FUZZY_LOGIC_H_
