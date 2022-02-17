/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      user_glcd_fonts.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This file allows fonts to be extended by user defined special characters.
    Most standard fonts contain ASCII values starting at ' ' and ending at '~', although some may have more or less.
    Free ASCII (8-bit) locations after the end of the standard font table can be used to extend the available characters with any
    user-defined special characters. These are not standardised since extended ASCII sets are often not compatible in any case.
    Small user-definable, and non-standard extensions, add flexibility without the need to have a complete and large set of further characters.

*/ 

#ifdef USER_SPECIAL_FONT_NINE_DOT

		// 'è' charwidth: 8 
		0x20, 	//  [  *     ]  
		0x10, 	//  [   *    ]  
		0x00, 	//  [        ]  
		0x3C, 	//  [  ****  ]  
		0x66, 	//  [ **  ** ]  
		0x66, 	//  [ **  ** ]  
		0x7E, 	//  [ ****** ]  
		0x60, 	//  [ **     ]  
		0x60, 	//  [ **     ]  
		0x3C, 	//  [  ****  ]  
		0x00, 	//  [        ]  
		0x00, 	//  [        ]  

		// 'é' charwidth: 8 
		0x08, 	//  [    *   ]  
		0x10, 	//  [   *    ]  
		0x00, 	//  [        ]  
		0x3C, 	//  [  ****  ]  
		0x66, 	//  [ **  ** ]  
		0x66, 	//  [ **  ** ]  
		0x7E, 	//  [ ****** ]  
		0x60, 	//  [ **     ]  
		0x60, 	//  [ **     ]  
		0x3C, 	//  [  ****  ]  
		0x00, 	//  [        ]  
		0x00, 	//  [        ]  

		// 'ê' charwidth: 8 
		0x08, 	//  [    *   ]  
		0x14, 	//  [   * *  ]  
		0x00, 	//  [        ]  
		0x3C, 	//  [  ****  ]  
		0x66, 	//  [ **  ** ]  
		0x66, 	//  [ **  ** ]  
		0x7E, 	//  [ ****** ]  
		0x60, 	//  [ **     ]  
		0x60, 	//  [ **     ]  
		0x3C, 	//  [  ****  ]  
		0x00, 	//  [        ]  
		0x00, 	//  [        ]  
#endif
