/*
*********************************************************************************************************
*                                               uC/CRC
*           ERROR DETECTING CODE (EDC) & ERROR CORRECTING CODE (ECC) CALCULATION UTILITIES
*
*                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                              CYCLIC REDUNDANCY CHECK (CRC) CALCULATION
*
* Filename : edc_crc.h
* Version  : V1.10.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef   EDC_CRC_PRESENT
#define   EDC_CRC_PRESENT


/*
*********************************************************************************************************
*                                           CRC VERSION NUMBER
*
* Note(s) : (1) (a) The CRC software version is denoted as follows :
*
*                       Vx.yy.zz
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes     major software version revision number
*                                   yy              denotes     minor software version revision number
*                                   zz              denotes sub-minor software version revision number
*
*               (b) The software version label #define is formatted as follows :
*
*                       ver = x.yyzz * 100 * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yyzz          denotes software version number, where the unscaled integer
*                                                       portion denotes the major version number & the unscaled
*                                                       fractional portion denotes the (concatenated) minor
*                                                       version numbers

*********************************************************************************************************
*/

#define  CRC_VERSION               11000u                         /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_def.h>

#include  <crc_cfg.h>


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef    EDC_CRC_MODULE
#define   EDC_CRC_EXT
#else
#define   EDC_CRC_EXT  extern
#endif


/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/

#ifndef  ECC_CRC_CFG_ARG_CHK_EXT_EN
#define  ECC_CRC_CFG_ARG_CHK_EXT_EN             DEF_ENABLED
#endif

#ifndef  EDC_CRC_CFG_OPTIMIZE_ASM_EN
#define  EDC_CRC_CFG_OPTIMIZE_ASM_EN            DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_1021_EN
#define  EDC_CRC_CFG_CRC16_1021_EN              DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_1021_REF_EN
#define  EDC_CRC_CFG_CRC16_1021_REF_EN          DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_8005_EN
#define  EDC_CRC_CFG_CRC16_8005_EN              DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_8005_REF_EN
#define  EDC_CRC_CFG_CRC16_8005_REF_EN          DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_8048_EN
#define  EDC_CRC_CFG_CRC16_8048_EN              DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC16_8048_REF_EN
#define  EDC_CRC_CFG_CRC16_8048_REF_EN          DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC32_EN
#define  EDC_CRC_CFG_CRC32_EN                   DEF_DISABLED
#endif

#ifndef  EDC_CRC_CFG_CRC32_REF_EN
#define  EDC_CRC_CFG_CRC32_REF_EN               DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            CRC ERROR CODES
*********************************************************************************************************
*/

typedef enum edc_err {
    EDC_CRC_ERR_NONE                             =      0u,         /* No error.                                            */
    EDC_CRC_ERR_NULL_PTR                         =      1u,         /* Null ptr argument.                                   */
    EDC_CRC_ERR_INVALID_MODEL                    =      2u,         /* Invalid CRC calc model.                              */
} EDC_ERR;


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  struct  crc_model_16 {
           CPU_INT16U    Poly;
           CPU_INT16U    InitVal;
           CPU_BOOLEAN   Reflect;
           CPU_INT16U    XorOut;
    const  CPU_INT16U   *TblPtr;
} CRC_MODEL_16;

typedef  struct  crc_model_32 {
           CPU_INT32U    Poly;
           CPU_INT32U    InitVal;
           CPU_BOOLEAN   Reflect;
           CPU_INT32U    XorOut;
    const  CPU_INT32U   *TblPtr;
} CRC_MODEL_32;

typedef  struct  crc_calc_16 {
    CRC_MODEL_16  Model;
    CPU_INT16U    CRC_Curr;
} CRC_CALC_16;

typedef  struct  crc_calc_32 {
    CRC_MODEL_32  Model;
    CPU_INT32U    CRC_Curr;
} CRC_CALC_32;


