//! @file types.h
//! @brief Aliases de tipos y tamaños portables para toda la librería.
//! @details Define los identificadores de tipo cómodos (`TI`, `TD`, `TS`,
//!          `Ref`, `XT`) y los tipos empleados por el B-Tree
//!          (`obj_id_t`, `tree_order_t`, `tree_height_t`, `btree_key_t`,
//!          `btree_pos_t`, `use_count_t`, `bree_size_t`).
//! @author Equipo MCC

#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
using namespace std;

/* ----- Aliases genéricos ----------------------------------------------- */
//! @typedef TI
//! @brief Alias para `int` (tipo entero principal usado en ejemplos y demos).
//! @typedef TD
//! @brief Alias para `double` (tipo numérico de coma flotante).
//! @typedef TS
//! @brief Alias para `string` (tipo cadena usado en ejemplos de Vector).
//! @typedef Ref
//! @brief Identificador asociado a un dato (`long`). Sirve como "puntero"
//!         ligero: cada nodo guarda un `Ref` además del valor.
//! @typedef XT
//! @brief Entero portable de 32 bits en Windows y 64 bits en Linux, pensado
//!        para tamaños dependientes del sistema operativo.

using TI = int;
using TD = double;
using TS = string;

#if defined(_WIN32) || defined(_WIN64)
    using XT = int;
#else
    using XT = long;
#endif

using Ref = long;

/* ----- Tipos específicos del B-Tree ------------------------------------ */
//! @typedef obj_id_t
//! @brief Identificador del objeto asociado a una clave del B-Tree (`long`).
//! @typedef use_count_t
//! @brief Contador de uso de un nodo (`long`). Se incrementa en cada `Search`.
//! @typedef tree_order_t
//! @brief Orden del B-Tree (`int`). Número máximo de hijos por nodo.
//! @typedef tree_height_t
//! @brief Altura del B-Tree (`int`). Nivel de profundidad para impresión.
//! @typedef btree_key_t
//! @brief Tipo de clave por defecto del B-Tree (`int`).
//! @typedef btree_pos_t
//! @brief Posición/índice dentro de un nodo del B-Tree (`int`).
//! @typedef bree_size_t
//! @brief Tamaño auxiliar usado en operaciones de desplazamiento del B-Tree.

using obj_id_t      =   long;
using use_count_t   =   long;
using tree_order_t  =   int;
using tree_height_t =   int;
using btree_key_t   =   int;
using btree_pos_t   =   int;
using bree_size_t   =   int;


#endif // __TYPES_H__
