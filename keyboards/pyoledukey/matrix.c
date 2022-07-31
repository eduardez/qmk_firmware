/*
Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>
Copyright 2015 ZSA Technology Labs Inc (@zsa)
Copyright 2020 Christopher Courtney <drashna@live.com> (@drashna)
Copyright 2022 Eduardo Garcia <eduardez96@gmail.com> (@eduardez)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>

#include "wait.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "pyoledukey.h"
#include "i2c_master.h"
#include "quantum.h"



#include "debug.h"



/*
 * This constant define not debouncing time in msecs, assuming eager_pr.
 *
 * On Ergodox matrix scan rate is relatively low, because of slow I2C.
 * Now it's only 317 scans/second, or about 3.15 msec/scan.
 * According to Cherry specs, debouncing time is 5 msec.
 *
 * However, some switches seem to have higher debouncing requirements, or
 * something else might be wrong. (Also, the scan speed has improved since
 * that comment was written.)
 */

/* matrix state(1:on, 0:off) */

/* Column pin configuration
 *
 * PI PICO
 * Row: 0    1    2    3    4
 * pin: GP5  GP6  GP7  GP8  GP9
 *
 * pcf8575
 * col: 0    1    2    3    4    5    6    7    8    9    10   11
 * pin: P00  P01  P02  P03  P04  P05  P06  P07  P10  P11  P12  P13
 */

extern matrix_row_t raw_matrix[MATRIX_COLS];  // raw values


static void init_cols(void) {
    // init on pcf8575
    uint8_t buf[] = {0b00000000, 0b00000000};
    i2c_transmit(I2C_ADDR_WRITE, buf, sizeof(buf), PCF8575_I2C_TIMEOUT);
}

// Col Output PCF8575 16bit
static void select_col(uint8_t col) {
    // Col Output 'L'
    uint8_t colByte = ~(1 << col);
    if (col <= 7) {
        uint8_t buf[] = {colByte, 0b00000000};
        i2c_transmit(I2C_ADDR_WRITE, buf, sizeof(buf), PCF8575_I2C_TIMEOUT);
    } else {
        uint8_t buf[] = {0b00000000, colByte};
        i2c_transmit(I2C_ADDR_WRITE, buf, sizeof(buf), PCF8575_I2C_TIMEOUT);
    }
}


static uint8_t read_rows(void) {
  return (readPin(GP9) << 4)
         | (readPin(GP8) << 3)
        //  | (1 << 2)
         | (readPin(GP7) << 2)
         | (readPin(GP6) << 1)
         | (readPin(GP5) );
}

// Reads and stores a row, returning
// whether a change occurred.
static inline bool store_raw_matrix_row(uint8_t index) {
    uint8_t temp = read_rows();
    if (raw_matrix[index] != temp) {
        raw_matrix[index] = temp;
        return true;
    }
    return false;
}


bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    debug_enable = true;

    // debug("Current matrix"); debug_hex(current_matrix); debug(" ");



    init_cols();
    bool changed = false;
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        debug("\nColumna "); debug_hex(i);
        select_col(i);
        matrix_io_delay();
        changed |= store_raw_matrix_row(i);
    }

    return changed;






//     // Scan Keyboard Matrix
//     ROW_TYPE  tmp_rows[MATRIX_ROWS];
//     ROW_TYPE* pt = tmp_rows;
//     for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
//         // MSB = col 7, LSB = col 0
//         select_col(col);
//         matrix_io_delay();
//         *pt++ = read_rows();
//     }
//     col_init();

// #ifdef MAXIMUM_ACCEPT_HIT_KEY
//     // Check Maximum Accept Hit Key count
//     uint8_t hit_key_count = 0;
//     for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
//         ROW_TYPE rows = tmp_rows[col];

//         // Hit None Key is Continue
//         if (rows == 0) continue;

//         // Counting Hit Key
//         hit_key_count += matrix_bitpop(rows);

//         // Not Accept over Maximum Accept Hit Key
//         if (hit_key_count > MAXIMUM_ACCEPT_HIT_KEY) return false;
//     }
// #endif

//     // Hunt Keyboard Matrix Ghost for Anti Ghosting by FREE WING
//     // for (uint8_t col = 0; col < MATRIX_COLS - 1; ++col) {
//     //     ROW_TYPE rows = tmp_rows[col];

//     //     // Hit None Key is Pass without Complaint
//     //     if (rows == 0) continue;

//     //     // Only Hit One Key
//     //     bool hit_one_key = (matrix_bitpop(rows) == 1);

//     //     // Check Ghost
//     //     for (uint8_t col2 = MATRIX_COLS - 1; col2 > col; --col2) {
//     //         // Not detecting bits is Safe
//     //         if (!(rows & tmp_rows[col2])) continue;

//     //         // When Only Hit One Key, Not detecting other bit is Safe
//     //         if (hit_one_key && !(rows ^ tmp_rows[col2])) continue;

//     //         // Catch Ghost !!
//     //         // We are Keyboard Matrix Ghost Busters !!
//     //         return false;
//     //     }
//     // }

//     // Check Keyboard Matrix has Changed
//     bool          matrix_has_changed = false;
//     matrix_row_t* p                  = current_matrix;
//     for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
//         // Convert Rows data to Column data Matrix
//         ROW_TYPE     and_row  = (ROW_SHIFTER << row);
//         matrix_row_t now_rows = 0;
//         int8_t       col      = MATRIX_COLS;
//         while (--col >= 0) {
//             now_rows <<= 1;
//             now_rows |= ((tmp_rows[col] & and_row) ? 1 : 0);
//         }

//         // Has Changed
//         if (*p != now_rows) {
//             *p                 = now_rows;
//             matrix_has_changed = true;
//         }
//         ++p;
//     }

//     return matrix_has_changed;
}
