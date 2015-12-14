/**
 * eCow-logic - Bootloader
 *
 * This file is based on miim.h from wiznet ioLibrary
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __MIIM_H__ 
#define __MIIM_H__
#include "types.h"

#define PHY_ADDR_KSZ8091 0x00
#define PHY_ADDR PHY_ADDR_KSZ8091

#define SVAL 0x2            /*right shift val = 2  */
#define PHYREG_CONTROL  0x0 /* Control Register */
#define PHYREG_STATUS   0x1 /* Status Register  */
#define PHYREG_ID1      0x2 /* PHY Identifier 1 */
#define PHYREG_ID2      0x3 /* PHY Identifier 2 */

#define CNTL_DUPLEX     (0x01ul<< 7)
#define CNTL_AUTONEGO   (0x01ul<<11)
#define CNTL_SPEED      (0x01ul<<12)
typedef enum
{
    HalfDuplex10 = 0x0000,
    FullDuplex10 = CNTL_DUPLEX,
    AUTONEGO = CNTL_AUTONEGO,
    HalfDuplex100 = CNTL_SPEED,
    FullDuplex100 = (CNTL_SPEED|CNTL_DUPLEX)
}SetLink_Type;

#define IS_SETLINK_TYPE(Mode) (((Mode)==HalfDuplex10) || ((Mode)==FullDuplex10) || ((Mode)==AUTONEGO) || \
                               ((Mode)==HalfDuplex100) || ((Mode)==FullDuplex100))
u32  detect(int n);
void mdio_init (void);
void mdio_write(u32 PhyRegAddr, u32 val);
u32  mdio_read (u32 PhyRegAddr, u32 PhyAddr);
u32  link(void);
void set_link(SetLink_Type mode);

#endif
