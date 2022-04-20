/*******************************************************************************
* \file     fuzzy_logic.c
* \author   Ilya Petrukhin (ilya.petrukhin@gmail.com)
* \brief    This file provides code for fuzzy logic functions
* \version  2.0
* \date     2022-04-14
*******************************************************************************/
#include  <math.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <stdbool.h>
#include  <string.h>
#include  "fuzzy_logic.h"

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) > (b) ? (a) : (b))


/*******************************************************************************
* кубическая аппроксимация гаусса, p1=M, p2=D_0.5
* \brief  Cubic approximation of Gauss function
* \param[in]  x   input value
* \param[in]  p1  Median
* \param[in]  p2  +-Delta 0.5 (n = 1 / D_0.5^3)
* \return         output value y = 1 / (1 + abs (n * (x - p1)^3)) 0-255
*******************************************************************************/
uint8_t cube (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int32_t d32;
  int32_t dx32;
  int32_t temp32;
  int16_t ret;
  
  (void)p3; 
  // y = D_0.5^3 / (D_0.5^3 + abs(x - p1)^3)
  if (p2 <0)
  {
    p2 = -p2;
  }
  d32 = p2;           // D_0.5
  d32 = (d32 * p2);   // D_0.5^2 * 127
  d32 = (d32 * p2);   // D_0.5^3 * 127^2

  if (x < p1)                         // abs(x-m)
  {
    dx32 = p1 - x;
  }
  else
  {
    dx32 = x - p1;                   
  }
  temp32 = (dx32 * dx32);     // (x-m)^2 * 127
  temp32 = (temp32 * dx32);   // (x-m)^3 * 127^2

  temp32 = temp32 + d32;      // d^3 + (x-m)^3

  if (temp32 != 0)
  {
    ret = d32 * 255 / temp32;  // d^3 / (d^3 + (x-m)^3)
  }
  else
  {
    ret = 255;
  }

  return lim_u8 (ret);
}


/*******************************************************************************
* симметричная треугольная функция p1=center, p1+p2=max, p1-p2=min _/\_
* \brief  Symmetric triangle function _/\_
* \param[in]  x   input value
* \param[in]  p1  Center
*                 p1 + p2 = max
*                 p1 - p2 = min
* \param[in]  p2  Delta 
* \return         output value
*******************************************************************************/
uint8_t triangle (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int16_t ret;
  int8_t min = p1 - p2;
  int8_t max = p1 + p2;
  
  (void)p3; 
  /// protection p2
  if (p2 == 0)
  {
    return 0;
  } 

  if ((x < min) || (x > max))
  {
    ret = 0;
  }
  else if (x < p1)
  {
    ret = ((int16_t)(x - min) * 255) / p2;
  }
  else
  {
    ret = ((int16_t)(max - x) * 255) / p2;
  }
  
  return lim_u8 (ret);
}

/*******************************************************************************
* Несимметричная треугольная функция p1=center, p1+p2=max, p1-p2=min _/\_
* \brief  Asymmetric triangle function _/\_
* \param[in]  x   input value
* \param[in]  p1  Center
*                 p1 + p3 = max
*                 p1 - p2 = min
* \param[in]  p2  Delta min
* \param[in]  p3  Delta max
* \return         output value
*******************************************************************************/
uint8_t a_triangle (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int16_t ret;
  int8_t min = p1 - p2;
  int8_t max = p1 + p3;

/// protection p2, p3
  if ((p2 == 0) || (p3 == 0))  
  {
    return 0;
  } 

  if ((x < min) || (x > max))
  {
    ret = 0;
  }
  else if (x < p1)
  {
    ret = ((int16_t)(x - min) * 255) / p2;
  }
  else
  {
    ret = ((int16_t)(max - x) * 255) / p3;
  }
  
  return lim_u8 (ret);
}

/*******************************************************************************
* симметричная прямоугольная функция p1=center, p1+p2=max, p1-p2=min _|~|_
* \brief  Symmetric square function _|~|_
* \param[in]  x   input value
* \param[in]  p1  Center
*                 p1 + p2 = max
*                 p1 - p2 = min
* \param[in]  p2  Delta 
* \return         output value
*******************************************************************************/
uint8_t square (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  uint8_t ret = 0;

  (void)p3;  

  if ((x >= (p1 - p2)) && (x <= (p1 + p2)))
  {
    ret = 255;
  }
  return ret;
}

/*******************************************************************************
* симметричная трапециональная функция p1=center, p1+p2=max, p1-p2=min _/~\_
* основание p1+p3=max, p1-p3=min
* \brief  Symmetric trapecial function _/~\_
* \param[in]  x   input value
* \param[in]  p1  Center
* \param[in]  p2  p1 + p2 = top max, p1 - p2 = top min
* \param[in]  p3  p1 + p3 = base max, p1 - p3 = base min 
* \return         output value
*******************************************************************************/
uint8_t trapecia (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int16_t ret;
  int8_t min = p1 - p2;
  int8_t max = p1 + p2;
  int8_t min2 = p1 - p3;
  int8_t max2 = p1 + p3;
  
  if (x < min2 || x > max2)
  {
    ret = 0;
  }
  else if (x >= min && x <= max)
  {
    ret = 255;
  }
  else if (x < min)
  {
    if (min > min2)
    {
      ret = ((int16_t)(x - min2) * 255) / (min - min2);
    }
    else
    {
      ret = 255;
    }
  }
  else 
  {
    if (max2 > max)
    {
      ret = ((int16_t)(max2 - x) * 255) / (max2 - max);
    }
    else
    {
      ret = 255;
    }
  }
  
return lim_u8 (ret);
}

