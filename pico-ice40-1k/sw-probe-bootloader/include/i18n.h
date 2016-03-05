/**
 * eCow-logic - Bootloader
 *
 * Copyright (c) 2016 Saint-Genest Gwenael <gwen@agilack.fr>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation. (See COPYING.GPL for details.)
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef I18N_H
#define I18N_H

#define LANG_EN 1
#define LANG_FR 2

#ifndef LANG
#define LANG LANG_EN
#endif

#if LANG == LANG_EN

#define I18N_NO_FW        "Bad firmware"
#define I18N_LDR_NET      "Init network..."
#define I18N_LDR_DHCP     "Network(DHCP)   "
#define I18N_LDR_MODE_H   "Mode HTTP"
#define I18N_LDR_EFLASH   "Erase flash"
#define I18N_LDR_TFTP_END "TFTP: end."
#define I18N_LDR_TFTP_TO  "TFTP: timeout!"
#define I18N_LDR_TFTP_E1  "TFTP: error 1   "

#elif LANG == LANG_FR

#define I18N_NO_FW        "Erreur firmware"
#define I18N_LDR_NET      "Init reseau ..."
#define I18N_LDR_DHCP     "Reseau (DHCP)   "
#define I18N_LDR_MODE_H   "Mode HTTP"
#define I18N_LDR_EFLASH   "Efface flash"
#define I18N_LDR_TFTP_END "TFTP: complet."
#define I18N_LDR_TFTP_TO  "TFTP: timeout!"
#define I18N_LDR_TFTP_E1  "TFTP: erreur 1  "

#endif

#endif