/*
*********************************************************************************************************
*                                              CONSTANTS
*
* Note(s) : (1) Constant tables for several common CRCs are supplied.  Unfortunately, there often exists
*               no definitive specification of these CRCs or such specification is difficult to obtain.
*               Consequently, the most convenient method for determining the necessary CRC is accomplished
*               by comparing CRCs from real tests with CRCs from example tests.  CRC outputs for the input
*               ASCII string "123456789" are given in this table :
*
*
*                       ------------------------------------------------------------------
*                       |    POLY    |  REFLECT?  |  INIT VAL  | COMP. OUT? |     CRC    |
*                       -------------+------------+------------+------------+-------------
*                       |     0x1021 |     NO     |     0x0000 |     NO     |     0x31C3 |
*                       |     0x1021 |     NO     |     0x0000 |     YES    |     0xCE3C |
*                       |     0x1021 |     NO     |     0x1D0F |     NO     |     0xE5CC |
*                       |     0x1021 |     NO     |     0xFFFF |     NO     |     0x29B1 |
*                       |     0x1021 |     NO     |     0xFFFF |     YES    |     0xD64E |
*                       -------------+------------+------------+------------+-------------
*                       |     0x1021 |     YES    |     0x0000 |     NO     |     0x2189 |
*                       |     0x1021 |     YES    |     0x0000 |     YES    |     0xDE76 |
*                       |     0x1021 |     YES    |     0xFFFF |     NO     |     0x6F91 |
*                       |     0x1021 |     YES    |     0xFFFF |     YES    |     0x906E |
*                       -------------+------------+------------+------------+-------------
*                       |     0x8005 |     NO     |     0x0000 |     NO     |     0xFEE8 |
*                       |     0x8005 |     NO     |     0x0000 |     YES    |     0x0117 |
*                       |     0x8005 |     NO     |     0xFFFF |     NO     |     0xAEE7 |
*                       |     0x8005 |     NO     |     0xFFFF |     YES    |     0x5118 |
*                       -------------+------------+------------+------------+-------------
*                       |     0x8005 |     YES    |     0x0000 |     NO     |     0xBB3D |
*                       |     0x8005 |     YES    |     0x0000 |     YES    |     0x44C2 |
*                       |     0x8005 |     YES    |     0xFFFF |     NO     |     0x4B37 |
*                       |     0x8005 |     YES    |     0xFFFF |     YES    |     0xB4C8 |
*                       -------------+------------+------------+------------+-------------
*                       |     0x8048 |     NO     |     0x0000 |     NO     |     0x80A0 |
*                       |     0x8048 |     NO     |     0x0000 |     YES    |     0x7F5F |
*                       |     0x8048 |     NO     |     0xFFFF |     NO     |     0xE8E0 |
*                       |     0x8048 |     NO     |     0xFFFF |     YES    |     0x171F |
*                       -------------+------------+------------+------------+-------------
*                       |     0x8048 |     YES    |     0x0000 |     NO     |     0x1506 |
*                       |     0x8048 |     YES    |     0x0000 |     YES    |     0xEAF9 |
*                       |     0x8048 |     YES    |     0xFFFF |     NO     |     0x1710 |
*                       |     0x8048 |     YES    |     0xFFFF |     YES    |     0xE8EF |
*                       -------------+------------+------------+------------+-------------
*                       | 0x04C11DB7 |     NO     | 0x00000000 |     NO     | 0x89A1897F |
*                       | 0x04C11DB7 |     NO     | 0x00000000 |     YES    | 0x765E7680 |
*                       | 0x04C11DB7 |     NO     | 0xFFFFFFFF |     NO     | 0x0376E6E7 |
*                       | 0x04C11DB7 |     NO     | 0xFFFFFFFF |     YES    | 0xFC891918 |
*                       -------------+------------+------------+------------+-------------
*                       | 0x04C11DB7 |     YES    | 0x00000000 |     NO     | 0x2DFD2D88 |
*                       | 0x04C11DB7 |     YES    | 0x00000000 |     YES    | 0xD202D277 |
*                       | 0x04C11DB7 |     YES    | 0xFFFFFFFF |     NO     | 0x340BC6D9 |
*                       | 0x04C11DB7 |     YES    | 0xFFFFFFFF |     YES    | 0xCBF43926 |
*                       -------------+------------+------------+------------+-------------
*
*               (a) The column 'COMP. OUT?' (short for 'COMPLEMENT OUTPUT?') indicates whether the output
*                   XOR value would be either 0xFFFF (for a CRC16) or 0xFFFFFFFF (for a CRC32).
*
*                   (1) If YES, this means that the CRC model struct ('CRC_MODEL_16' or 'CRC_MODEL_32')
*                       should have a 'XorOut' member equal to 0xFFFF or 0xFFFFFFFF, respectively.
*
*                   (2) If NO,  this means that the CRC model struct ('CRC_MODEL_16' or 'CRC_MODEL_32')
*                       should have a 'XorOut' member equal to 0x0000 or 0x00000000, respectively.
*
*               (b) The column 'REFLECT?' indicates that BOTH the input data & final output value
*                   would be reflected.
*
*                   (1) If YES, this means that the CRC model struct ('CRC_MODEL_16' or 'CRC_MODEL_32')
*                       should have a 'Reflect' member equal to DEF_YES.
*
*                   (2) If NO,  this means that the CRC model struct ('CRC_MODEL_16' or 'CRC_MODEL_32')
*                       should have a 'Reflect' member equal to DEF_NO.
*
*           (2) The constant tables depend ONLY on the polynomial & whether the data is reflected.
*               The additional model parameters, such as the initial value & the XOR for the output,
*               do not matter.  Consequently, the provided tables may be used with user-defined models
*               that differ only in initial value &/or XOR for the output with the models provided
*               here.
*********************************************************************************************************
*/

