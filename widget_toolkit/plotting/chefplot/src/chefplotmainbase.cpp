/****************************************************************************
** Form implementation generated from reading ui file 'chefplotmainbase.ui'
**
** Created: Wed Jul 7 16:21:36 2004
**      by: The User Interface Compiler ($Id: chefplotmainbase.cpp,v 1.2 2004/07/16 21:46:14 michelot Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "chefplotmainbase.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "chefplotmainbase.ui.h"
static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x74, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0xd5, 0xc1, 0x09, 0xc0,
    0x20, 0x0c, 0x05, 0xd0, 0x6f, 0xe9, 0x36, 0x81, 0x2c, 0x10, 0xb2, 0xff,
    0xdd, 0x85, 0xd2, 0x53, 0x85, 0xb6, 0xa9, 0x91, 0x48, 0x0f, 0x05, 0x3f,
    0x08, 0x1a, 0xf0, 0x29, 0x12, 0x10, 0xf8, 0x28, 0xc5, 0xa9, 0xd9, 0xc4,
    0xde, 0x96, 0xcd, 0x2b, 0x9a, 0xd9, 0xeb, 0x00, 0x00, 0x66, 0x0e, 0x2f,
    0xe0, 0xc2, 0x51, 0x98, 0x39, 0xc4, 0xf7, 0x0c, 0x4c, 0x44, 0x6d, 0x5e,
    0x6b, 0x35, 0x38, 0xcf, 0x92, 0x82, 0x45, 0xe4, 0xb2, 0xf6, 0xf0, 0x14,
    0xac, 0xaa, 0x8f, 0xda, 0x1d, 0x4f, 0xc1, 0xa5, 0x74, 0x1b, 0x22, 0x07,
    0x9f, 0x9d, 0x11, 0x1d, 0x96, 0xea, 0x8a, 0x91, 0x2c, 0x78, 0xc1, 0x0b,
    0xee, 0x64, 0xe6, 0x07, 0x19, 0xf5, 0x7e, 0x92, 0x03, 0xad, 0x45, 0x2a,
    0x04, 0x03, 0x18, 0xf5, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image1_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x99, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x94, 0x41, 0x0e, 0x85,
    0x20, 0x0c, 0x44, 0x5f, 0x89, 0xc7, 0x36, 0x7f, 0x61, 0xbc, 0x77, 0x5d,
    0x28, 0x48, 0xa4, 0x28, 0x60, 0xff, 0xce, 0xd9, 0x54, 0x8b, 0xbe, 0x8e,
    0x13, 0x04, 0x3e, 0x1d, 0x92, 0x81, 0x77, 0xf4, 0x81, 0xa1, 0x23, 0xdc,
    0x2b, 0x34, 0xf6, 0xf4, 0x7a, 0x3d, 0xe2, 0xb8, 0x65, 0xa8, 0x84, 0x3f,
    0x40, 0x01, 0x98, 0x2a, 0x0b, 0x3d, 0x5f, 0x62, 0xc5, 0x83, 0x00, 0xaa,
    0x1a, 0xd7, 0x05, 0x50, 0x44, 0x9a, 0xb9, 0xd5, 0x07, 0xa7, 0x73, 0xa8,
    0xa4, 0xba, 0x4f, 0x92, 0xa2, 0xdf, 0x33, 0x3c, 0x64, 0xc6, 0x3b, 0xeb,
    0xbd, 0x82, 0xe5, 0xb8, 0xad, 0xde, 0xcb, 0xcc, 0x78, 0x20, 0xeb, 0x42,
    0x66, 0xc6, 0x39, 0x74, 0x5d, 0xfa, 0x80, 0xf3, 0x6f, 0xaf, 0x66, 0xc6,
    0x6f, 0xa1, 0x9c, 0x3f, 0x88, 0x2f, 0xb4, 0x70, 0xec, 0x05, 0xcd, 0xc0,
    0xbe, 0xd0, 0x78, 0x93, 0xf6, 0x8e, 0x17, 0x14, 0x92, 0x63, 0x5f, 0x68,
    0x6c, 0x3e, 0xef, 0xf6, 0xba, 0x3c, 0x8f, 0xdd, 0x36, 0x6d, 0xc4, 0xc0,
    0x45, 0x2c, 0xf1, 0x53, 0xa9, 0x59, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image2_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xa0, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xd5, 0x95, 0x4d, 0x0a, 0x80,
    0x20, 0x10, 0x85, 0x9f, 0xd1, 0x46, 0x68, 0xe1, 0x8d, 0xe6, 0x62, 0xd2,
    0x22, 0xbc, 0x98, 0x37, 0x6a, 0x21, 0xb4, 0xac, 0x45, 0x19, 0x92, 0xc6,
    0x64, 0x69, 0xe0, 0xb7, 0xf1, 0x87, 0xf1, 0xf1, 0x1c, 0x47, 0x05, 0x2a,
    0x21, 0x8e, 0x76, 0x2d, 0xad, 0xdb, 0xfb, 0x9e, 0x99, 0xf6, 0x56, 0x8f,
    0x80, 0xb5, 0x36, 0x4b, 0x85, 0x88, 0xce, 0x35, 0x44, 0x04, 0x00, 0xe8,
    0x0a, 0x39, 0x8c, 0xe8, 0xf9, 0x90, 0x34, 0xd2, 0x29, 0x2c, 0xc3, 0x7c,
    0x8e, 0xbd, 0x53, 0x0f, 0xeb, 0x58, 0x3a, 0x05, 0xe9, 0x54, 0x34, 0x1f,
    0x8a, 0x02, 0x7b, 0x2a, 0x7d, 0x3a, 0x1f, 0x09, 0xbf, 0x85, 0x4d, 0xc5,
    0xd5, 0xd9, 0x53, 0xaa, 0x39, 0x6e, 0x4f, 0x38, 0xca, 0xb1, 0x99, 0xe2,
    0xd2, 0xe1, 0x08, 0xab, 0xe1, 0x56, 0xf8, 0x2e, 0x30, 0x97, 0x7f, 0xcb,
    0x4d, 0x8f, 0xf9, 0x42, 0xd7, 0x5d, 0xbe, 0xbe, 0xd2, 0xe1, 0x43, 0x95,
    0x3a, 0x93, 0xf6, 0xca, 0xad, 0x3d, 0x61, 0x11, 0xf4, 0x4b, 0x7d, 0x4f,
    0x82, 0x0f, 0xf9, 0xc0, 0x06, 0x9b, 0xb5, 0x1e, 0xcd, 0x12, 0x20, 0x66,
    0x74, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
    0x82
};

static const unsigned char image3_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0x9c, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x8d, 0x95, 0xad, 0x76, 0xdb,
    0x40, 0x10, 0x85, 0x3f, 0xf7, 0x18, 0xcc, 0x32, 0x89, 0xd9, 0x50, 0xd0,
    0x61, 0x36, 0x34, 0x4c, 0x98, 0xc3, 0x62, 0x96, 0x40, 0x87, 0x25, 0x6f,
    0x50, 0x3f, 0x42, 0x61, 0x61, 0x02, 0x1b, 0xe6, 0xb2, 0x84, 0x25, 0x50,
    0x61, 0x2e, 0x8b, 0xe1, 0x42, 0x99, 0x49, 0x6c, 0x86, 0x6d, 0xc1, 0x4a,
    0xb2, 0xfc, 0x77, 0xda, 0x21, 0x92, 0x66, 0x57, 0x77, 0xee, 0xdc, 0x3b,
    0x5a, 0xf5, 0x38, 0x13, 0xaf, 0xaf, 0xaf, 0x41, 0x44, 0x48, 0xd3, 0x74,
    0x2f, 0x6f, 0x66, 0x00, 0xa8, 0x2a, 0x00, 0x55, 0x55, 0x91, 0x24, 0x09,
    0x57, 0x57, 0x57, 0xbd, 0xee, 0xbe, 0xfe, 0x39, 0x60, 0x11, 0x61, 0x32,
    0x99, 0xb4, 0x40, 0x87, 0x6b, 0x4d, 0x94, 0x65, 0x89, 0xf7, 0xfe, 0x68,
    0xcf, 0x59, 0x60, 0x80, 0xcd, 0x66, 0x73, 0x04, 0x76, 0x58, 0x48, 0x55,
    0x71, 0xce, 0xfd, 0x3f, 0xf0, 0x29, 0x00, 0x33, 0x3b, 0x2a, 0x70, 0xaa,
    0x23, 0x80, 0x6f, 0xa7, 0x92, 0x79, 0x9e, 0x07, 0x33, 0x6b, 0x99, 0x38,
    0xe7, 0x70, 0xce, 0xed, 0xe9, 0xdd, 0xe8, 0x2f, 0x22, 0x47, 0xfa, 0x9e,
    0x65, 0xac, 0xaa, 0x24, 0x49, 0x42, 0x59, 0x96, 0x88, 0x48, 0x6b, 0x54,
    0x37, 0x4e, 0xb5, 0xff, 0x4f, 0xc6, 0x10, 0x5b, 0x3c, 0x9c, 0x88, 0x2e,
    0x68, 0x53, 0xec, 0x9c, 0x14, 0x27, 0x19, 0x37, 0x6c, 0x4e, 0x31, 0xed,
    0xe6, 0x55, 0x75, 0x6f, 0x42, 0xba, 0x71, 0xa4, 0x0d, 0xc0, 0x6a, 0xb5,
    0x0a, 0x59, 0x96, 0x31, 0x1c, 0x0e, 0xcf, 0x82, 0x37, 0x46, 0x7e, 0x7e,
    0x7e, 0x02, 0x20, 0x92, 0x30, 0x9f, 0x5f, 0xb7, 0x78, 0x7b, 0x8c, 0xdf,
    0xdf, 0xdf, 0x83, 0xf7, 0x9e, 0xfc, 0x23, 0x47, 0x66, 0x82, 0x88, 0xb4,
    0x00, 0x87, 0xd7, 0x86, 0x69, 0x59, 0x94, 0xe4, 0x79, 0xce, 0xb6, 0xda,
    0xf2, 0xf0, 0xf0, 0x10, 0x66, 0xb3, 0x19, 0xd7, 0xd7, 0xd7, 0xbd, 0x5e,
    0x17, 0x74, 0xb3, 0xf1, 0x54, 0xc5, 0x16, 0x35, 0x80, 0xd3, 0x4c, 0x01,
    0x9c, 0xa4, 0x08, 0x02, 0x0e, 0x7c, 0xe1, 0x59, 0xaf, 0xff, 0xb0, 0xdd,
    0x16, 0xa8, 0x1a, 0x17, 0x17, 0x19, 0x8b, 0xc5, 0x22, 0x4a, 0xd1, 0x30,
    0xbd, 0x9c, 0x5e, 0xe2, 0xd2, 0x14, 0x55, 0x03, 0x53, 0x8e, 0x6c, 0x31,
    0x03, 0x84, 0x9c, 0x4f, 0x3e, 0x78, 0x65, 0x6a, 0x53, 0xd2, 0xaf, 0x94,
    0xe7, 0x97, 0x67, 0xfc, 0x57, 0xfc, 0xfa, 0xd4, 0x94, 0x6c, 0x74, 0x11,
    0x41, 0x9f, 0x9e, 0x7e, 0x85, 0xb2, 0x28, 0xc3, 0xff, 0xc4, 0x57, 0xf8,
    0x0a, 0xa3, 0x30, 0x0a, 0x12, 0x24, 0x8c, 0xc2, 0x28, 0xac, 0xd7, 0xeb,
    0xf0, 0xe3, 0xfb, 0xcf, 0x30, 0x1e, 0x8f, 0xc3, 0x60, 0x90, 0x85, 0x24,
    0x49, 0x42, 0x36, 0xc8, 0x42, 0xbf, 0xda, 0x56, 0xdc, 0xdd, 0xdd, 0x9c,
    0x75, 0xf7, 0x30, 0x52, 0x52, 0x2e, 0x99, 0x92, 0x23, 0xcc, 0x98, 0x31,
    0x1e, 0x8f, 0x49, 0x64, 0x48, 0x69, 0x05, 0xcf, 0xbf, 0x5e, 0xa8, 0xaa,
    0x8a, 0x74, 0x90, 0xd2, 0x37, 0xc0, 0xfb, 0x22, 0xce, 0xa3, 0x19, 0x88,
    0x10, 0x6b, 0x48, 0xed, 0x36, 0x38, 0x5c, 0x54, 0xdc, 0x14, 0xc4, 0xf1,
    0x60, 0xdf, 0xb9, 0xc1, 0x33, 0xb4, 0x21, 0x7f, 0xd8, 0x80, 0x19, 0xe9,
    0x70, 0x18, 0xd7, 0x6b, 0x77, 0xfa, 0x65, 0x51, 0xe0, 0x45, 0xa2, 0x9e,
    0x66, 0xb4, 0xbe, 0x39, 0x88, 0x2e, 0xd6, 0x9d, 0x38, 0x03, 0x15, 0x20,
    0xe6, 0x04, 0xf0, 0xb6, 0xc5, 0x88, 0x67, 0x88, 0xdf, 0x6c, 0x5a, 0x4f,
    0x1c, 0xf5, 0xb8, 0x35, 0x09, 0x6b, 0x00, 0xb1, 0x76, 0x28, 0x14, 0x8b,
    0x35, 0x74, 0x6f, 0x67, 0x3b, 0x39, 0xd2, 0x78, 0xda, 0x09, 0x45, 0xe9,
    0x23, 0x60, 0x65, 0xe7, 0x05, 0xad, 0xc9, 0x76, 0x37, 0x1a, 0x20, 0x0a,
    0x76, 0xb8, 0xe2, 0x30, 0x2b, 0xa9, 0xfb, 0x6c, 0x7a, 0x63, 0x32, 0x99,
    0xf2, 0x0d, 0xeb, 0xb0, 0x6c, 0xc9, 0x6a, 0x7c, 0xb4, 0xfa, 0xba, 0x07,
    0xea, 0x9a, 0x6d, 0x35, 0x68, 0x0d, 0x58, 0xcb, 0x39, 0x18, 0x0c, 0x58,
    0x2c, 0xee, 0x22, 0x63, 0xef, 0x7d, 0x63, 0x15, 0x88, 0x41, 0x25, 0x40,
    0x15, 0x9d, 0x33, 0x8b, 0x30, 0xd2, 0xb0, 0xb2, 0x1d, 0x18, 0x3b, 0xcd,
    0x31, 0x43, 0x04, 0x96, 0xcb, 0x25, 0xf3, 0xf9, 0xbc, 0xd7, 0xcf, 0xb2,
    0x8c, 0x8f, 0xb7, 0x0f, 0x7e, 0xbf, 0xbd, 0xa1, 0x6a, 0xc4, 0xf3, 0x47,
    0xd8, 0x1b, 0x3e, 0xe9, 0x3c, 0xcb, 0x0e, 0xb2, 0xed, 0xb3, 0x9e, 0xa6,
    0xe5, 0x72, 0xc9, 0xe3, 0xe3, 0x63, 0x0f, 0x3a, 0x87, 0xd0, 0x6a, 0xb5,
    0x0a, 0xab, 0xd5, 0x1b, 0xdb, 0xfa, 0xff, 0xa5, 0x68, 0x6d, 0xca, 0xce,
    0x99, 0xdd, 0x5f, 0x03, 0x54, 0xcb, 0x78, 0x5f, 0x19, 0x93, 0xe9, 0x84,
    0xdb, 0xdb, 0x5b, 0xee, 0xef, 0xef, 0x5b, 0xbc, 0xbf, 0xd1, 0xf6, 0x9e,
    0x0c, 0x0f, 0x84, 0x48, 0x03, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image4_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x73, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x92, 0xc1, 0x0e, 0x80,
    0x20, 0x08, 0x40, 0xb1, 0xef, 0x66, 0x1d, 0x1c, 0xff, 0x4d, 0x87, 0x6a,
    0xa8, 0x88, 0xa1, 0x76, 0x69, 0xf9, 0x36, 0x0f, 0x28, 0x3e, 0xd8, 0x00,
    0x60, 0xf1, 0x59, 0x42, 0x5f, 0x3a, 0x71, 0xf5, 0x36, 0x02, 0xe0, 0x8e,
    0x99, 0x2b, 0x09, 0x88, 0x01, 0xd0, 0x28, 0x54, 0x17, 0x6a, 0xe4, 0x7f,
    0x21, 0xce, 0x1f, 0xb5, 0xb0, 0x5d, 0x38, 0xed, 0xdc, 0x90, 0x60, 0xd0,
    0xf1, 0x13, 0x79, 0x63, 0x5b, 0x3b, 0xc9, 0x2b, 0xd5, 0x18, 0xe2, 0x39,
    0xa9, 0x43, 0xec, 0x1d, 0x5a, 0xb7, 0x78, 0x5c, 0xee, 0x10, 0x7b, 0xe4,
    0xb2, 0x15, 0xaf, 0x40, 0x91, 0xf8, 0x94, 0xde, 0x47, 0x18, 0x1e, 0xce,
    0xa5, 0x9e, 0xde, 0x9e, 0xc5, 0x9f, 0x38, 0x00, 0x62, 0xac, 0x28, 0xb1,
    0x0f, 0xe7, 0x4f, 0xef, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82
};

static const unsigned char image5_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x74, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x92, 0xc1, 0x0a, 0xc0,
    0x20, 0x08, 0x40, 0x6d, 0xdf, 0x2d, 0x3b, 0x84, 0xff, 0xed, 0x0e, 0xa3,
    0x58, 0x6a, 0x26, 0xd1, 0x65, 0xe0, 0x83, 0x0e, 0xa5, 0x3e, 0x85, 0x04,
    0x48, 0x7e, 0x4b, 0x91, 0x0f, 0x54, 0x89, 0xf1, 0x9e, 0xa5, 0xa3, 0xca,
    0x0f, 0x8a, 0x89, 0x63, 0x65, 0xb3, 0x06, 0xc4, 0x2d, 0xd6, 0x13, 0xc6,
    0x49, 0xbd, 0xc2, 0x59, 0x83, 0x16, 0x13, 0x62, 0x19, 0xf0, 0xf9, 0x36,
    0xc0, 0xa2, 0xef, 0x00, 0xd7, 0x5a, 0x62, 0x61, 0x4d, 0x3a, 0xb2, 0x29,
    0x96, 0xf2, 0xa3, 0x62, 0xff, 0xa3, 0x37, 0xc5, 0xeb, 0xed, 0xe9, 0x62,
    0xaa, 0xd1, 0xa2, 0xe8, 0x4a, 0xaa, 0xa2, 0xf7, 0x50, 0xdd, 0x12, 0x74,
    0x8c, 0x0f, 0xd0, 0xab, 0x93, 0x24, 0x67, 0x78, 0x00, 0x59, 0x6e, 0x28,
    0xb1, 0xbb, 0x69, 0xe3, 0x51, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image6_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x82, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xcd, 0xd3, 0x41, 0x12, 0x80,
    0x20, 0x08, 0x05, 0x50, 0xe8, 0xe0, 0x2e, 0xbc, 0x38, 0xad, 0x32, 0x73,
    0x50, 0x3e, 0x48, 0x53, 0x7f, 0xe3, 0xe4, 0x8c, 0x4f, 0x24, 0x25, 0xfa,
    0x28, 0xe2, 0x9c, 0x6f, 0x39, 0x92, 0x0b, 0xf9, 0x27, 0x6c, 0xb6, 0x01,
    0x85, 0x35, 0x88, 0x77, 0x61, 0x13, 0x88, 0xc2, 0x57, 0x64, 0x18, 0xcd,
    0xa0, 0x15, 0xf5, 0x20, 0xb4, 0xe6, 0xb5, 0x5b, 0xe1, 0x09, 0xdc, 0x06,
    0x22, 0xb8, 0xe2, 0x2a, 0xcf, 0x31, 0x05, 0x6e, 0x18, 0xdf, 0xdf, 0xf8,
    0x06, 0x06, 0xaa, 0x55, 0x1c, 0xc6, 0x35, 0x64, 0xc4, 0xdc, 0xf8, 0x0c,
    0xd0, 0x20, 0x1d, 0x57, 0x7a, 0x5c, 0x85, 0xa8, 0x84, 0x5f, 0xdc, 0x02,
    0x5e, 0xa5, 0x30, 0x7a, 0xfc, 0xcd, 0x07, 0xe2, 0x3a, 0x1d, 0xf2, 0x83,
    0xec, 0x2b, 0x37, 0xd9, 0xad, 0x5f, 0xb4, 0xdf, 0xef, 0xd4, 0x9c, 0xfb,
    0xf7, 0x2f, 0xac, 0xb6, 0x51, 0xb8, 0x9d, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image7_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xbf, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xd5, 0x93, 0x41, 0x0a, 0x83,
    0x30, 0x10, 0x45, 0xdf, 0x48, 0x8e, 0xe5, 0x1d, 0xbc, 0x8c, 0x3b, 0xa9,
    0x8b, 0xf4, 0x6a, 0x9e, 0xab, 0xd3, 0x85, 0x35, 0x0d, 0x26, 0x63, 0x62,
    0x44, 0x4a, 0x3f, 0x0c, 0x42, 0x66, 0xfc, 0xf3, 0xf8, 0x24, 0xf0, 0x6f,
    0x12, 0x40, 0x2b, 0x66, 0xda, 0x8c, 0x55, 0xf3, 0xde, 0x22, 0x12, 0xcf,
    0x9d, 0x92, 0xcb, 0x98, 0xc0, 0xba, 0x2d, 0x7c, 0x45, 0x44, 0xcf, 0x9a,
    0x07, 0x63, 0x8b, 0xba, 0xd5, 0x3c, 0x44, 0x91, 0x23, 0x5e, 0xcf, 0x7c,
    0xc1, 0x62, 0x36, 0x97, 0xa9, 0x25, 0x40, 0xc1, 0x1f, 0xf4, 0xfd, 0xa7,
    0x52, 0x75, 0x01, 0x5d, 0x24, 0xa9, 0x38, 0x9e, 0x7d, 0x6f, 0x53, 0xdf,
    0x4f, 0xe4, 0xcc, 0xab, 0x32, 0x3e, 0xea, 0x0f, 0x03, 0xc0, 0xc4, 0xb2,
    0xa0, 0x71, 0x2c, 0xe6, 0xad, 0xd8, 0x9b, 0x59, 0xb7, 0x66, 0x1c, 0x3b,
    0xe0, 0x95, 0x98, 0x5f, 0x26, 0x16, 0x79, 0xee, 0x4e, 0xbc, 0xc2, 0x2c,
    0x97, 0x88, 0x55, 0x1f, 0xe6, 0xa2, 0xcb, 0xc4, 0x96, 0x9a, 0x89, 0x4b,
    0xcb, 0x6f, 0x23, 0xee, 0x36, 0x1a, 0xab, 0x62, 0xe2, 0x52, 0xc5, 0x72,
    0x94, 0xdf, 0xbf, 0xb6, 0x10, 0xbb, 0xf2, 0xc8, 0x97, 0xb8, 0xa4, 0x6c,
    0xc6, 0x67, 0x7e, 0xaa, 0x51, 0x95, 0x71, 0xfa, 0x08, 0x7e, 0xa8, 0x37,
    0x62, 0xda, 0x9a, 0xba, 0x45, 0xb6, 0x37, 0x52, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image8_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xd5, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xc5, 0x95, 0x5d, 0x0a, 0x84,
    0x30, 0x0c, 0x84, 0x27, 0xe2, 0xa9, 0x0a, 0x9e, 0x6c, 0x8b, 0x0f, 0x4b,
    0x3d, 0xd9, 0x82, 0xd7, 0xca, 0x3e, 0x58, 0xd7, 0xfe, 0x4c, 0xd0, 0xba,
    0x5d, 0x76, 0x40, 0x02, 0x4d, 0xf2, 0x65, 0xda, 0x0a, 0x05, 0x7e, 0x24,
    0x39, 0xc9, 0xeb, 0x8d, 0x9e, 0xaa, 0x88, 0x41, 0xa0, 0xc9, 0xaa, 0xd8,
    0xc8, 0x2a, 0xb3, 0x2f, 0x9c, 0x42, 0x5b, 0xe1, 0xe3, 0x0e, 0x0d, 0xcf,
    0x00, 0xc0, 0x03, 0x08, 0xf0, 0xb3, 0xa7, 0xa0, 0x74, 0x10, 0xa9, 0xd7,
    0x14, 0x2e, 0x00, 0xb4, 0x2c, 0x5a, 0x5f, 0xab, 0x69, 0x6b, 0x97, 0x9b,
    0x1c, 0x83, 0x7f, 0xc0, 0xc3, 0x16, 0xb6, 0xe4, 0x16, 0x5b, 0x64, 0xf7,
    0x8d, 0x71, 0x63, 0x59, 0x91, 0x9b, 0xdc, 0x45, 0x70, 0xde, 0x47, 0xc0,
    0x47, 0x32, 0xdd, 0x5e, 0x5b, 0xcc, 0x35, 0xf0, 0xc9, 0x77, 0x62, 0xae,
    0x78, 0x79, 0x36, 0xdc, 0xcf, 0x75, 0x13, 0x57, 0x7e, 0x79, 0xf4, 0x8c,
    0x4b, 0x27, 0xaa, 0x0f, 0x13, 0x27, 0xb2, 0x40, 0xf5, 0x11, 0x7f, 0xcb,
    0xe3, 0x48, 0xaa, 0x33, 0xb6, 0xe0, 0x22, 0x4b, 0x05, 0x4d, 0x07, 0x46,
    0xb8, 0x02, 0x5e, 0x2e, 0x3b, 0x3e, 0x73, 0xcd, 0xe0, 0xdd, 0x1c, 0x97,
    0xf0, 0x2e, 0x8e, 0xd9, 0xd0, 0xaf, 0x1d, 0xb3, 0x81, 0x22, 0x4b, 0xdf,
    0x33, 0xee, 0xe6, 0x98, 0xa9, 0x34, 0xa0, 0xf6, 0x17, 0xb4, 0x55, 0x40,
    0xd0, 0x0b, 0xcf, 0x4c, 0xa0, 0x8f, 0xc0, 0xdf, 0xf4, 0x06, 0xe3, 0x25,
    0xc1, 0x98, 0xb1, 0x6b, 0x67, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image9_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0x5d, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xd5, 0x93, 0xa1, 0x72, 0xdb,
    0x40, 0x10, 0x86, 0x3f, 0x67, 0x0a, 0x56, 0xec, 0xc4, 0x22, 0x78, 0xd0,
    0x65, 0x36, 0x93, 0xa0, 0xc2, 0x1c, 0x68, 0xd6, 0xc2, 0xe6, 0x0d, 0xf2,
    0x1a, 0x81, 0x81, 0x11, 0x34, 0x94, 0x99, 0xc2, 0x54, 0xa8, 0x32, 0x9b,
    0x55, 0xf0, 0xe0, 0x89, 0xdd, 0xb1, 0x5b, 0xa6, 0x02, 0xb7, 0x9d, 0x66,
    0x92, 0xd6, 0x99, 0xb6, 0xd3, 0x99, 0xfe, 0xe8, 0x6e, 0x67, 0xe7, 0xdb,
    0x7f, 0x77, 0xef, 0xe0, 0x7f, 0xd3, 0xe2, 0xc7, 0x4b, 0xd7, 0x75, 0xb3,
    0x73, 0x0e, 0xef, 0x3d, 0x51, 0x15, 0x00, 0x23, 0x82, 0xb5, 0x16, 0x6b,
    0x2d, 0x57, 0x57, 0x57, 0x8b, 0x17, 0x29, 0xbf, 0x02, 0xb7, 0x6d, 0x3b,
    0x0f, 0x87, 0x03, 0xb9, 0x2d, 0x58, 0xae, 0xd7, 0x60, 0x04, 0x00, 0xef,
    0x1c, 0xe3, 0xc7, 0x03, 0x06, 0xa8, 0xaa, 0x8a, 0xeb, 0xeb, 0xeb, 0x57,
    0xc1, 0x17, 0xdf, 0xa0, 0x6d, 0xdb, 0x52, 0x5d, 0xd7, 0x54, 0xef, 0xb6,
    0x00, 0xa8, 0x2a, 0x49, 0x13, 0x8a, 0x12, 0x35, 0x32, 0xec, 0x3a, 0xc4,
    0x2b, 0x9b, 0xcd, 0xe6, 0x55, 0xce, 0x2f, 0xfa, 0xbe, 0x9f, 0x87, 0xc3,
    0x40, 0xfd, 0xe1, 0x3d, 0xcb, 0x4d, 0x8d, 0xaa, 0xa2, 0x4e, 0x48, 0xee,
    0x12, 0xc6, 0x82, 0x38, 0x08, 0xc1, 0x07, 0x96, 0x9b, 0x1a, 0x8a, 0x9c,
    0xe3, 0xf1, 0xf8, 0xaa, 0x51, 0x5c, 0x38, 0xe7, 0xc8, 0xad, 0xa5, 0xaa,
    0x6b, 0x00, 0xc4, 0x5f, 0x12, 0x9c, 0x67, 0xd2, 0x23, 0x93, 0x8c, 0x88,
    0xe6, 0xc8, 0x60, 0xd1, 0x18, 0xb1, 0xd5, 0x92, 0xd1, 0x39, 0xba, 0xae,
    0x9b, 0xcf, 0x83, 0xa7, 0x89, 0x65, 0xb5, 0x46, 0x51, 0x34, 0x80, 0x1b,
    0x1d, 0x2e, 0x1f, 0x49, 0x45, 0xc0, 0xe3, 0x50, 0x09, 0x64, 0x08, 0xea,
    0x15, 0x44, 0x90, 0xc2, 0xe0, 0xbd, 0x3f, 0xef, 0x58, 0x53, 0xc2, 0xe4,
    0x86, 0xa0, 0x01, 0x9f, 0x4d, 0x84, 0xf5, 0x84, 0x18, 0x41, 0x83, 0x62,
    0xb0, 0x40, 0x8e, 0x8b, 0x23, 0xc9, 0x24, 0x50, 0x10, 0x93, 0x31, 0x4d,
    0xd3, 0x59, 0xf0, 0x1b, 0x80, 0x98, 0x14, 0x11, 0x20, 0x25, 0x14, 0x40,
    0x15, 0xf1, 0x96, 0x4c, 0x0b, 0xbc, 0x1b, 0x48, 0x4b, 0x07, 0xe4, 0x68,
    0x88, 0x80, 0xc0, 0x29, 0xeb, 0xd7, 0x8e, 0x41, 0x41, 0xf5, 0xb4, 0x34,
    0xfd, 0x76, 0x86, 0x4c, 0x05, 0x3f, 0x1e, 0x08, 0x4b, 0x0f, 0x85, 0x80,
    0x26, 0x54, 0x40, 0x63, 0x40, 0x44, 0xce, 0x83, 0x8b, 0xbc, 0xc0, 0x39,
    0x87, 0xa6, 0x13, 0x50, 0xa3, 0xa2, 0x28, 0x5e, 0x1d, 0x5a, 0x44, 0x14,
    0xd0, 0x70, 0x8a, 0xa5, 0x98, 0x08, 0x21, 0x62, 0xad, 0x3d, 0x0f, 0xb6,
    0xd6, 0xe2, 0x87, 0xcf, 0xa4, 0x98, 0x50, 0x8d, 0x27, 0x40, 0x50, 0x44,
    0x73, 0x70, 0x42, 0x8c, 0x91, 0xaf, 0x8d, 0x10, 0xfd, 0x44, 0x81, 0x60,
    0x8c, 0x39, 0x0b, 0x5e, 0x00, 0xdc, 0xdd, 0xdd, 0xcd, 0x8e, 0x80, 0xa9,
    0xde, 0x42, 0x02, 0x48, 0xe8, 0x04, 0x84, 0x08, 0x56, 0xf0, 0x3e, 0x02,
    0x90, 0x7d, 0x72, 0x94, 0x65, 0xc9, 0xba, 0x5a, 0xe3, 0x46, 0x87, 0x31,
    0xe6, 0xa7, 0x9f, 0xe5, 0x02, 0x60, 0xb5, 0x5a, 0x61, 0x02, 0xc4, 0xee,
    0x40, 0xa6, 0x89, 0x4c, 0x33, 0xf2, 0xcb, 0x0c, 0xb1, 0x06, 0x51, 0x28,
    0x14, 0xf8, 0xf8, 0x99, 0xb2, 0x2c, 0xb9, 0xb9, 0xb9, 0x59, 0xb8, 0xd1,
    0xf1, 0xf8, 0xf8, 0x48, 0xd3, 0x34, 0xb4, 0x6d, 0xfb, 0xe2, 0x9b, 0xfe,
    0x5e, 0xad, 0xef, 0xfb, 0xf9, 0x78, 0x3c, 0x32, 0x3a, 0x87, 0x18, 0x81,
    0xec, 0xb4, 0x20, 0x0d, 0x11, 0x51, 0xa8, 0xeb, 0x9a, 0xed, 0x76, 0xbb,
    0x00, 0x18, 0x86, 0x61, 0xee, 0xba, 0x8e, 0xfd, 0x7e, 0x8f, 0x31, 0x86,
    0xed, 0x76, 0xcb, 0x6a, 0xb5, 0x7a, 0xe2, 0xfe, 0x59, 0x1b, 0x5d, 0xd7,
    0xcd, 0xde, 0x7b, 0x62, 0x8c, 0x88, 0x08, 0x79, 0x9e, 0x63, 0xad, 0xa5,
    0xaa, 0xaa, 0x67, 0xb9, 0xbb, 0xdd, 0x6e, 0x6e, 0x9a, 0x06, 0xef, 0x3d,
    0x75, 0x5d, 0x3f, 0x29, 0xfe, 0xc7, 0xea, 0xfb, 0x7e, 0xbe, 0xbd, 0xbd,
    0x9d, 0xad, 0xb5, 0x73, 0x59, 0x96, 0xf3, 0xfd, 0xfd, 0xfd, 0xfc, 0xa2,
    0xe3, 0xdf, 0xd5, 0xc3, 0xc3, 0xc3, 0xdc, 0x34, 0x0d, 0xd3, 0x34, 0xb1,
    0xd9, 0x6c, 0xfe, 0x1e, 0x18, 0x4e, 0x63, 0xdc, 0xef, 0xf7, 0xa4, 0x94,
    0xfe, 0x26, 0xf6, 0x1f, 0xe9, 0x0b, 0xbc, 0x4c, 0x5e, 0x59, 0xb8, 0xac,
    0x89, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
    0x60, 0x82
};


/*
 *  Constructs a CHEFPlotMainBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
CHEFPlotMainBase::CHEFPlotMainBase( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    img.loadFromData( image1_data, sizeof( image1_data ), "PNG" );
    image1 = img;
    img.loadFromData( image2_data, sizeof( image2_data ), "PNG" );
    image2 = img;
    img.loadFromData( image3_data, sizeof( image3_data ), "PNG" );
    image3 = img;
    img.loadFromData( image4_data, sizeof( image4_data ), "PNG" );
    image4 = img;
    img.loadFromData( image5_data, sizeof( image5_data ), "PNG" );
    image5 = img;
    img.loadFromData( image6_data, sizeof( image6_data ), "PNG" );
    image6 = img;
    img.loadFromData( image7_data, sizeof( image7_data ), "PNG" );
    image7 = img;
    img.loadFromData( image8_data, sizeof( image8_data ), "PNG" );
    image8 = img;
    img.loadFromData( image9_data, sizeof( image9_data ), "PNG" );
    image9 = img;
    if ( !name )
	setName( "CHEFPlotMainBase" );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( image0 ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( image1 ) );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( image2 ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( image3 ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( image4 ) );
    editRedoAction = new QAction( this, "editRedoAction" );
    editRedoAction->setIconSet( QIconSet( image5 ) );
    editCutAction = new QAction( this, "editCutAction" );
    editCutAction->setIconSet( QIconSet( image6 ) );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( image7 ) );
    editPasteAction = new QAction( this, "editPasteAction" );
    editPasteAction->setIconSet( QIconSet( image8 ) );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( image9 ) );
    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpIndexAction = new QAction( this, "helpIndexAction" );
    helpIndexAction->setToggleAction( TRUE );
    helpIndexAction->setOn( TRUE );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    optionsGridAction = new QAction( this, "optionsGridAction" );
    optionsGridAction->setToggleAction( TRUE );
    optionsGridAction->setOn( TRUE );
    optionsThumbWheelsAction = new QAction( this, "optionsThumbWheelsAction" );
    optionsThumbWheelsAction->setToggleAction( TRUE );
    optionsThumbWheelsAction->setOn( FALSE );
    optionsThumbWheelsAction->setEnabled( TRUE );
    optionsScalesAction = new QAction( this, "optionsScalesAction" );
    optionsLegendAction = new QAction( this, "optionsLegendAction" );
    optionsLegendAction->setToggleAction( TRUE );
    optionsFontsAction = new QAction( this, "optionsFontsAction" );
    optionsLego_PlotAction = new QAction( this, "optionsLego_PlotAction" );
    optionsLego_PlotAction->setToggleAction( TRUE );
    optionsLego_PlotAction->setOn( TRUE );
    optionsLego_PlotAction->setEnabled( TRUE );
    optionsZoomAction = new QAction( this, "optionsZoomAction" );
    optionsAxis_Focus_Left_AxisAction = new QAction( this, "optionsAxis_Focus_Left_AxisAction" );
    optionsAxis_Focus_RightAction = new QAction( this, "optionsAxis_Focus_RightAction" );
    optionsto_PyCHEFAction = new QAction( this, "optionsto_PyCHEFAction" );
    optionsShow_DataAction = new QAction( this, "optionsShow_DataAction" );
    optionsShow_DataAction->setToggleAction( TRUE );
    optionsShow_DataAction->setOn( FALSE );
    optionsInterpolationAction = new QAction( this, "optionsInterpolationAction" );
    optionsInterpolationAction->setToggleAction( TRUE );
    optionsInterpolationAction->setOn( TRUE );
    optionsCoordinatesAction = new QAction( this, "optionsCoordinatesAction" );
    optionsCoordinatesAction->setToggleAction( TRUE );
    Action = new QAction( this, "Action" );
    fileAttachAction = new QAction( this, "fileAttachAction" );
    Action_2 = new QAction( this, "Action_2" );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    fileOpenAction->addTo( toolBar );
    fileSaveAction->addTo( toolBar );
    fileSaveAsAction->addTo( toolBar );
    filePrintAction->addTo( toolBar );
    fileExitAction->addTo( toolBar );
    toolBar->addSeparator();


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileAttachAction->addTo( fileMenu );
    Action->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    Options = new QPopupMenu( this );
    PopupMenuEditor_12 = new QPopupMenu( this );
    Options->setAccel( tr( "" ), 
        Options->insertItem( optionsZoomAction->iconSet(), tr( "Axis Focus" ), PopupMenuEditor_12 ) );
    optionsAxis_Focus_Left_AxisAction->addTo( PopupMenuEditor_12 );
    optionsAxis_Focus_RightAction->addTo( PopupMenuEditor_12 );
    optionsShow_DataAction->addTo( Options );
    optionsThumbWheelsAction->addTo( Options );
    optionsLego_PlotAction->addTo( Options );
    optionsGridAction->addTo( Options );
    optionsLegendAction->addTo( Options );
    optionsInterpolationAction->addTo( Options );
    optionsCoordinatesAction->addTo( Options );
    MenuBar->insertItem( QString(""), Options, 2 );

    helpMenu = new QPopupMenu( this );
    helpContentsAction->addTo( helpMenu );
    helpIndexAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    MenuBar->insertItem( QString(""), helpMenu, 3 );

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( helpIndexAction, SIGNAL( activated() ), this, SLOT( helpIndex() ) );
    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( optionsThumbWheelsAction, SIGNAL( toggled(bool) ), this, SLOT( enableThumbWheels(bool) ) );
    connect( optionsGridAction, SIGNAL( toggled(bool) ), this, SLOT( enableGrid(bool) ) );
    connect( optionsLego_PlotAction, SIGNAL( toggled(bool) ), this, SLOT( enableLegoPlot(bool) ) );
    connect( optionsAxis_Focus_RightAction, SIGNAL( activated() ), this, SLOT( zoomUseRightAxis() ) );
    connect( optionsAxis_Focus_Left_AxisAction, SIGNAL( activated() ), this, SLOT( zoomUseLeftAxis() ) );
    connect( optionsShow_DataAction, SIGNAL( toggled(bool) ), this, SLOT( showDataPoints(bool) ) );
    connect( optionsCoordinatesAction, SIGNAL( toggled(bool) ), this, SLOT( enableCoordinatesDisplay(bool) ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( exit() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CHEFPlotMainBase::~CHEFPlotMainBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CHEFPlotMainBase::languageChange()
{
    setCaption( tr( "CHEF" ) );
    fileNewAction->setText( tr( "New" ) );
    fileNewAction->setMenuText( tr( "&New" ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileSaveAction->setText( tr( "Save" ) );
    fileSaveAction->setMenuText( tr( "&Save" ) );
    fileSaveAction->setAccel( tr( "Ctrl+S" ) );
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    fileSaveAsAction->setAccel( QString::null );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    editUndoAction->setText( tr( "Undo" ) );
    editUndoAction->setMenuText( tr( "&Undo" ) );
    editUndoAction->setAccel( tr( "Ctrl+Z" ) );
    editRedoAction->setText( tr( "Redo" ) );
    editRedoAction->setMenuText( tr( "&Redo" ) );
    editRedoAction->setAccel( tr( "Ctrl+Y" ) );
    editCutAction->setText( tr( "Cut" ) );
    editCutAction->setMenuText( tr( "Cu&t" ) );
    editCutAction->setAccel( tr( "Ctrl+X" ) );
    editCopyAction->setText( tr( "Copy" ) );
    editCopyAction->setMenuText( tr( "&Copy" ) );
    editCopyAction->setAccel( tr( "Ctrl+C" ) );
    editPasteAction->setText( tr( "Paste" ) );
    editPasteAction->setMenuText( tr( "&Paste" ) );
    editPasteAction->setAccel( tr( "Ctrl+V" ) );
    editFindAction->setText( tr( "Find" ) );
    editFindAction->setMenuText( tr( "&Find..." ) );
    editFindAction->setAccel( tr( "Ctrl+F" ) );
    helpContentsAction->setText( tr( "Contents" ) );
    helpContentsAction->setMenuText( tr( "&Contents..." ) );
    helpContentsAction->setAccel( QString::null );
    helpIndexAction->setText( tr( "Index" ) );
    helpIndexAction->setMenuText( tr( "&Index..." ) );
    helpIndexAction->setAccel( QString::null );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About" ) );
    helpAboutAction->setAccel( QString::null );
    optionsGridAction->setText( tr( "Grid" ) );
    optionsGridAction->setMenuText( tr( "Grid" ) );
    optionsThumbWheelsAction->setText( tr( "Thumbwheels" ) );
    optionsThumbWheelsAction->setMenuText( tr( "Thumbwheels" ) );
    optionsScalesAction->setText( tr( "junk" ) );
    optionsScalesAction->setMenuText( tr( "junk" ) );
    optionsLegendAction->setText( tr( "Legend" ) );
    optionsLegendAction->setMenuText( tr( "Legend" ) );
    optionsFontsAction->setText( tr( "Fonts" ) );
    optionsFontsAction->setMenuText( tr( "Fonts" ) );
    optionsLego_PlotAction->setText( tr( "Lego Plot" ) );
    optionsLego_PlotAction->setMenuText( tr( "Lego Plot" ) );
    optionsZoomAction->setText( tr( "Axis Focus" ) );
    optionsZoomAction->setMenuText( tr( "Axis Focus" ) );
    optionsAxis_Focus_Left_AxisAction->setText( tr( "Left" ) );
    optionsAxis_Focus_Left_AxisAction->setMenuText( tr( "Left" ) );
    optionsAxis_Focus_RightAction->setText( tr( "Right" ) );
    optionsAxis_Focus_RightAction->setMenuText( tr( "Right" ) );
    optionsto_PyCHEFAction->setText( tr( "to PyCHEF" ) );
    optionsto_PyCHEFAction->setMenuText( tr( "to PyCHEF" ) );
    optionsShow_DataAction->setText( tr( "Show Data" ) );
    optionsShow_DataAction->setMenuText( tr( "Show Data" ) );
    optionsInterpolationAction->setText( tr( "Interpolation" ) );
    optionsInterpolationAction->setMenuText( tr( "Interpolation" ) );
    optionsCoordinatesAction->setText( tr( "Coordinates" ) );
    optionsCoordinatesAction->setMenuText( tr( "Coordinates" ) );
    Action->setText( tr( "Detach" ) );
    fileAttachAction->setText( tr( "Attach" ) );
    fileAttachAction->setMenuText( tr( "Attach" ) );
    Action_2->setText( tr( "Unnamed" ) );
    toolBar->setLabel( tr( "Tools" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
    Options->changeItem( Options->idAt( 0 ), tr( "Axis Focus" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "Options" ) );
    if (MenuBar->findItem(3))
        MenuBar->findItem(3)->setText( tr( "&Help" ) );
}