/*******************************************************************************
* несимметричная функция минимума p1=min p2=max ~\_
* \brief  Asymmetric minimum function ~\_
* \param[in]  x   input value
* \param[in]  p1  min
* \param[in]  p2  max 
* \return         output value
*******************************************************************************/
uint8_t low (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int16_t ret;
  int8_t temp;
  
  (void)p3;

  if (p1 > p2)        // swap p1, p2
  {
    temp = p1;
    p1 = p2;
    p2 = temp;
  }
  if (x < p1)
  {
    ret = 255;
  }
  else if (x > p2)
  {
    ret = 0;
  }
  else
  {
    if (p2 > p1)
    {
      ret = ((int16_t)(p2 - x) * 255) / (p2 - p1);
    }
    else
    {
      ret = 255;
    }
  }

return lim_u8 (ret);
}


/*******************************************************************************
* несимметричная функция максимума p1=min p2=max _/~
* \brief  Asymmetric maximum function _/~
* \param[in]  x   input value
* \param[in]  p1  min
* \param[in]  p2  max 
* \return         output value
*******************************************************************************/
uint8_t high (int8_t x, int8_t p1, int8_t p2, int8_t p3)
{
  int16_t ret;
  int8_t temp;

  (void)p3;  
  if (p1 > p2)  // swap p1, p2
  {
    temp = p1;
    p1 = p2;
    p2 = temp;
  }
  if (x < p1)
  {
    ret = 0;
  }
  else if (x > p2)
  {
    ret = 255;
  }
  else
  {
    if (p2 > p1)
    {
      ret = ((int16_t)(x - p1) * 255) / (p2 - p1);
    }
    else
    {
      ret = 255;
    }
  }
  
return lim_u8 (ret);
}




/*******************************************************************************
* Реализация нечеткого регулятора согласно правил fuzzy_param *fuzzy
* \brief Fuzzy logic controller by rules fuzzy_param *fuzzy
* \param[in] fuzzy->in_array[0] - error value (текущая ошибка)
* \param[in] fuzzy->in_array[1] - error diff (скорость изменения ошибки)
* \param[in] fuzzy->in_array[2] - error diff diff (option) 
*                                 (ускорение изменения ошибки (опция))
* \return Output control value 
*******************************************************************************/
int8_t process_fuzzy_logic (fuzzy_param *fuzzy)
{
  fuzzy_funct *f, *f1;
  fuzzy_rules *r, *r1;
  int16_t summ_alpha_c = 0;
  int16_t summ_alpha = 0;
  int16_t alpha, a, b, ret;
  int8_t *in_array;
  bool start;
  
  /// получить результаты функций фуззификации
  f1 = f = fuzzy->start_ffunc;
  in_array = fuzzy->in_array;
  start = true;
  while ((f != f1) || start)
  {
    if (f->func)
    {
      f->y = f->func (in_array[f->xn], f->a, f->b, f->c);
    }
    start = false;
    f = f->next;
  }
  
  /// цикл по правилам нечёткой логики
  r1 = r = fuzzy->start_rule;
  start = true;
  while ((r != r1) || start)
  {
    a = *(r->a);
    b = *(r->b);
    /// применяем логические операторы
    switch (r->op)
    {
    case F_AND:
      alpha = MIN (a, b);
      break;
      
    case F_OR:
      alpha = MAX (a, b);
      break;
      
    case F_NOT:
      alpha = 255 - a;
      break;
      
    case F_IMP:
      if (a == 0)
      {
        alpha = 255;
      }
      else
      {
        if (a)
        {
          alpha = (b * (int16_t)255) / a;
          alpha = lim_u8 (alpha);
        }
        else
        {
          alpha = 255;
        }
      }
      break;
      
    case F_A:
      alpha = a;
      break;
      
    case F_B:
      alpha = b;
      break;
      
    case F_FALSE:
    default:
      alpha = 0;
      break;
    }
    r->y = alpha;

    if (r->fin)  // если это конечное выражение
    {
      /// числитель и знаменатель для дискретного варианта 
      /// центроидного метода приведения к четкости
      summ_alpha_c += (alpha * (int16_t)r->out); // / 255;
      summ_alpha += alpha;
    }
    /// вычисляем следующее правило
    r = r->next;
    start = false;
  }

  /// вычисляем воздействие на объект управления
  if (summ_alpha == 0)
  {
    ret = 0;
  }
  else
  {
    ret = summ_alpha_c / summ_alpha;
  }
  return lim_s8 (ret);
}