#if (EDC_CRC_CFG_CRC16_1021_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_1021[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_1021;
#endif

#if (EDC_CRC_CFG_CRC16_1021_REF_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_1021_ref[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_1021_ref;
#endif

#if (EDC_CRC_CFG_CRC16_8005_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_8005[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_8005;
#endif

#if (EDC_CRC_CFG_CRC16_8005_REF_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_8005_ref[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_8005_ref;
#endif

#if (EDC_CRC_CFG_CRC16_8048_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_8048[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_8048;
#endif

#if (EDC_CRC_CFG_CRC16_8048_REF_EN == DEF_ENABLED)
extern  const  CPU_INT16U    CRC_TblCRC16_8048_ref[256];
extern  const  CRC_MODEL_16  CRC_ModelCRC16_8048_ref;
#endif

#if (EDC_CRC_CFG_CRC32_EN == DEF_ENABLED)
extern  const  CPU_INT32U    CRC_TblCRC32[256];
extern  const  CRC_MODEL_32  CRC_ModelCRC32;
#endif

#if (EDC_CRC_CFG_CRC32_REF_EN == DEF_ENABLED)
extern  const  CPU_INT32U    CRC_TblCRC32_ref[256];
extern  const  CRC_MODEL_32  CRC_ModelCRC32_ref;
#endif


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                /* ------------- 16-BIT CRC ON DATA STREAM ------------ */
void        CRC_Open_16Bit             (CRC_MODEL_16       *p_model,
                                        CRC_CALC_16        *p_calc,
                                        EDC_ERR            *p_err);

void        CRC_WrBlock_16Bit          (CRC_CALC_16        *p_calc,
                                        void               *p_data,
                                        CPU_SIZE_T          size);

void        CRC_WrOctet_16Bit          (CRC_CALC_16        *p_calc,
                                        CPU_INT08U          octet);

CPU_INT16U  CRC_Close_16Bit            (CRC_CALC_16        *p_calc);

                                                                /* ------------- 32-BIT CRC ON DATA STREAM ------------ */
void        CRC_Open_32Bit             (CRC_MODEL_32       *p_model,
                                        CRC_CALC_32        *p_calc,
                                        EDC_ERR            *p_err);

void        CRC_WrBlock_32Bit          (CRC_CALC_32        *p_calc,
                                        void               *p_data,
                                        CPU_SIZE_T          size);

void        CRC_WrOctet_32Bit          (CRC_CALC_32        *p_calc,
                                        CPU_INT08U          octet);

CPU_INT32U  CRC_Close_32Bit            (CRC_CALC_32        *p_calc);

                                                                /* --------------------- Reflection ------------------- */
CPU_INT08U  CRC_Reflect_08Bit          (CPU_INT08U          datum);

CPU_INT16U  CRC_Reflect_16Bit          (CPU_INT16U          datum);

CPU_INT32U  CRC_Reflect_32Bit          (CPU_INT32U          datum);


                                                                /* -------------- CALCULATION ON DATA SET ------------- */
CPU_INT16U  CRC_ChkSumCalc_16Bit       (CRC_MODEL_16       *p_model,
                                        void               *p_data,
                                        CPU_SIZE_T          size,
                                        EDC_ERR            *p_err);

CPU_INT32U  CRC_ChkSumCalc_32Bit       (CRC_MODEL_32       *p_model,
                                        void               *p_data,
                                        CPU_SIZE_T          size,
                                        EDC_ERR            *p_err);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                       defined in edc_crc_a.asm
*********************************************************************************************************
*/

#if (EDC_CRC_CFG_OPTIMIZE_ASM_EN == DEF_ENABLED)
CPU_INT16U  CRC_ChkSumCalcTbl_16Bit    (CPU_INT16U          init_val,
                                        const  CPU_INT16U  *p_tbl,
                                        CPU_INT08U         *p_data,
                                        CPU_SIZE_T          size);

CPU_INT16U  CRC_ChkSumCalcTbl_16Bit_ref(CPU_INT16U          init_val,
                                        const  CPU_INT16U  *p_tbl,
                                        CPU_INT08U         *p_data,
                                        CPU_SIZE_T          size);

CPU_INT32U  CRC_ChkSumCalcTbl_32Bit    (CPU_INT32U          init_val,
                                        const  CPU_INT32U  *p_tbl,
                                        CPU_INT08U          *p_data,
                                        CPU_SIZE_T          size);

CPU_INT32U  CRC_ChkSumCalcTbl_32Bit_ref(CPU_INT32U          init_val,
                                        const  CPU_INT32U  *p_tbl,
                                        CPU_INT08U         *p_data,
                                        CPU_SIZE_T          size);
#endif


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  ECC_CRC_CFG_ARG_CHK_EXT_EN
#error  "ECC_CRC_CFG_ARG_CHK_EXT_EN             not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "

#elif  ((ECC_CRC_CFG_ARG_CHK_EXT_EN != DEF_DISABLED) && \
        (ECC_CRC_CFG_ARG_CHK_EXT_EN != DEF_ENABLED ))
#error  "ECC_CRC_CFG_ARG_CHK_EXT_EN       illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_OPTIMIZE_ASM_EN
#error  "EDC_CRC_CFG_OPTIMIZE_ASM_EN            not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_OPTIMIZE_ASM_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_OPTIMIZE_ASM_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_OPTIMIZE_ASM_EN      illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC16_1021_EN
#error  "EDC_CRC_CFG_CRC16_1021_EN              not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_1021_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_1021_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_1021_EN        illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC16_1021_REF_EN
#error  "EDC_CRC_CFG_CRC16_1021_REF_EN          not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_1021_REF_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_1021_REF_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_1021_REF_EN    illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif


#ifndef  EDC_CRC_CFG_CRC16_8005_EN
#error  "EDC_CRC_CFG_CRC16_8005_EN              not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_8005_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_8005_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_8005_EN        illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC16_8005_REF_EN
#error  "EDC_CRC_CFG_CRC16_8005_REF_EN          not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_8005_REF_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_8005_REF_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_8005_REF_EN    illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC16_8048_EN
#error  "EDC_CRC_CFG_CRC16_8048_EN              not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_8048_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_8048_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_8048_EN        illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC16_8048_REF_EN
#error  "EDC_CRC_CFG_CRC16_8048_REF_EN          not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC16_8048_REF_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC16_8048_REF_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC16_8048_REF_EN    illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif



#ifndef  EDC_CRC_CFG_CRC32_EN
#error  "EDC_CRC_CFG_CRC32_EN                   not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC32_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC32_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC32_EN             illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif


#ifndef  EDC_CRC_CFG_CRC32_REF_EN
#error  "EDC_CRC_CFG_CRC32_REF_EN               not #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#elif  ((EDC_CRC_CFG_CRC32_REF_EN != DEF_ENABLED ) && \
        (EDC_CRC_CFG_CRC32_REF_EN != DEF_DISABLED))
#error  "EDC_CRC_CFG_CRC32_REF_EN         illegally #define'd in 'app_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]            "
#error  "                                 [     || DEF_DISABLED]            "
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of CRC module include.                           */
